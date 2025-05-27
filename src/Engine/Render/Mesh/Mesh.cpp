#include "Mesh.h"
#include <iostream>
#include <glad/glad.h>


Mesh::~Mesh()
{
    // Yok edici
    if (VAO.ID) VAO.Delete();
}

void Mesh::Initialize(const std::vector<Vertex>& vertices,const std::vector<unsigned int>& indices)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;


    VAO.Bind();

    // VBO
    VBO VBO(Mesh::vertices);

    // EBO
    EBO EBO(Mesh::indices);

    // Position (layout=0)
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);

    // Normal (layout=1)
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex),(void*)(3 * sizeof(float)));

    // TexCoords (layout=2)
    VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::Draw()
{
    VAO.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}
