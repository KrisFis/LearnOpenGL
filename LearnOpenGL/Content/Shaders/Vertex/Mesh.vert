#version 330 core

layout (position = 0) vec3 aPos;
layout (position = 0) vec3 aNormal;
layout (position = 0) vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}