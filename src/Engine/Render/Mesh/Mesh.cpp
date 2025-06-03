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
<<<<<<< HEAD
    // Store a copy of the mesh data for collision detection, ray casting, etc.
    m_Vertices = vertices;
    m_Indices = indices;
    
    indexCount = static_cast<unsigned int>(indices.size());
=======
    Mesh::vertices = vertices;
    Mesh::indices = indices;
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2


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

    // Calculate bounds after initialization
    CalculateBounds();
}

void Mesh::Draw()
{
    VAO.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

glm::vec3 Mesh::GetMinBounds() const
{
    if (m_BoundsDirty) {
        const_cast<Mesh*>(this)->CalculateBounds();
    }
    return m_MinBounds;
}

glm::vec3 Mesh::GetMaxBounds() const
{
    if (m_BoundsDirty) {
        const_cast<Mesh*>(this)->CalculateBounds();
    }
    return m_MaxBounds;
}

void Mesh::CalculateBounds()
{
    if (m_Vertices.empty()) {
        m_MinBounds = glm::vec3(0.0f);
        m_MaxBounds = glm::vec3(0.0f);
        m_BoundsDirty = false;
        return;
    }
    
    // Reset bounds
    m_MinBounds = glm::vec3(std::numeric_limits<float>::max());
    m_MaxBounds = glm::vec3(std::numeric_limits<float>::lowest());
    
    // Find min and max for each axis
    for (const auto& vertex : m_Vertices) {
        m_MinBounds.x = std::min(m_MinBounds.x, vertex.position.x);
        m_MinBounds.y = std::min(m_MinBounds.y, vertex.position.y);
        m_MinBounds.z = std::min(m_MinBounds.z, vertex.position.z);
        
        m_MaxBounds.x = std::max(m_MaxBounds.x, vertex.position.x);
        m_MaxBounds.y = std::max(m_MaxBounds.y, vertex.position.y);
        m_MaxBounds.z = std::max(m_MaxBounds.z, vertex.position.z);
    }
    
    m_BoundsDirty = false;
}
