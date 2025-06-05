#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the texture coordinates from the Vertex Shader
in vec2 TexCoord;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;
uniform bool hasTexture;
// Added uniform for camera position
uniform vec3 camPos;

vec4 directLight()
{
    // ambient lighting
    float ambient = 0.20f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(-1.0f, 1.0f, 1.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);


    // specular lighting
    	float specularLight = 0.50f;
    	vec3 viewDirection = normalize(camPos - crntPos);
    	vec3 reflectionDirection = reflect(-lightDirection, Normal);
    	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    	float specular = specAmount * specularLight;

    // local lightColor variable defined as vec4
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 texColor;
    if (hasTexture) {
        texColor = texture(tex0, TexCoord);
        // If the texture is grayscale (R only), replicate R to G and B
        if (texColor.g == 0.0 && texColor.b == 0.0) {
            texColor.g = texColor.r;
            texColor.b = texColor.r;
        }
    } else {
        texColor = vec4(1.0, 1.0, 1.0, 1.0); // fallback white
    }

    return (texColor * (diffuse + ambient) + specular) * lightColor;
}

void main()
{
    FragColor = directLight();

    // outputs final color
}

