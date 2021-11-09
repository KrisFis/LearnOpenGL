#version 460 core

struct Material
{
	samplerCube texture;
};

out vec4 FragColor;

in vec3 TexCoord;

uniform Material material;

void main()
{
	FragColor = texture(material.texture, TexCoord);
}