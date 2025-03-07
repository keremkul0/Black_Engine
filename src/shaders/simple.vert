#version 330 core

// OpenGL'de vertex konumunu layout(location = 0) ile alıyoruz
layout (location = 0) in vec3 aPos;
// Eğer normal (layout=1) ve texCoord (layout=2) da kullanıyorsanız,
// bunları eklemeniz gerekir.

// MVP (Model-View-Projection) uniform'ları
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // En basit yaklaşım: pozisyonu MVP matrisleriyle çarpıyoruz
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
