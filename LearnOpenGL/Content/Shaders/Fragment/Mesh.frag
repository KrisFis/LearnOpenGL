#version 330 core

struct Material
{
    sampler2D diffuse0;
};

out vec4 FragColor;
in vec2 TexCoord;

uniform Material material;

uniform bool useOverrideColor;
uniform vec4 overrideColor;

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : texture(material.diffuse0, TexCoord);
	if(resultColor.a < 0.1f)
		discard;

	FragColor = resultColor;
}