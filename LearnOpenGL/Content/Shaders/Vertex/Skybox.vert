#version 460 core

layout (location = 0) in vec3 aPos;

out VERT_OUT {
	vec3 TexCoord;
} vert_out;

layout (std140) uniform UMatrices
{
	mat4 projection; 
	mat4 view;
};

void main()
{
	vert_out.TexCoord = aPos;
	gl_Position = (projection * mat4(mat3(view)) * vec4(aPos, 1.0)).xyww;
}