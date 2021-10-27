#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

// Reflective indexes
const float AirIdx = 1.f;
const float WaterIdx = 1.33f;
const float IceIdx = 1.309f;
const float GlassIdx = 1.52f;
const float DiamondIdx = 2.42f;

void main()
{
    float ratio = 1.00 / GlassIdx;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}