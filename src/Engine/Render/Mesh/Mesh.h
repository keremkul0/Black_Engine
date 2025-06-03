#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
<<<<<<< HEAD
#include <limits>
=======
#include "VBO/VBO.h"
#include "EBO/EBO.h"
#include "VAO/VAO.h"

>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2


class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    VAO VAO;;


    /////////////////////////
    Mesh() = default; // Added default constructor
    ~Mesh();

    void Initialize(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    void Draw();

<<<<<<< HEAD
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
=======

private:

    /////////////////////////
    //GLuint VAO = 0;
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2
};

#endif // MESH_H
