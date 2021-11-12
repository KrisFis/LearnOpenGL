#version 460 core

in VERT_OUT {
	vec3 TexCoord;
} frag_in;

uniform struct Material
{
	samplerCube texture;
} material;

out vec4 FragColor;

void main()
{
	FragColor = texture(material.texture, frag_in.TexCoord);
}