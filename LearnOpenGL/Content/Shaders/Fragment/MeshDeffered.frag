#version 460 core

layout (location = 0) out vec3 GPosition;
layout (location = 1) out vec3 GNormal;
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
} material;

void main()
{
	GPosition = frag_in.FragPos;
	GNormal = normalize(frag_in.Normal);
	GAlbedoSpec.rgb = texture(material.diffuse0, frag_in.TexCoord).rgb;
	GAlbedoSpec.a = texture(material.specular0, frag_in.TexCoord).r;
}  