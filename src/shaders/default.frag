#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the texture coordinates from the Vertex Shader
in vec2 TexCoord;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;

vec4 directLight()
{
    // ambient lighting
    float ambient = 0.20f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(-1.0f, 1.0f, -1.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // local lightColor variable defined as vec4
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    return (texture(tex0, TexCoord) * (diffuse + ambient)) * lightColor;
}

void main()
{
    FragColor = directLight();

    // outputs final color
}
