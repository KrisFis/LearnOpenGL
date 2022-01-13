#version 460 core

out float FragColor;

in VERT_OUT {
	vec2 TexCoord;
} frag_in;

layout (std140) uniform UMatrices
{
	mat4 projection;
	mat4 view;
};

uniform struct GBuffer
{
	sampler2D position;
	sampler2D normal;
} gBuffer;

uniform struct Noise
{
	vec2 scale;
	sampler2D map;
} noise;

#define KERNEL_SIZE 64
#define RADIUS 0.5f
#define BIAS 0.025f

uniform vec3 samples[KERNEL_SIZE];

void main()
{
	vec3 fragPos	= texture(gBuffer.position, frag_in.TexCoord).xyz;
	vec3 normal		= texture(gBuffer.normal, frag_in.TexCoord).rgb;
	vec3 randomVec	= texture(noise.map, frag_in.TexCoord * noise.scale).xyz;

	vec3 tangent	= normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent	= cross(normal, tangent);
	mat3 TBN		= mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < KERNEL_SIZE; ++i)
	{
		// get sample position
		vec3 samplePos = TBN * samples[i]; // from tangent to view-space
		samplePos = fragPos + samplePos * RADIUS; 
		
		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
		
		// get sample depth
		float sampleDepth = texture(gBuffer.position, offset.xy).z; // get depth value of kernel sample
		
		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;           
	}

	occlusion = 1.0 - (occlusion / KERNEL_SIZE);

	FragColor = occlusion;
}