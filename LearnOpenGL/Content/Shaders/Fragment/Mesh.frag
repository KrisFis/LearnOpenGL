#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;  

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;

} frag_in;

uniform struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
} material;

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

layout (std140) uniform ULight
{
	vec4 viewPos;
	bool useBlinn;
} u_light;

uniform bool useOverrideColor;
uniform vec4 overrideColor;

vec4 CalculateLight(Light light)
{
	vec3 ambient, diffuse, specular;
	float attenuation;
	
	vec3 lightDir = normalize(light.position - frag_in.FragPos);
	vec3 norm = normalize(frag_in.Normal);
	
	// ambient
	{
		ambient = light.ambient * texture(material.diffuse0, frag_in.TexCoord).rgb;
	}

	// diffuse
	{
		float diff = max(dot(norm, lightDir), 0.f);
		diffuse = light.diffuse * diff * texture(material.diffuse0, frag_in.TexCoord).rgb;
	}

	// specular
	{
		vec3 viewDir = normalize(u_light.viewPos.xyz - frag_in.FragPos);
		
		float spec = 0.f;
		if(u_light.useBlinn)
		{
			vec3 halfwayDir = normalize(lightDir + viewDir);
			spec = pow(max(dot(norm, halfwayDir), 0.f), material.shininess);
		}
		else
		{
			vec3 reflectDir = reflect(-lightDir, norm);
			spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
		}
		
		specular = light.specular * spec * texture(material.specular0, frag_in.TexCoord).rgb;
	}

	// attenuation
	{
		float distance = length(light.position - frag_in.FragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	}

	return vec4(attenuation * (ambient + diffuse + specular), 1.f);
}

vec4 CalculateLights()
{
	vec4 result = vec4(vec3(0.f), 1.f);

	for(int i = 0; i < 3; ++i)
	{
		result.rgb += CalculateLight(lights[i]).rgb;
	}
	
	return result;
}

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : CalculateLights();
	if (resultColor.a < 0.1f)
	discard;

	FragColor = resultColor;
	
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	    if(brightness > 1.0)
	        BrightColor = vec4(FragColor.rgb, 1.f);
	    else
	        BrightColor = vec4(vec3(0.f), 1.f);
}