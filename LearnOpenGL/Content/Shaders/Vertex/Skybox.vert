#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

layout (std140) uniform UMatrices
{
	mat4 projection; 
	mat4 view;
};

void main()
{
	TexCoord = aPos;
	vec4 resultPos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
	gl_Position = resultPos.xyww;
}