#version 460 core

struct Material
{
    sampler2D diffuse0;
};

out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;

void main()
{
	FragColor = texture(material.diffuse0, TexCoord);
}