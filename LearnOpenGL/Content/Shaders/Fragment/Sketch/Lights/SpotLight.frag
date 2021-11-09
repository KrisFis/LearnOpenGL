#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position; // possible value: camera.Position
    vec3 direction; // possible value: camera.Front
    float cutOff; // possible value: cos(radians(12.5f))

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant; // possible value: 1.f
    float linear; // possible value: 0.09f
    float quadratic; // possible value: 0.032f
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    if(dot(lightDir, normalize(-light.direction)) < light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

        // diffuse
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.f);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

        // attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));

        FragColor = vec4(attenuation * (diffuse + specular) + ambient, 1.f);
    }
    else
    {
        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.f);
    }
}