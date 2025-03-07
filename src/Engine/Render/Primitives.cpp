#include "Primitives.h"

namespace Primitives {

std::shared_ptr<Mesh> CreateCube()
{
    // 24 vertex + 36 index
    std::vector<Vertex> vertices = {
        // Ön yüz
        {{-0.5f, -0.5f,  0.5f}, {0.f, 0.f, 1.f},  {0.f, 0.f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.f, 0.f, 1.f},  {1.f, 0.f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.f, 0.f, 1.f},  {1.f, 1.f}},
        {{-0.5f,  0.5f,  0.5f}, {0.f, 0.f, 1.f},  {0.f, 1.f}},

        // Arka yüz
        {{-0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {0.f, 1.f}},
        {{-0.5f,  0.5f, -0.5f}, {0.f, 0.f, -1.f}, {1.f, 1.f}},

        // Sol yüz
        {{-0.5f, -0.5f, -0.5f}, {-1.f,0.f,0.f},   {0.f,0.f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.f,0.f,0.f},   {1.f,0.f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.f,0.f,0.f},   {1.f,1.f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.f,0.f,0.f},   {0.f,1.f}},

        // Sağ yüz
        {{ 0.5f, -0.5f, -0.5f}, {1.f,0.f,0.f},    {1.f,0.f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.f,0.f,0.f},    {0.f,0.f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.f,0.f,0.f},    {0.f,1.f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.f,0.f,0.f},    {1.f,1.f}},

        // Üst yüz
        {{-0.5f,  0.5f,  0.5f}, {0.f,1.f,0.f},    {0.f,0.f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.f,1.f,0.f},    {1.f,0.f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.f,1.f,0.f},    {1.f,1.f}},
        {{-0.5f,  0.5f, -0.5f}, {0.f,1.f,0.f},    {0.f,1.f}},

        // Alt yüz
        {{-0.5f, -0.5f,  0.5f}, {0.f,-1.f,0.f},   {1.f,1.f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.f,-1.f,0.f},   {0.f,1.f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.f,-1.f,0.f},   {0.f,0.f}},
        {{-0.5f, -0.5f, -0.5f}, {0.f,-1.f,0.f},   {1.f,0.f}},
    };

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    auto mesh = std::make_shared<Mesh>();
    mesh->Initialize(vertices, indices);
    return mesh;
}

} // namespace Primitives
