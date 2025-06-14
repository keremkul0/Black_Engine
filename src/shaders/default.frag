#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the texture coordinates from the Vertex Shader
in vec2 TexCoord;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
in vec4 fragPosLight;
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//


// Gets the Texture Unit from the main function
uniform sampler2D tex0;
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
uniform sampler2D shadowMap;
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
uniform bool hasTexture;
// Added uniform for camera position
uniform vec3 camPos;

vec4 directLight()
{
    float ambient = 0.20f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(-1.0f, 1.0f, 1.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, Normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 texColor;
    if (hasTexture) {
        texColor = texture(tex0, TexCoord);
        if (texColor.g == 0.0 && texColor.b == 0.0) {
            texColor.g = texColor.r;
            texColor.b = texColor.r;
        }
    } else {
        texColor = vec4(1.0, 1.0, 1.0, 1.0);
    }

    // --- Shadow calculation ---
    float shadow = 1.0f;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if(lightCoords.z <= 1.0f)
    {
        lightCoords = (lightCoords + 1.0f) / 2.0f;
        float closestDepth = texture(shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        // shadow = 0.0 gölgede, 1.0 aydınlıkta
        shadow = currentDepth - 0.005 > closestDepth ? 0.0 : 1.0;
    }

    // Sadece diffuse ve specular gölgelenir, ambient her zaman eklenir
    float lighting = ambient + shadow * (diffuse + specular);

    return texColor * lighting * lightColor;
}

void main()
{
    FragColor = directLight();

    // outputs final color
}
