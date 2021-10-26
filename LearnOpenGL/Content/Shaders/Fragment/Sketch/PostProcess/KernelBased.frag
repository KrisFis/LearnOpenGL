#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D screenTexture;

// A kernel (or convolution matrix) is a small matrix-like array of values
// * centered on the current pixel that multiplies surrounding pixel values by its kernel values and adds them all together to form a single value
// Known kernel effects:
// * [1, 1, 1, 1, −8, 1, 1, 1, 1] -> Edge-detection
// * [1, 2, 1, 2, 4, 2, 1, 2, 1] / 16 -> Blur
// * [2, 2, 2, 2, −15, 2, 2, 2, 2] -> Narcotic 

void main()
{
	const float offset = 1.0 / 300.0;  
	
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
		sampleTex[i] = vec3(texture(screenTexture, TexCoord.xy + offsets[i]));
		
	vec3 col = vec3(0.0);
	
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];
	
	FragColor = vec4(col, 1.0);
}