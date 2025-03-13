#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

struct Material {
    // Base properties
    vec3 albedo;        // Replace diffuse
    float metallic;     // 0-1 value
    float roughness;    // 0-1 value
    float ao;           // Ambient occlusion

    // Maps
    sampler2D albedoMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
    sampler2D normalMap;

    // Map toggles
    int hasAlbedoMap;
    int hasMetallicMap;
    int hasRoughnessMap;
    int hasAoMap;
    int hasNormalMap;
};

uniform Material material;

// Simple lighting for testing
uniform vec3 lightPos = vec3(1.0, 1.0, 2.0);
uniform vec3 lightColor = vec3(1.0);
uniform vec3 viewPos = vec3(0.0, 0.0, 5.0);

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilteredMap;
uniform sampler2D brdfLUT;
uniform int useIBL = 0;

const float PI = 3.14159265359;

// Calculate normal distribution function (microfacet distribution)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Calculate geometry function (self-shadowing of microfacets)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Calculate Fresnel equation (material reflectance based on view angle)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 IBLDiffuse(vec3 N, vec3 albedo) {
    return texture(irradianceMap, N).rgb * albedo;
}

vec3 IBLSpecular(vec3 N, vec3 V, float roughness, vec3 F0) {
    float NdotV = max(dot(N, V), 0.0);

    // Calculate reflection vector
    vec3 R = reflect(-V, N);

    // Sample from prefiltered environment map based on roughness
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;

    // Get scale and bias from BRDF LUT
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;

    // Combine for final specular IBL contribution
    return prefilteredColor * (F0 * brdf.x + brdf.y);
}

void main() {
    // Get all material properties
    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;

    if (material.hasAlbedoMap == 1)
        albedo = texture(material.albedoMap, TexCoords).rgb;
    if (material.hasMetallicMap == 1)
        metallic = texture(material.metallicMap, TexCoords).r;
    if (material.hasRoughnessMap == 1)
        roughness = texture(material.roughnessMap, TexCoords).r;
    if (material.hasAoMap == 1)
        ao = texture(material.aoMap, TexCoords).r;

    // Calculate normal
    vec3 N = normalize(Normal);
    if (material.hasNormalMap == 1) {
        // Sample normal from normal map
        vec3 normalMap = texture(material.normalMap, TexCoords).rgb;
        normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] to [-1,1]

        // Transform normal from tangent space to world space
        N = normalize(TBN * normalMap);
    }

    vec3 V = normalize(viewPos - FragPos);

    // Calculate reflectance at normal incidence (F0)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Initialize final color
    vec3 Lo = vec3(0.0);

    // Calculate per-light radiance
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // Avoid division by zero
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    // Add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    // Calculate ambient term (either IBL or simple ambient)
    vec3 ambient;

    if (useIBL == 1) {
        // Ambient with IBL
        vec3 F = FresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 diffuse = IBLDiffuse(N, albedo);
        vec3 specular = IBLSpecular(N, V, roughness, F0);

        ambient = (kD * diffuse + specular) * ao;
    } else {
        // Fallback to simple ambient
        ambient = vec3(0.03) * albedo * ao;
    }

    // Combine ambient and direct lighting
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}