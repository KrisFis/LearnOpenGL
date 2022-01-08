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

uniform float parallaxHScale;
uniform vec3 viewPos;

vec2 GetParallaxMappedTexCoord(vec2 texCoord, vec3 viewDir)
{
	return (material.numOfHeights > 0) ?
		(texCoord - (viewDir.xy / viewDir.z * (texture(material.height0, texCoord).r * parallaxHScale))) :
		texCoord;
}

vec4 GetNormal(vec2 texCoord)
{
	return (material.numOfNormals > 0) ? 
		vec4(normalize(frag_in.TBN * normalize(texture(material.normal0, texCoord).rgb * 2.f - vec3(1.f))), 1.f) : 
		vec4(frag_in.TBN[2], 1.f); // result normal
}

vec4 GetAlbedo(vec2 texCoord)
{
	return (material.numOfDiffuses > 0) ? 
		texture(material.diffuse0, texCoord) : 
		vec4(1.f, 0.f, 0.f, 1.f); // red as error
}

vec4 GetSpecular(vec2 texCoord)
{
	return (material.numOfSpeculars > 0) ? 
		texture(material.specular0, texCoord) : 
		vec4(0.f); // No specular
}

void main()
{
	vec3 viewDir = normalize(viewPos - frag_in.FragPos);
	vec2 texCoord = GetParallaxMappedTexCoord(frag_in.TexCoord, viewDir * frag_in.TBN);
	
	if(texCoord.x > 1.f || texCoord.y > 1.f || texCoord.x < 0.f || texCoord.y < 0.f)
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