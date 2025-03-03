#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <memory>
#include "Mesh.h"

namespace Primitives {
    // Basit bir küp oluşturan fonksiyon
    std::shared_ptr<Mesh> CreateCube();

    // İleride CreateSphere(), CreateCylinder() gibi fonksiyonlar da ekleyebilirsin
}

#endif // PRIMITIVES_H
