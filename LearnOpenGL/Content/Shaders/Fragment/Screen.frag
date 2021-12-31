#version 460 core

out vec4 FragColor;

in VERT_OUT {
	vec2 TexCoord;
} frag_in;

uniform struct Material
{
	sampler2D diffuse0;
} material;

layout (std140) uniform UPostProcess
{
	float gamma;
	float exposure;
} u_postprocess;

uniform sampler2D bloomBlur;

vec3 ApplyBloom(vec3 InFragColor)
{
	return InFragColor + texture(bloomBlur, frag_in.TexCoord).rgb;
}

vec3 ApplyToneMapping(vec3 InFragColor)
{
	return vec3(1.f) - exp(-InFragColor * u_postprocess.exposure);
}

vec3 ApplyGamaCorrection(vec3 InFragColor)
{
	return pow(InFragColor.rgb, vec3(1.f / u_postprocess.gamma));
}

void main()
{
	vec3 resultColor = texture(material.diffuse0, frag_in.TexCoord).rgb;

	// Post-processing
	{
		resultColor = ApplyBloom(resultColor);
		resultColor = ApplyToneMapping(resultColor);
		resultColor = ApplyGamaCorrection(resultColor);
	}

	// We dont care about alpha calculation
	FragColor = vec4(resultColor, 1.f);
}