#version 330 core

struct Material
{
    sampler2D diffuse0;
    sampler2D diffuse1;
    sampler2D diffuse2;
    
    sampler2D specular0;
    sampler2D specular1;
    sampler2D specular2;
    
    sampler2D emission0;
    sampler2D emission1;
    sampler2D emission2;
    
    float shininess;
};

out vec4 FragColor;
in vec2 TexCoord;

uniform Material material;

void main()
{
	// for now use only 0 index
	FragColor = texture(useMaterial.diffuse0, TexCoord);
}