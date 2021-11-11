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
	vec3 direction;			// possible value: camera.Front

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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
	
	vec3 lightDir = normalize(-light.direction);
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

	FragColor = vec4(ambient + diffuse + specular, 1.f);
}