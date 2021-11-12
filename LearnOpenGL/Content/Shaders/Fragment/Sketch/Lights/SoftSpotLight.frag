#version 460 core

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	
} frag_in;

uniform struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
} material;

uniform struct Light
{
	vec3 position;			// possible value: camera.Position
	vec3 direction;			// possible value: camera.Front
	float cutOff;			// possible value: cos(radians(12.5f))
	float outerCutOff;		// possible value: cos(radians(17.5f))

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;			// possible value: 1.f
	float linear;			// possible value: 0.09f
	float quadratic;		// possible value: 0.032f
} light;

layout (std140) uniform ULight
{
	vec3 viewPos;
	bool useBlinn;
} u_light;

out vec4 FragColor;

void main()
{
	vec3 ambient, diffuse, specular;
	float intensity, attenuation;
	
	vec3 lightDir = normalize(light.position - frag_in.FragPos);
	vec3 norm = normalize(frag_in.Normal);

	// ambient
	{
		ambient = light.ambient * texture(material.diffuse, frag_in.TexCoord).rgb;
	}

	// diffuse
	{
		float diff = max(dot(norm, lightDir), 0.f);
		diffuse = light.diffuse * diff * texture(material.diffuse, frag_in.TexCoord).rgb;
	}

	// specular
	{
		vec3 viewDir = normalize(u_light.viewPos - frag_in.FragPos);
		
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
		
		specular = light.specular * spec * texture(material.specular, frag_in.TexCoord).rgb;
	}

	// spotlight (soft edges)
	{
		float theta = dot(lightDir, normalize(-light.direction));
		float epsilon = (light.cutOff - light.outerCutOff);
		intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);
	}

	// attenuation
	{
		float distance = length(light.position - frag_in.FragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance);
	}

	FragColor = vec4(attenuation * intensity * (diffuse + specular) + ambient, 1.f);
}