#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out VERT_OUT {
	vec2 TexCoord;
} vert_out;

void main()
{
	vert_out.TexCoord = aTexCoord;
	
	gl_Position = vec4(aPos.x, aPos.y, 0.f, 1.f);
}