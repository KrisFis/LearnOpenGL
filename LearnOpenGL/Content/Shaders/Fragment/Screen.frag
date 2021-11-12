#version 460 core

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
} u_postprocess;

out vec4 FragColor;

vec4 ApplyGamaCorrection(vec4 InFragColor)
{
	vec3 correctedColor = pow(InFragColor.rgb, vec3(1.f / u_postprocess.gamma));
	return vec4(correctedColor, InFragColor.a);
}

void main()
{
	vec4 resultColor = texture(material.diffuse0, frag_in.TexCoord);

	// Post-processing
	{
		resultColor = ApplyGamaCorrection(resultColor);
	}
	
	FragColor = resultColor;
}