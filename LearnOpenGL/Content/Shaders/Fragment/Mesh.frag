#version 460 core

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

	float constant;			// possible value: 1.f
	float linear;			// possible value: 0.09f
	float quadratic;		// possible value: 0.032f
} light;

layout (std140) uniform ULight
{
	vec3 viewPos;
	int useBlinn;
} u_light;

out vec4 FragColor;

uniform bool useOverrideColor;
uniform vec4 overrideColor;

vec4 CalculateSpotLight()
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
		
		specular = vec3(0.f); //light.specular * spec;
	}
	
	// attenuation
	{
		float distance = length(light.position - frag_in.FragPos);
		attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	}
	
	return vec4(attenuation * (ambient + diffuse + specular), 1.f);
}

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : CalculateSpotLight();
	if(resultColor.a < 0.1f)
		discard;

	FragColor = resultColor;
}