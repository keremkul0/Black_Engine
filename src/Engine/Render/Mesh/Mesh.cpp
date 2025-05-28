#include "Mesh.h"
#include <iostream>
#include <glad/glad.h>


Mesh::~Mesh()
{
    // Yok edici
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

void Mesh::Initialize(const std::vector<Vertex>& vertices,
                      const std::vector<unsigned int>& indices)
{
    // Store a copy of the mesh data for collision detection, ray casting, etc.
    m_Vertices = vertices;
    m_Indices = indices;
    
    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);

    // Position (layout=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          static_cast<void *>(nullptr));

    // Normal (layout=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, normal)));

    // TexCoords (layout=2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);

    // Calculate bounds after initialization
    CalculateBounds();
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
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
