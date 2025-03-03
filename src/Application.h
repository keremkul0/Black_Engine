#pragma once

#include <glm/glm.hpp>

// Global kamera matrisleri (silinebilir obje değil, sabit kamera gibi davranacak)
extern glm::mat4 gViewMatrix;
extern glm::mat4 gProjectionMatrix;

class Application
{
public:
    Application() = default;
    ~Application() = default;

    int Run();
};
