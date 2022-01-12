#version 460 core

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;
layout (location = 2) out vec4 GAlbedoSpec;

in VERT_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoord;
} frag_in;

uniform struct Material
{
	int numOfDiffuses;
	int numOfSpeculars;
	int numOfNormals;
	int numOfHeights;

	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
	sampler2D height0;
} material;

uniform struct Parallax
{
	float scale;

	float minLayers;
	float maxLayers;
} parallax;

uniform vec3 viewPos;

vec3 PackNormal(vec3 normal)
{
	return (normal + 1.f) * 0.5f; 
}

vec3 UnpackNormal(vec3 normal)
{
	return 2.f * normal - 1.f;
}

bool IsValidTexCoord(vec2 texCoord)
{
	return texCoord.x >= 0.f && texCoord.x <= 1.f && texCoord.y >= 0.f && texCoord.y <= 1.f;
}

vec2 GetParallaxMappedTexCoord()
{
	if (material.numOfHeights > 0)
	{
		mat3 inverseTBN = transpose(frag_in.TBN);

		vec3 viewPosTS = inverseTBN * viewPos;
		vec3 fragPosTS = inverseTBN * frag_in.FragPos;

		vec3 viewDirTS = normalize(viewPosTS - fragPosTS);

		float numLayers = mix(parallax.maxLayers, parallax.minLayers, abs(dot(vec3(0.f, 0.f, 1.f), viewDirTS)));
		float layerH = 1.0 / numLayers;
		float currentLayer = 0.f;

		vec2 p = (viewDirTS.xy / viewDirTS.z) * parallax.scale;
		vec2 deltaTexCoord = p / numLayers;

		vec2  currentTexCoords = frag_in.TexCoord;
		float currentHValue = texture(material.height0, currentTexCoords).r;

		while (currentLayer < currentHValue)
		{
			currentTexCoords -= deltaTexCoord;
			currentHValue = texture(material.height0, currentTexCoords).r;
			currentLayer += layerH;
		}

		vec2 prevTexCoords = currentTexCoords + deltaTexCoord;

		float afterDepth  = currentHValue - currentLayer;
		float beforeDepth = texture(material.height0, prevTexCoords).r - currentLayer + layerH;
		
		float weight = afterDepth / (afterDepth - beforeDepth);
		return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	}
	else
	{
		return frag_in.TexCoord;
	}
}

void main()
{
	vec2 texCoord = GetParallaxMappedTexCoord();

	if (!IsValidTexCoord(texCoord))
		discard;

	// Position
	{
		GPosition = vec4(frag_in.FragPos, 1.f);
	}

	// Normal
	{
		vec3 normal = frag_in.TBN[2];

		// Read from normal map and move to world space
		if(material.numOfNormals > 0)
		{
			normal = texture(material.normal0, texCoord).rgb;
			normal = UnpackNormal(normal); // Unpack
			normal = frag_in.TBN * normal; // To world space
		}

		normal = PackNormal(normal); // Pack
		GNormal = vec4(normal, 1.f);
	}

	// Albedo && Spec
	{
		vec3 albedo = vec3(1.f, 0.f, 0.f); // red as error
		float specular = 0.f;

		if(material.numOfDiffuses > 0)
		{
			albedo = texture(material.diffuse0, texCoord).rgb;
		}

		if(material.numOfSpeculars > 0)
		{
			specular = texture(material.specular0, texCoord).r;
		}

		GAlbedoSpec = vec4(albedo, specular);
	}
}