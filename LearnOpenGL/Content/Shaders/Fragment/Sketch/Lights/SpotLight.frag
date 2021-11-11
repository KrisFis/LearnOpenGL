#version 460 core

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

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;			// possible value: 1.f
	float linear;			// possible value: 0.09f
	float quadratic;		// possible value: 0.032f
} light;

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	
} frag_in;

layout (std140) uniform ULight
{
	vec3 viewPos;
	int useBlinn;
} u_light;

out vec4 FragColor;

void main()
{
	vec3 lightDir = normalize(light.position - frag_in.FragPos);
	if (dot(lightDir, normalize(-light.direction)) < light.cutOff)// remember that we're working with angles as cosines instead of degrees so a '>' is used.
	{
		vec3 ambient, diffuse, specular;
		float attenuation;
		
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
			if(u_light.useBlinn == 1)
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

		// attenuation
		{
			float distance = length(light.position - frag_in.FragPos);
			attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
		}

		FragColor = vec4(attenuation * (diffuse + specular) + ambient, 1.f);
	}
	else
	{
		FragColor = vec4(light.ambient * texture(material.diffuse, frag_in.TexCoord).rgb, 1.f);
	}
}