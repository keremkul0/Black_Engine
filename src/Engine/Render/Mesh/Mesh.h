#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Core/AssetDatabase/Asset.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh : public Asset {
    // Inherit from Asset
public:
    Mesh() = default;

    ~Mesh() override; // Mark as override

    void Initialize(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    void Draw() const;

    [[nodiscard]] unsigned int GetIndexCount() const { return indexCount; }

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    unsigned int indexCount = 0;
};

#endif // MESH_H
