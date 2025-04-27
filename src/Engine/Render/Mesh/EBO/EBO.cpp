#include"EBO.h"

EBO::EBO(std::vector <GLuint>& indices)
{
    // Generate the VBO with only 1 object
    glGenBuffers(1, &ID);
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    // Introduce the vertices into the VBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::Bind()
{
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER - VBO'yu bağlayarak GL_ARRAY_BUFFER olduğunu belirtir
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind()
{
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
    // Delete the VBO
    glDeleteBuffers(1, &ID);
}