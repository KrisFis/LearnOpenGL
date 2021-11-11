#version 460 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 direction;// possible value: 0.2f, -1.0f, -0.3f

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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
	
	// ambient
	{
		ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;
	}

	// diffuse
	{
		vec3 norm = normalize(fs_in.Normal);
		vec3 lightDir = normalize(-light.direction);
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
		
		specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	}

	FragColor = vec4(ambient + diffuse + specular + emission, 1.f);
}