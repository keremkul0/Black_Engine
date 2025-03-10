#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <memory>
#include "Engine/Render/Mesh/Mesh.h"

namespace Primitives {
    // Küp oluşturur - boyut parametresi küpün kenar uzunluğunu belirler
    std::shared_ptr<Mesh> CreateCube(float size = 1.0f);

    // Küre oluşturur
    // radius: Kürenin yarıçapı
    // segments: Enlem ve boylam bölümlerinin sayısı (detay seviyesi)
    std::shared_ptr<Mesh> CreateSphere(float radius = 0.5f, int segments = 16);

    // Düzlem/plane oluşturur (XZ düzlemi üzerinde)
    // width: X eksenindeki genişlik
    // depth: Z eksenindeki derinlik
    // subdivisions: Kenar başına bölüm sayısı
    std::shared_ptr<Mesh> CreatePlane(float width = 1.0f, float depth = 1.0f, int subdivisions = 1);

    // Dörtgen/quad oluşturur (XY düzlemi üzerinde)
    // width: X eksenindeki genişlik
    // height: Y eksenindeki yükseklik
    std::shared_ptr<Mesh> CreateQuad(float width = 1.0f, float height = 1.0f);

    // Silindir oluşturur
    // radius: Silindirin taban yarıçapı
    // height: Silindirin yüksekliği
    // segments: Çevredeki segment sayısı (detay seviyesi)
    std::shared_ptr<Mesh> CreateCylinder(float radius = 0.5f, float height = 1.0f, int segments = 16);

    // Kapsül oluşturur (iki yarım küre ile birleştirilmiş silindir)
    // radius: Kapsülün yarıçapı
    // height: Silindirin yüksekliği (toplam boy = height + 2*radius)
    // segments: Çevredeki segment sayısı (detay seviyesi)
    std::shared_ptr<Mesh> CreateCapsule(float radius = 0.5f, float height = 1.0f, int segments = 16);
}


#endif // PRIMITIVES_H