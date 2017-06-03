#version 330 core
struct Material {
    sampler2D diffuse;
    float     shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;

void main() {
    Light light;
    light.position = vec3(0.0f, 9.0f, -5.5f);
    light.ambient = vec3(0.2f, 0.2f, 0.2f);
    light.diffuse = vec3(0.5f, 0.5f, 0.5f);
    light.specular = vec3(1.0f, 1.0f, 1.0f);
    // Ambient
    vec4 diffuseTex = texture(material.diffuse, TexCoords);
    if(diffuseTex.a < 0.1)
            discard;
    vec3 ambient = light.ambient * vec3(diffuseTex);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
//    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords));

    color = vec4(ambient + diffuse + specular, 1.0f);
}