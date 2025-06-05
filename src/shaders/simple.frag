#version 330 core

in vec3 Normal;
out vec4 FragColor;


// Texture coordinates
in vec2 TexCoord;


// Gets the Texture Unit from the main function
uniform sampler2D tex0;
uniform bool hasTexture;

void main()
{
    vec4 color;
    if (hasTexture) {
        vec4 texColor = texture(tex0, TexCoord);
        // If the texture is grayscale (R only), replicate R to G and B
        if (texColor.g == 0.0 && texColor.b == 0.0) {
            texColor.g = texColor.r;
            texColor.b = texColor.r;
        }
        color = texColor;
    } else {
        color = vec4(1.0, 1.0, 1.0, 1.0); // fallback white
    }
    FragColor = color;
}