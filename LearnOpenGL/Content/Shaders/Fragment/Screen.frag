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

const float offset = 1.0 / 300.0;  

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset,  offset), // top-left
		vec2( 0.0f,    offset), // top-center
		vec2( offset,  offset), // top-right
		vec2(-offset,  0.0f),   // center-left
		vec2( 0.0f,    0.0f),   // center-center
		vec2( offset,  0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2( 0.0f,   -offset), // bottom-center
		vec2( offset, -offset)  // bottom-right    
	);
	
	float kernel[9] = float[](
		2, 2, 2,
		2, -15, 2,
		2, 2, 2
	);
	
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
		sampleTex[i] = vec3(texture(material.diffuse0, TexCoord.xy + offsets[i]));
		
	vec3 col = vec3(0.0);
	
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];
	
	FragColor = vec4(col, 1.0);
}