#version 330 core

in vec3 Normal;
out vec4 FragColor;


// Texture coordinates
in vec2 TexCoord;


// Gets the Texture Unit from the main function
uniform sampler2D tex0;

void main()
{
    // Determine color based on normal direction
    vec3 color = vec3(0.1, 0.1, 0.5); // Default color

    /*// Front face (z positive) - red
    if(Normal.z > 0.5)
        color = vec3(1.0, 0.0, 0.0);
    // Back face (z negative) - green
    else if(Normal.z < -0.5)
        color = vec3(0.0, 1.0, 0.0);
    // Right face (x positive) - blue
    else if(Normal.x > 0.5)
        color = vec3(0.0, 0.0, 1.0);
    // Left face (x negative) - yellow
    else if(Normal.x < -0.5)
        color = vec3(1.0, 1.0, 0.0);
    // Top face (y positive) - cyan
    else if(Normal.y > 0.5)
        color = vec3(0.0, 1.0, 1.0);
    // Bottom face (y negative) - magenta
    else if(Normal.y < -0.5)
        color = vec3(1.0, 0.0, 1.0);
    // Fallback color
    else
        color = vec3(0.5, 0.5, 0.5);*/

    //FragColor = vec4(color, 1.0);
    FragColor = texture(tex0, TexCoord);
}