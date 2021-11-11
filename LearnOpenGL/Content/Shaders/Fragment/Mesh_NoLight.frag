#version 460 core

in VERT_OUT {

	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	
} frag_in;

out vec4 FragColor;

uniform bool useOverrideColor;
uniform vec4 overrideColor;

void main()
{
	// for now use only 0 index
	vec4 resultColor = (useOverrideColor) ? overrideColor : vec4(0.9f,0.9f,0.9f, 1.f);
	if(resultColor.a < 0.1f)
		discard;

	FragColor = resultColor;
}