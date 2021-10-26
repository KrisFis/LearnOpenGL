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

uniform bool useOverrideColor;
uniform vec4 overrideColor;

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : texture(material.diffuse0, TexCoord);
	if(resultColor.a < 0.1f)
		discard;

	FragColor = vec4(vec3(1.f - resultColor.rgb), resultColor.a);
}