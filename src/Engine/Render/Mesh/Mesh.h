#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <limits>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh() = default; // Added default constructor
    ~Mesh();

    void Initialize(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    void Draw() const;

    [[nodiscard]] unsigned int GetIndexCount() const { return indexCount; }
    
    // Add accessor methods for vertices and indices
    [[nodiscard]] const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    [[nodiscard]] const std::vector<unsigned int>& GetIndices() const { return m_Indices; }

    // Add bounds methods
    [[nodiscard]] glm::vec3 GetMinBounds() const;
    [[nodiscard]] glm::vec3 GetMaxBounds() const;
    void CalculateBounds();

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    unsigned int indexCount = 0;
    
    // Store the mesh data for ray intersection and other operations
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    
    // Bounds information
    glm::vec3 m_MinBounds = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_MaxBounds = glm::vec3(std::numeric_limits<float>::lowest());
    bool m_BoundsDirty = true;
};

#endif // MESH_H
