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

#define near 0.1f
#define far 100.f

float LinearizeDepth(float depth)
{
	float z = depth * 2.f - 1.f;
	return (2.f * near * far) / (far + near - z * (far - near));
}

void main()
{
	FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z)), 1.f) / far;

	// for now use only 0 index
	//FragColor = texture(material.diffuse0, TexCoord);
}