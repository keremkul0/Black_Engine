#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>

// Tek bir vertex verisi
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    // istersen renk veya diğer verileri de ekleyebilirsin
};

// Mesh sınıfı, VAO/VBO/EBO oluşturur ve Draw() metoduyla çizilir
class Mesh
{
public:
    Mesh() = default;
    ~Mesh();

    // Vertex ve index verilerini GPU'ya yükler
    void Initialize(const std::vector<Vertex>& vertices,
                    const std::vector<unsigned int>& indices);

    // Bu Mesh'i çiz
    void Draw() const;

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    unsigned int indexCount = 0;
};

#endif // MESH_H
