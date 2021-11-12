#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out VERT_OUT {
	vec2 TexCoord;
} vert_out;


void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
	vert_out.TexCoord = aTexCoord;
}