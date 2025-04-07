#include "Primitives.h"
#include "Engine/Render/Mesh/Mesh.h"

namespace Primitives {
    std::shared_ptr<Mesh> CreateCube(const float size) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float halfSize = size * 0.5f;

        // Define all 8 corners of the cube
        const glm::vec3 corners[8] = {
            {-halfSize, -halfSize, -halfSize}, // 0: left bottom back
            {halfSize, -halfSize, -halfSize}, // 1: right bottom back
            {halfSize, -halfSize, halfSize}, // 2: right bottom front
            {-halfSize, -halfSize, halfSize}, // 3: left bottom front
            {-halfSize, halfSize, -halfSize}, // 4: left top back
            {halfSize, halfSize, -halfSize}, // 5: right top back
            {halfSize, halfSize, halfSize}, // 6: right top front
            {-halfSize, halfSize, halfSize} // 7: left top front
        };

        // Define the 6 face normals
        const glm::vec3 normals[6] = {
            {0.0f, 0.0f, 1.0f}, // Front face  (+Z)
            {0.0f, 0.0f, -1.0f}, // Back face   (-Z)
            {-1.0f, 0.0f, 0.0f}, // Left face   (-X)
            {1.0f, 0.0f, 0.0f}, // Right face  (+X)
            {0.0f, 1.0f, 0.0f}, // Top face    (+Y)
            {0.0f, -1.0f, 0.0f} // Bottom face (-Y)
        };

        // Define the face corner indices in counter-clockwise order
        const int faceCorners[6][4] = {
            {3, 2, 6, 7}, // Front face (+Z)
            {1, 0, 4, 5}, // Back face (-Z)
            {0, 3, 7, 4}, // Left face (-X)
            {2, 1, 5, 6}, // Right face (+X)
            {7, 6, 5, 4}, // Top face (+Y)
            {0, 1, 2, 3} // Bottom face (-Y)
        };

        // Generate vertices and indices for all faces
        for (int faceIdx = 0; faceIdx < 6; ++faceIdx) {
            const glm::vec3 &normal = normals[faceIdx];

            // UV coordinates for the quad corners
            constexpr glm::vec2 uvs[4] = {
                {0.0f, 0.0f}, // bottom-left
                {1.0f, 0.0f}, // bottom-right
                {1.0f, 1.0f}, // top-right
                {0.0f, 1.0f} // top-left
            };

            // Add four vertices for this face
            for (int i = 0; i < 4; ++i) {
                vertices.push_back({
                    corners[faceCorners[faceIdx][i]], // position
                    normal, // normal
                    uvs[i] // uv
                });
            }

            // Add indices for two triangles (counter-clockwise order)
            const unsigned int baseIdx = faceIdx * 4;
            // First triangle
            indices.push_back(baseIdx);
            indices.push_back(baseIdx + 1);
            indices.push_back(baseIdx + 2);

            // Second triangle
            indices.push_back(baseIdx);
            indices.push_back(baseIdx + 2);
            indices.push_back(baseIdx + 3);
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }

    std::shared_ptr<Mesh> CreateSphere(const float radius, const int segments) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Küreyi enlem ve boylam çizgileri ile oluştur
        for (int y = 0; y <= segments; ++y) {
            // phi: dikey açı (0 = üst, PI = alt)
            const float phi = M_PI * static_cast<float>(y) / static_cast<float>(segments);
            const float sinPhi = std::sin(phi);
            const float cosPhi = std::cos(phi);

            for (int x = 0; x <= segments; ++x) {
                // theta: yatay açı (0-2PI arası)
                const float theta = 2.0f * M_PI * static_cast<float>(x) / static_cast<float>(segments);
                const float sinTheta = std::sin(theta);
                const float cosTheta = std::cos(theta);

                // Küresel koordinatları kartezyen koordinatlara dönüştür
                float posX = radius * sinPhi * cosTheta;
                float posY = radius * cosPhi;
                float posZ = radius * sinPhi * sinTheta;

                // Normal vektörü, pozisyon vektörünün normalize edilmiş hali
                const glm::vec3 normal = glm::normalize(glm::vec3(posX, posY, posZ));

                // UV koordinatları
                float u = static_cast<float>(x) / static_cast<float>(segments);
                float v = static_cast<float>(y) / static_cast<float>(segments);

                vertices.push_back({{posX, posY, posZ}, normal, {u, v}});
            }
        }

