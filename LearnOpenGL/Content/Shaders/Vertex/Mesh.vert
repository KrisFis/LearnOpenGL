#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
} vert_out;

layout (std140) uniform UMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

// Shadow
uniform mat4 lightSpaceMatrix;

void main()
{
	vert_out.FragPos = aPos;
	vert_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vert_out.TexCoord = aTexCoord;
	vert_out.FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.f);
	
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}