#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;

    int hasDiffuseMap;
    int hasSpecularMap;
    int hasNormalMap;

    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
};

uniform Material material;

// Simple lighting for testing
uniform vec3 lightPos = vec3(1.0, 1.0, 2.0);
uniform vec3 lightColor = vec3(1.0);
uniform vec3 viewPos = vec3(0.0, 0.0, 5.0);

void main() {
    // Normalize normal
    vec3 norm = normalize(Normal);

    // Sample textures if available
    vec3 diffuseColor = material.diffuse;
    if (material.hasDiffuseMap == 1) {
        diffuseColor = texture(material.diffuseMap, TexCoords).rgb;
    }

    vec3 specularColor = material.specular;
    if (material.hasSpecularMap == 1) {
        specularColor = texture(material.specularMap, TexCoords).rgb;
    }

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * diffuseColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffuseColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * lightColor * specularColor;

    // Final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}