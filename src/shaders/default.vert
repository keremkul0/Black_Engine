#version 330 core

// Vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
uniform mat4 lightSpaceMatrix; // For shadow mapping, if needed
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//


// Output to fragment shader
out vec3 Normal;
out vec2 TexCoord;
// Outputs the current position for the Fragment Shader
out vec3 crntPos;

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
out vec4 fragPosLight;
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

void main()
{
// Calculates the current position of the vertex in world space
    crntPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoord = aTexCoord;


    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    fragPosLight = lightSpaceMatrix * vec4(crntPos, 1.0);
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//


    gl_Position = projection * view * model * vec4(aPos, 1.0);
}