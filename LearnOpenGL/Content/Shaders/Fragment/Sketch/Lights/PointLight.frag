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
	vec2 TexCoords;

} fs_in;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

#define USE_BLINN 1

void main()
{
	vec3 ambient, diffuse, specular;
	float attenuation;
	
	// ambient
	{
		ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;
	}

	// diffuse
	{
		vec3 norm = normalize(fs_in.Normal);
		vec3 lightDir = normalize(light.position - fs_in.FragPos);
		float diff = max(dot(norm, lightDir), 0.f);
		diffuse = light.diffuse * diff * texture(material.diffuse, fs_in.TexCoords).rgb;
	}

	// specular
	{
		vec3 viewDir = normalize(viewPos - fs_in.FragPos);
		
		#if USE_BLINN
		
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.f), material.shininess);
		
		#else
		
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
		
		#endif
		
		specular = light.specular * spec * texture(material.specular, fs_in.TexCoords).rgb;
	}

	// attenuation
	{
		float distance = length(light.position - fs_in.FragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	}

	FragColor = vec4(attenuation * (ambient + diffuse + specular) + emission, 1.f);
}