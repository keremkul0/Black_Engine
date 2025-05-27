#include"VAO.h"

VAO::VAO()
{
    // Generate the VAO with only 1 object
    glGenVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}
void VAO::Bind()
{
    // Bind the VAO
    glBindVertexArray(ID);
}
void VAO::Unbind()
{
    // Unbind the VAO
    glBindVertexArray(0);
}
void VAO::Delete()
{
    // Delete the VAO
    glDeleteVertexArrays(1, &ID);
}
