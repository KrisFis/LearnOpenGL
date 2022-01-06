#version 460 core

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;
layout (location = 2) out vec4 GAlbedoSpec;

in VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
} frag_in;

uniform struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
} material;

void main()
{
	GPosition = vec4(frag_in.FragPos, 1.f);
	GNormal = vec4(normalize(frag_in.Normal), 1.f);
	GAlbedoSpec.rgb = texture(material.diffuse0, frag_in.TexCoord).rgb;
	GAlbedoSpec.a = texture(material.specular0, frag_in.TexCoord).r;
}