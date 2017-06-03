#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 5;
uniform Light lights[NR_LIGHTS];

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int inGame;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // Transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;
        // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        // Get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        // Calculate bias (based on depth map resolution and slope)
        vec3 normal = normalize(fs_in.Normal);
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
        // Check whether current frag pos is in shadow
        // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
        // PCF
        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;

        // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if(projCoords.z > 1.0)
            shadow = 0.0;

        return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.05 * lightColor;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = 0.4 * diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    if (inGame == 1) {
        lighting = vec3(0.0f);
    }
    // Cal fire
    for (int i = 0; i < NR_LIGHTS; ++i) {
        // Calculate distance between light source and current fragment
        float distance = length(lights[i].Position - fs_in.FragPos);
        if(distance < lights[i].Radius) {
            // Diffuse
            vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
            vec3 diffuse = max(dot(normal, lightDir), 0.0) * lights[i].Color;
            // Specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), 32.0) * lights[i].Color;
            // Attenuation
            float attenuation = 15 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            lighting += (diffuse + specular) * attenuation;
        }
    }

    FragColor = vec4(lighting * color, 1.0f);
}