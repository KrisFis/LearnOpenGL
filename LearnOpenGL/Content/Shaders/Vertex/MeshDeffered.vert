#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
} vert_out;

layout (std140) uniform UMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	
	vert_out.FragPos = worldPos.xyz;
	vert_out.TexCoord = aTexCoord;
	vert_out.Normal = transpose(inverse(mat3(model))) * aNormal;

	gl_Position = projection * view * worldPos;
}