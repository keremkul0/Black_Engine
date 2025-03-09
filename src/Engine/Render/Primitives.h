#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <memory>
#include "Mesh.h"

namespace Primitives {
    // Remove the static keyword
    std::shared_ptr<Mesh> CreateCube();
    // Other primitive functions can be added later
}

#endif // PRIMITIVES_H