#include"VBO.h"

VBO::VBO(std::vector<Vertex>& vertices)
{
    // Generate the VBO with only 1 object
    glGenBuffers(1, &ID);
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    // Introduce the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VBO::Bind()
{
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER - VBO'yu bağlayarak GL_ARRAY_BUFFER olduğunu belirtir
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    // Delete the VBO
    glDeleteBuffers(1, &ID);
}