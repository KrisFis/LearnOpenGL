#version 460 core

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;
layout (location = 2) out vec4 GAlbedoSpec;

in VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
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
	bool steep;

	float scale;

	float minLayers;
	float maxLayers;
} parallax;

uniform float parallaxHScale;
uniform vec3 viewPos;

vec2 GetParallaxMappedTexCoord()
{
	if (material.numOfHeights > 0)
	{
		vec3 viewDir = transpose(frag_in.TBN) * normalize(viewPos - frag_in.FragPos);

		if (parallax.steep)
		{
			float numLayers = mix(parallax.maxLayers, parallax.minLayers, max(dot(vec3(0.f, 0.f, 1.f), viewDir), 0.f));
			float layerH = 1.0 / numLayers;
			float currentLayer = 0.f;

			vec2 p = viewDir.xy * parallax.scale;
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
			float height = texture(material.height0, frag_in.TexCoord).r;
			vec2 p = viewDir.xy / viewDir.z * (height * parallax.scale);

			return frag_in.TexCoord - p;
		}
	}
	else
	{
		return frag_in.TexCoord;
	}
}

vec4 GetNormal(vec2 texCoord)
{
	return (material.numOfNormals > 0) ?
		vec4(normalize(frag_in.TBN * normalize(texture(material.normal0, texCoord).rgb * 2.f - vec3(1.f))), 1.f) :
		vec4(frag_in.Normal, 1.f);// result normal
}

vec4 GetAlbedo(vec2 texCoord)
{
	return (material.numOfDiffuses > 0) ?
		texture(material.diffuse0, texCoord) :
		vec4(1.f, 0.f, 0.f, 1.f);// red as error
}

vec4 GetSpecular(vec2 texCoord)
{
	return (material.numOfSpeculars > 0) ?
		texture(material.specular0, texCoord) :
		vec4(0.f);// No specular
}

bool IsValidTexCoord(vec2 texCoord)
{
	return texCoord.x >= 0.f && texCoord.x <= 1.f && texCoord.y >= 0.f && texCoord.y <= 1.f;
}

void main()
{
	vec2 texCoord = GetParallaxMappedTexCoord();
	if (!IsValidTexCoord(texCoord))
	discard;

	// Position
	GPosition = vec4(frag_in.FragPos, 1.f);

	// Normal
	GNormal = GetNormal(texCoord);

	// Albedo
	GAlbedoSpec.rgb = GetAlbedo(texCoord).rgb;

	// Specular
	GAlbedoSpec.a = GetSpecular(texCoord).r;
}