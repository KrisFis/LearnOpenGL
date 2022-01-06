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
	
	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
} material;

vec4 GetPosition()
{
	return vec4(frag_in.FragPos, 1.f);
}

vec4 GetNormal()
{
	return (material.numOfNormals > 0) ? vec4(normalize(frag_in.TBN * normalize(texture(material.normal0, frag_in.TexCoord).rgb * 2.f - vec3(1.f))), 1.f) : vec4(frag_in.Normal, 1.f); // result normal
}

vec4 GetAlbedo()
{
	return (material.numOfDiffuses > 0) ? texture(material.diffuse0, frag_in.TexCoord) : vec4(1.f, 0.f, 0.f, 1.f); // red as error
}

vec4 GetSpecular()
{
	return (material.numOfSpeculars > 0) ? texture(material.specular0, frag_in.TexCoord) : vec4(0.f); // No specular
}
void main()
{
	GPosition = GetPosition();
	GNormal = GetNormal();
	
	GAlbedoSpec.rgb = GetAlbedo().rgb;
	GAlbedoSpec.a = GetSpecular().r;
}