        // Indeksleri hesapla - her grid karesini iki üçgene böl
        for (int y = 0; y < segments; ++y) {
            for (int x = 0; x < segments; ++x) {
                // Her grid karesinde 4 nokta var
                unsigned int topLeft = y * (segments + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * (segments + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;

                // İlk üçgen (saat yönünün tersine)
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // İkinci üçgen (saat yönünün tersine)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }

    std::shared_ptr<Mesh> CreatePlane(const float width, const float depth, const int subdivisions) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Düzlemde kaç nokta olacağını hesapla
        const int gridX = subdivisions + 1;
        const int gridZ = subdivisions + 1;

        // Grid için nokta aralığı
        const float stepX = width / static_cast<float>(subdivisions);
        const float stepZ = depth / static_cast<float>(subdivisions);

        // Düzlemin sol-alt köşesinin koordinatı
        const float startX = -width * 0.5f;
        const float startZ = -depth * 0.5f;

        // Tüm grid noktaları için vertex oluştur
        for (int z = 0; z < gridZ; ++z) {
            for (int x = 0; x < gridX; ++x) {
                // Pozisyon hesapla
                float posX = startX + stepX * static_cast<float>(x);
                float posZ = startZ + stepZ * static_cast<float>(z);

                // UV koordinatları (0,0 sol alt köşe, 1,1 sağ üst köşe)
                float u = static_cast<float>(x) / static_cast<float>(subdivisions);
                float v = static_cast<float>(z) / static_cast<float>(subdivisions);

                // Düzlem Y ekseninde olduğu için normal vektörü yukarı yönde (0,1,0)
                constexpr glm::vec3 normal(0.0f, 1.0f, 0.0f);

                vertices.push_back({{posX, 0.0f, posZ}, normal, {u, v}});
            }
        }

        // İndeksleri oluştur (iki üçgen ile bir kare)
        for (int z = 0; z < subdivisions; ++z) {
            for (int x = 0; x < subdivisions; ++x) {
                const unsigned int topLeft = z * gridX + x;
                const unsigned int topRight = topLeft + 1;
                const unsigned int bottomLeft = (z + 1) * gridX + x;
                const unsigned int bottomRight = bottomLeft + 1;

                // İlk üçgen (sol üst, sol alt, sağ üst)
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // İkinci üçgen (sağ üst, sol alt, sağ alt)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }

    std::shared_ptr<Mesh> CreateQuad(const float width, const float height) {
        // Quad boyutlarını hesapla
        const float halfWidth = width * 0.5f;
        const float halfHeight = height * 0.5f;

        // Yüz Z+ yönüne bakacak
        const std::vector<Vertex> vertices = {
            // Sol alt
            {{-halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            // Sağ alt
            {{halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            // Sağ üst
            {{halfWidth, halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            // Sol üst
            {{-halfWidth, halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
        };

        // İki üçgen ile quad oluşturma (saat yönünün tersine indeks sırası)
        const std::vector<unsigned int> indices = {
            0, 1, 2, // İlk üçgen: sol alt, sağ alt, sağ üst
            0, 2, 3 // İkinci üçgen: sol alt, sağ üst, sol üst
        };

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }

    std::shared_ptr<Mesh> CreateCylinder(const float radius, const float height, const int segments) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Yükseklik değerleri
        const float halfHeight = height * 0.5f;

        // Merkez noktalar (üst ve alt kapaklar için)

        // Üst merkez
        vertices.push_back({{0.0f, halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});
        // Alt merkez
        vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}});

        // Çember etrafında noktalar oluştur
        for (int i = 0; i <= segments; ++i) {
            const float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(segments);
            const float x = radius * std::cos(angle);
            const float z = radius * std::sin(angle);
            const float u = static_cast<float>(i) / static_cast<float>(segments);

            // Silindir yan yüzey noktaları
            const glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));

            // Üst kenar
            vertices.push_back({{x, halfHeight, z}, sideNormal, {u, 1.0f}});
            // Alt kenar
            vertices.push_back({{x, -halfHeight, z}, sideNormal, {u, 0.0f}});

            // Üst kapak dış kenar
            vertices.push_back({
                {x, halfHeight, z}, {0.0f, 1.0f, 0.0f}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}
            });

            // Alt kapak dış kenar
            vertices.push_back({
                {x, -halfHeight, z}, {0.0f, -1.0f, 0.0f}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}
            });
        }

        // Yan yüzey üçgenleri
        for (int i = 0; i < segments; ++i) {
            const unsigned int i0 = 2 + i * 4; // Üst kenar mevcut
            const unsigned int i1 = i0 + 1; // Alt kenar mevcut
            const unsigned int i2 = i0 + 4; // Üst kenar sonraki
            const unsigned int i3 = i0 + 5; // Alt kenar sonraki

            // İlk üçgen
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            // İkinci üçgen
            indices.push_back(i2);
            indices.push_back(i1);
            indices.push_back(i3);
        }

        // Üst kapak üçgenleri
        for (int i = 0; i < segments; ++i) {
            constexpr unsigned int topCenterIndex = 0;
            const unsigned int i0 = 2 + i * 4 + 2; // Üst kapak dış kenar mevcut
            const unsigned int i1 = i0 + 4; // Üst kapak dış kenar sonraki

            indices.push_back(topCenterIndex);
            indices.push_back(i0);
            indices.push_back(i1);
        }

        // Alt kapak üçgenleri
        for (int i = 0; i < segments; ++i) {
            constexpr unsigned int bottomCenterIndex = 1;
            const unsigned int i0 = 2 + i * 4 + 3; // Alt kapak dış kenar mevcut
            const unsigned int i1 = i0 + 4; // Alt kapak dış kenar sonraki

            indices.push_back(bottomCenterIndex);
            indices.push_back(i1);
            indices.push_back(i0);
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }

    std::shared_ptr<Mesh> CreateCapsule(const float radius, const float height, const int segments) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        const float halfHeight = height * 0.5f;
        const int rings = segments / 2;

        // 1. SİLİNDİR KISMI (ORTA)
        for (int i = 0; i <= segments; ++i) {
            const float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(segments);
            const float x = radius * std::cos(angle);
            const float z = radius * std::sin(angle);
            const float u = static_cast<float>(i) / static_cast<float>(segments);

            const glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            // Üst kenar
            vertices.push_back({{x, halfHeight, z}, normal, {u, 0.5f - 0.25f}});
            // Alt kenar
            vertices.push_back({{x, -halfHeight, z}, normal, {u, 0.5f + 0.25f}});
        }

        // 2. ÜST YARI KÜRE
        for (int y = 0; y <= rings; ++y) {
            const float phi = M_PI * 0.5f * (1.0f - static_cast<float>(y) / static_cast<float>(rings));
            const float sinPhi = std::sin(phi);
            const float cosPhi = std::cos(phi);

            for (int x = 0; x <= segments; ++x) {
                const float theta = 2.0f * M_PI * static_cast<float>(x) / static_cast<float>(segments);
                const float sinTheta = std::sin(theta);
                const float cosTheta = std::cos(theta);

                const float posX = radius * sinPhi * cosTheta;
                const float posY = halfHeight + radius * cosPhi;
                const float posZ = radius * sinPhi * sinTheta;

                const glm::vec3 normal = glm::normalize(glm::vec3(posX, posY - halfHeight, posZ));

                const float u = static_cast<float>(x) / static_cast<float>(segments);
                const float v = (0.5f - 0.25f) * (1.0f - static_cast<float>(y) / static_cast<float>(rings));

                vertices.push_back({{posX, posY, posZ}, normal, {u, v}});
            }
        }

        // Üst yarımkürenin tepe noktası
        vertices.push_back({{0.0f, halfHeight + radius, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.0f}});

        // 3. ALT YARI KÜRE
        for (int y = 0; y <= rings; ++y) {
            const float phi = M_PI * 0.5f + M_PI * 0.5f * static_cast<float>(y) / static_cast<float>(rings);
            const float sinPhi = std::sin(phi);
            const float cosPhi = std::cos(phi);

            for (int x = 0; x <= segments; ++x) {
                const float theta = 2.0f * M_PI * static_cast<float>(x) / static_cast<float>(segments);
                const float sinTheta = std::sin(theta);
                const float cosTheta = std::cos(theta);

                const float posX = radius * sinPhi * cosTheta;
                const float posY = -halfHeight + radius * cosPhi;
                const float posZ = radius * sinPhi * sinTheta;

                const glm::vec3 normal = glm::normalize(glm::vec3(posX, posY + halfHeight, posZ));

                const float u = static_cast<float>(x) / static_cast<float>(segments);
                const float v = 0.5f + 0.25f + 0.25f * static_cast<float>(y) / static_cast<float>(rings);

                vertices.push_back({{posX, posY, posZ}, normal, {u, v}});
            }
        }

        // Alt yarımkürenin en alt noktası
        vertices.push_back({{0.0f, -halfHeight - radius, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}});

        // İNDEKSLER
        // 1. Silindir kısmı indeksleri
        for (int i = 0; i < segments; ++i) {
            // Note: we use `segments` here, not `segments + 1` to handle wrapping
            const unsigned int i0 = i * 2;
            const unsigned int i1 = i * 2 + 1;
            const unsigned int i2 = ((i + 1) % segments) * 2;
            const unsigned int i3 = ((i + 1) % segments) * 2 + 1;

            // İlk üçgen (saat yönünün tersine)
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            // İkinci üçgen (saat yönünün tersine)
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }

        // 2. Üst yarıküre indeksleri
        const int upperHemisphereStartIdx = 2 * (segments + 1);

        // Special connection between cylinder and upper hemisphere first ring
        for (int i = 0; i < segments; ++i) {
            const unsigned int cylinderTop = i * 2;
            const unsigned int cylinderTopNext = ((i + 1) % segments) * 2;
            const unsigned int sphereBottom = upperHemisphereStartIdx + i;
            const unsigned int sphereBottomNext = upperHemisphereStartIdx + (i + 1) % (segments + 1);

            indices.push_back(cylinderTop);
            indices.push_back(cylinderTopNext);
            indices.push_back(sphereBottom);

            indices.push_back(sphereBottom);
            indices.push_back(cylinderTopNext);
            indices.push_back(sphereBottomNext);
        }

        // Rest of upper hemisphere
        for (int y = 0; y < rings - 1; ++y) {
            const int ringStartIdx = upperHemisphereStartIdx + y * (segments + 1);
            const int nextRingStartIdx = upperHemisphereStartIdx + (y + 1) * (segments + 1);

            for (int i = 0; i < segments; ++i) {
                const unsigned int currentVert = ringStartIdx + i;
                const unsigned int nextVert = ringStartIdx + (i + 1) % (segments + 1);
                const unsigned int currentVertNext = nextRingStartIdx + i;
                const unsigned int nextVertNext = nextRingStartIdx + (i + 1) % (segments + 1);

                // First triangle
                indices.push_back(currentVert);
                indices.push_back(nextVert);
                indices.push_back(currentVertNext);

                // Second triangle
                indices.push_back(currentVertNext);
                indices.push_back(nextVert);
                indices.push_back(nextVertNext);
            }
        }

        // Upper hemisphere top cap
        const int lastUpperRingStartIdx = upperHemisphereStartIdx + (rings - 1) * (segments + 1);
        const unsigned int topPoleIdx = upperHemisphereStartIdx + (rings + 1) * (segments + 1);

        for (int i = 0; i < segments; ++i) {
            indices.push_back(lastUpperRingStartIdx + i);
            indices.push_back(lastUpperRingStartIdx + (i + 1) % (segments + 1));
            indices.push_back(topPoleIdx);
        }

        // 3. Alt yarıküre indeksleri
        const int lowerHemisphereStartIdx = upperHemisphereStartIdx + (rings + 1) * (segments + 1) + 1;

        // Special connection between cylinder and lower hemisphere first ring
        for (int i = 0; i < segments; ++i) {
            const unsigned int cylinderBottom = i * 2 + 1;
            const unsigned int cylinderBottomNext = ((i + 1) % segments) * 2 + 1;
            const unsigned int sphereTop = lowerHemisphereStartIdx + i;
            const unsigned int sphereTopNext = lowerHemisphereStartIdx + (i + 1) % (segments + 1);

            indices.push_back(cylinderBottom);
            indices.push_back(sphereTop);
            indices.push_back(cylinderBottomNext);

            indices.push_back(cylinderBottomNext);
            indices.push_back(sphereTop);
            indices.push_back(sphereTopNext);
        }

        // Rest of lower hemisphere
        for (int y = 0; y < rings - 1; ++y) {
            const int ringStartIdx = lowerHemisphereStartIdx + y * (segments + 1);
            const int nextRingStartIdx = lowerHemisphereStartIdx + (y + 1) * (segments + 1);

            for (int i = 0; i < segments; ++i) {
                const unsigned int currentVert = ringStartIdx + i;
                const unsigned int nextVert = ringStartIdx + (i + 1) % (segments + 1);
                const unsigned int currentVertNext = nextRingStartIdx + i;
                const unsigned int nextVertNext = nextRingStartIdx + (i + 1) % (segments + 1);

                // First triangle
                indices.push_back(currentVert);
                indices.push_back(currentVertNext);
                indices.push_back(nextVert);

                // Second triangle
                indices.push_back(nextVert);
                indices.push_back(currentVertNext);
                indices.push_back(nextVertNext);
            }
        }

        // Lower hemisphere bottom cap
        const int lastLowerRingStartIdx = lowerHemisphereStartIdx + (rings - 1) * (segments + 1);
        const unsigned int bottomPoleIdx = lowerHemisphereStartIdx + (rings + 1) * (segments + 1);

        for (int i = 0; i < segments; ++i) {
            indices.push_back(lastLowerRingStartIdx + i);
            indices.push_back(bottomPoleIdx);
            indices.push_back(lastLowerRingStartIdx + (i + 1) % (segments + 1));
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->Initialize(vertices, indices);
        return mesh;
    }
} // namespace Primitives
