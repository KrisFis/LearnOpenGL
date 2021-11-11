#version 460 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;// possible value: 1.f
	float linear;// possible value: 0.09f
	float quadratic;// possible value: 0.032f
};

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;

} frag_in;

layout (std140) uniform Light
{
	vec3 viewPos;
	Material material;
	Light light;
};

out vec4 FragColor;

#define USE_BLINN 1

void main()
{
	vec3 ambient, diffuse, specular;
	float attenuation;
	
	// ambient
	{
		ambient = light.ambient * texture(material.diffuse, frag_in.TexCoord).rgb;
	}

	// diffuse
	{
		vec3 norm = normalize(frag_in.Normal);
		vec3 lightDir = normalize(light.position - frag_in.FragPos);
		float diff = max(dot(norm, lightDir), 0.f);
		diffuse = light.diffuse * diff * texture(material.diffuse, frag_in.TexCoord).rgb;
	}

	// specular
	{
		vec3 viewDir = normalize(viewPos - frag_in.FragPos);
		
		#if USE_BLINN
		
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.f), material.shininess);
		
		#else
		
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
		
		#endif
		
		specular = light.specular * spec * texture(material.specular, frag_in.TexCoord).rgb;
	}

	// attenuation
	{
		float distance = length(light.position - frag_in.FragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	}

	FragColor = vec4(attenuation * (ambient + diffuse + specular) + emission, 1.f);
}