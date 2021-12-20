#version 460 core

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
	
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
} light;

layout (std140) uniform ULight
{
	vec4 viewPos;
	bool useBlinn;
} u_light;

out vec4 FragColor;

uniform bool useOverrideColor;
uniform vec4 overrideColor;

// Shadow
uniform sampler2D shadowMap;

// Is in shadow
float CalculateShadow(vec4 FragPositionInLightSpace)
{
	vec3 projCoords = FragPositionInLightSpace.xyz / FragPositionInLightSpace.w;
	
	projCoords = projCoords * 0.5 + 0.5;
	
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	return (currentDepth > closestDepth)  ? 1.0 : 0.0;
}

vec4 CalculateDirectionalLight()
{
	float shadow;
	vec3 ambient, diffuse, specular;
	
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

	// Lightning
	{
		shadow = CalculateShadow(frag_in.FragPosLightSpace);
	}

	return vec4(ambient + ((diffuse + specular) * (1.f - shadow)), 1.f);
}

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : CalculateDirectionalLight();
	if(resultColor.a < 0.1f)
		discard;

	FragColor = resultColor;
}