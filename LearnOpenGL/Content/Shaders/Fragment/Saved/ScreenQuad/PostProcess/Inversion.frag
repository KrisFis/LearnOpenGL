#version 460 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoord)), 1.0);
}