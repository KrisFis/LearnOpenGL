#version 330 core

#define near 0.1f
#define far 100.f

float LinearizeDepth(float depth)
{
	float z = depth * 2.f - 1.f;
	return (2.f * near * far) / (far + near - z * (far - near));
}

void main()
{
	FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z)), 1.f) / far;
}