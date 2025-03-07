#pragma once
#include <vector>
#include <glm/glm.hpp>

// Küp transform verisi
struct Cube
{
    glm::vec3 position;
    glm::vec3 rotation;
};

class Scene
{
public:
    void AddCube(const glm::vec3& pos);
    void UpdateCubeRotations(float currentTime);
    const std::vector<Cube>& GetCubes() const { return cubes; }

    // Tek seferlik VAO-VBO oluşturma
    static void InitCubeGeometry();
    static void CleanUpCubeGeometry();

    // VAO ile küp çizecek fonksiyon
    void DrawAllCubes(unsigned int shaderProgram, int width, int height);

private:
    std::vector<Cube> cubes;

    // Statik üyeler (ortak küp geometrisi)
    static unsigned int s_VAO;
    static unsigned int s_VBO;
    static bool s_Initialized;
};
