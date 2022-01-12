#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VERT_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoord;
} vert_out;

layout (std140) uniform UMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat4 inverseModel;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	
	vert_out.FragPos = worldPos.xyz;
	vert_out.TexCoord = aTexCoord;
	
	vec3 T = normalize(mat3(model) * aTangent);
	vec3 B = normalize(mat3(model) * aBitangent);
	vec3 N = normalize(mat3(model) * aNormal);
	vert_out.TBN = mat3(T, B, N);
	
	gl_Position = projection * view * worldPos;
}