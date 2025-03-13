#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent; // New tangent attribute

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN; // TBN matrix for normal mapping

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 N = normalize(Normal);

    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);

    // Calculate the bitangent
    vec3 B = cross(N, T);

    // Create TBN matrix
    TBN = mat3(T, B, N);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}