#version 460 core

out vec4 FragColor;

in VERT_OUT {
	vec2 TexCoord;
} frag_in;

uniform struct Material
{
	float shininess;
} material;

layout (std140) uniform ULight
{
	vec4 viewPos;
	bool useBlinn;
} u_light;

layout (std140) uniform UPostProcess
{
	float gamma;
	float exposure;
} u_postprocess;

uniform struct GBuffer
{
	sampler2D position;
	sampler2D normal;
	sampler2D albedoSpecular;
} gBuffer;

uniform struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
} lights[3];

struct GBufferTexData
{
	vec3 fragPos;
	vec3 normal;
	vec3 albedo;
	float specular;
};

vec3 UnpackNormal(vec3 normal)
{
	return 2.f * normal - 1.f;
}

vec4 ApplyToneMapping(vec4 InFragColor)
{
	return vec4(vec3(1.f) - exp(-InFragColor.rgb * u_postprocess.exposure), 1.f);
}

vec4 ApplyGamaCorrection(vec4 InFragColor)
{
	return vec4(pow(InFragColor.rgb, vec3(1.f / u_postprocess.gamma)), 1.f);
}

vec4 ApplyPostProcesses(vec4 InFragColor)
{
	InFragColor = ApplyToneMapping(InFragColor);
	InFragColor = ApplyGamaCorrection(InFragColor);
	return InFragColor;
}

vec4 CalculateLight(Light light, GBufferTexData texData)
{
	vec3 ambient, diffuse, specular;
	float attenuation;

	vec3 lightDir = normalize(light.position - texData.fragPos);
	vec3 norm = normalize(texData.normal);

	// ambient
	{
		ambient = light.ambient * texData.albedo;
	}

	// diffuse
	{
		float diff = max(dot(norm, lightDir), 0.f);
		diffuse = light.diffuse * diff * texData.albedo;
	}

	// specular
	{
		vec3 viewDir = normalize(u_light.viewPos.xyz - texData.fragPos);

		float spec = 0.f;
		if (u_light.useBlinn)
		{
			vec3 halfwayDir = normalize(lightDir + viewDir);
			spec = pow(max(dot(norm, halfwayDir), 0.f), material.shininess);
		}
		else
		{
			vec3 reflectDir = reflect(-lightDir, norm);
			spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
		}

		specular = light.specular * spec * texData.specular;
	}

	// attenuation
	{
		float distance = length(light.position - texData.fragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	}
	
	return vec4(attenuation * (ambient + diffuse + specular), 1.f);
}

GBufferTexData ConstructGBufferData()
{
	GBufferTexData result;
	result.fragPos = texture(gBuffer.position, frag_in.TexCoord).rgb;
	result.normal = UnpackNormal(texture(gBuffer.normal, frag_in.TexCoord).rgb);
	result.albedo = texture(gBuffer.albedoSpecular, frag_in.TexCoord).rgb;
	result.specular = texture(gBuffer.albedoSpecular, frag_in.TexCoord).a;
	
	return result;
}

vec4 CalculateLights()
{
	GBufferTexData data = ConstructGBufferData();

	vec3 result = vec3(0.f);

	for (int i = 0; i < 3; ++i)
	{
		result += CalculateLight(lights[i], data).rgb;
	}

	return vec4(result, 1.f);
}

void main()
{
	vec4 lightColor = CalculateLights();
	FragColor = ApplyPostProcesses(lightColor);
}