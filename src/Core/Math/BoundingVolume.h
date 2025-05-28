#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.h"
#include <array>
#include <limits>

namespace Math {

/**
 * @brief Represents a bounding sphere in 3D space
 */
class BoundingSphere {
public:
    BoundingSphere() : m_Center(0.0f), m_Radius(1.0f) {}
    
    BoundingSphere(const glm::vec3& center, float radius)
        : m_Center(center), m_Radius(radius) {}
    
    /**
     * @brief Test if a ray intersects with this sphere
     * 
     * @param ray The ray to test against
     * @param t Output parameter: distance along ray to intersection point (if hit)
     * @return true if the ray intersects the sphere
     */
    bool IntersectsRay(const Ray& ray, float& t) const {
        // Vector from ray origin to sphere center
        const glm::vec3 oc = ray.GetOrigin() - m_Center;
        
        // Quadratic formula coefficients
        const float a = glm::dot(ray.GetDirection(), ray.GetDirection()); // Usually 1.0 if normalized
        const float b = 2.0f * glm::dot(oc, ray.GetDirection());
        const float c = glm::dot(oc, oc) - m_Radius * m_Radius;
        
        // Calculate discriminant
        const float discriminant = b * b - 4.0f * a * c;
        
        // If discriminant is negative, ray misses sphere
        if (discriminant < 0.0f) {
            return false;
        }
        
        // Calculate intersection distance
        const float sqrtDiscriminant = std::sqrt(discriminant);
        
        // Find the nearest intersection point in the ray's forward direction
        float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t1 = (-b + sqrtDiscriminant) / (2.0f * a);
        
        // Check if intersection is in front of ray origin
        if (t0 > 0.0f) {
            t = t0;
            return true;
        }
        
        if (t1 > 0.0f) {
            t = t1;
            return true;
        }
        
        return false; // Both intersections are behind the ray
    }
    
    /**
     * @brief Get the center of the sphere
     */
    [[nodiscard]] const glm::vec3& GetCenter() const { return m_Center; }
    
    /**
     * @brief Get the radius of the sphere
     */
    [[nodiscard]] float GetRadius() const { return m_Radius; }
    
    /**
     * @brief Set the center of the sphere
     */
    void SetCenter(const glm::vec3& center) { m_Center = center; }
    
    /**
     * @brief Set the radius of the sphere
     */
    void SetRadius(float radius) { m_Radius = radius > 0.0f ? radius : 0.001f; }
    
private:
    glm::vec3 m_Center;
    float m_Radius;
};

/**
 * @brief Represents an axis-aligned bounding box in 3D space
 */
class AABB {
public:
    AABB() : m_Min(-1.0f), m_Max(1.0f) {}
    
    AABB(const glm::vec3& min, const glm::vec3& max)
        : m_Min(min), m_Max(max) {}
    
    /**
     * @brief Test if a ray intersects with this AABB
     * 
     * @param ray The ray to test against
     * @param t Output parameter: distance along ray to intersection point (if hit)
     * @return true if the ray intersects the AABB
     */
    bool IntersectsRay(const Ray& ray, float& t) const {
        const glm::vec3& rayOrigin = ray.GetOrigin();
        const glm::vec3& rayDir = ray.GetDirection();
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();
        
        // Test intersection with each slab (pair of planes)
        for (int i = 0; i < 3; ++i) {
            if (std::abs(rayDir[i]) < std::numeric_limits<float>::epsilon()) {
                // Ray is parallel to the slab, check if origin is within the slab
                if (rayOrigin[i] < m_Min[i] || rayOrigin[i] > m_Max[i]) {
                    return false;
                }
            } else {
                // Compute intersection with the two planes of the slab
                float invD = 1.0f / rayDir[i];
                float t1 = (m_Min[i] - rayOrigin[i]) * invD;
                float t2 = (m_Max[i] - rayOrigin[i]) * invD;
                
                // Ensure t1 <= t2
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                
                // Update tMin and tMax
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                
                // Early exit if no intersection
                if (tMin > tMax) {
                    return false;
                }
            }
        }
        
        // If we get here, the ray intersects the AABB
        t = tMin;
        return true;
    }
    
    /**
     * @brief Get the minimum corner of the AABB
     */
    [[nodiscard]] const glm::vec3& GetMin() const { return m_Min; }
    
    /**
     * @brief Get the maximum corner of the AABB
     */
    [[nodiscard]] const glm::vec3& GetMax() const { return m_Max; }
    
    /**
     * @brief Set the minimum corner of the AABB
     */
    void SetMin(const glm::vec3& min) { m_Min = min; }
    
    /**
     * @brief Set the maximum corner of the AABB
     */
    void SetMax(const glm::vec3& max) { m_Max = max; }
    
    /**
     * @brief Get the center of the AABB
     */
    [[nodiscard]] glm::vec3 GetCenter() const { return (m_Min + m_Max) * 0.5f; }
    
    /**
     * @brief Get the extents (half-size) of the AABB
     */
    [[nodiscard]] glm::vec3 GetExtents() const { return (m_Max - m_Min) * 0.5f; }
    
private:
    glm::vec3 m_Min; // Minimum corner
    glm::vec3 m_Max; // Maximum corner
};

/**
 * @brief Represents an AABB transformed into world space
 * 
 * This class handles an AABB that has been transformed by a matrix,
 * supporting non-axis-aligned orientations in world space while 
 * maintaining fast intersection tests.
 */
class TransformedAABB {
public:
    TransformedAABB() 
        : m_LocalAABB(), m_WorldTransform(1.0f), m_TransformDirty(true) {}
    
    TransformedAABB(const AABB& localAABB, const glm::mat4& worldTransform) 
        : m_LocalAABB(localAABB), m_WorldTransform(worldTransform), m_TransformDirty(true) {}
    
    /**
     * @brief Update the world transform of this AABB
     * @param transform The new world transform matrix
     */
    void UpdateTransform(const glm::mat4& transform) {
        m_WorldTransform = transform;
        m_TransformDirty = true;
    }
    
    /**
     * @brief Test if a ray intersects with the transformed AABB
     * 
     * @param ray The ray to test against in world space
     * @param t Output parameter: distance along ray to intersection point (if hit)
     * @return true if the ray intersects the transformed AABB
     */
    bool IntersectsRay(const Ray& ray, float& t) const {
        UpdateWorldBounds();
        
        // First, do a quick test against the world-space AABB
        if (m_WorldAABB.IntersectsRay(ray, t)) {
            // For better accuracy, transform the ray to local space and test against the local AABB
            glm::mat4 inverseTransform = glm::inverse(m_WorldTransform);
            
            // Transform the ray to local space
            glm::vec3 localOrigin = glm::vec3(inverseTransform * glm::vec4(ray.GetOrigin(), 1.0f));
            glm::vec3 localDir = glm::normalize(glm::vec3(inverseTransform * glm::vec4(ray.GetDirection(), 0.0f)));
            Ray localRay(localOrigin, localDir);
            
            float localT;
            if (m_LocalAABB.IntersectsRay(localRay, localT)) {
                // Convert intersection distance back to world space
                glm::vec3 localHitPoint = localRay.GetPointAtDistance(localT);
                glm::vec3 worldHitPoint = glm::vec3(m_WorldTransform * glm::vec4(localHitPoint, 1.0f));
                t = glm::length(worldHitPoint - ray.GetOrigin());
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief Get the local-space AABB
     */
    [[nodiscard]] const AABB& GetLocalAABB() const { return m_LocalAABB; }
    
    /**
     * @brief Set the local-space AABB
     */
    void SetLocalAABB(const AABB& aabb) { 
        m_LocalAABB = aabb; 
        m_TransformDirty = true;
    }
    
    /**
     * @brief Get the world-space AABB (bounding box of the transformed AABB)
     */
    [[nodiscard]] const AABB& GetWorldAABB() const {
        UpdateWorldBounds();
        return m_WorldAABB;
    }
    
    /**
     * @brief Get the world transform matrix
     */
    [[nodiscard]] const glm::mat4& GetWorldTransform() const { return m_WorldTransform; }
    
private:
    /**
     * @brief Update the world-space AABB if the transform has changed
     */
    void UpdateWorldBounds() const {
        if (!m_TransformDirty) {
            return;
        }
        
        // Get the eight corners of the local AABB
        std::array<glm::vec3, 8> corners;
        const glm::vec3& min = m_LocalAABB.GetMin();
        const glm::vec3& max = m_LocalAABB.GetMax();
        
        corners[0] = glm::vec3(min.x, min.y, min.z);
        corners[1] = glm::vec3(max.x, min.y, min.z);
        corners[2] = glm::vec3(min.x, max.y, min.z);
        corners[3] = glm::vec3(max.x, max.y, min.z);
        corners[4] = glm::vec3(min.x, min.y, max.z);
        corners[5] = glm::vec3(max.x, min.y, max.z);
        corners[6] = glm::vec3(min.x, max.y, max.z);
        corners[7] = glm::vec3(max.x, max.y, max.z);
        
        // Initialize min and max points for world AABB
        glm::vec3 worldMin(std::numeric_limits<float>::max());
        glm::vec3 worldMax(std::numeric_limits<float>::lowest());
        
        // Transform each corner to world space and expand the world AABB
        for (const auto& corner : corners) {
            glm::vec3 worldCorner = glm::vec3(m_WorldTransform * glm::vec4(corner, 1.0f));
            worldMin = glm::min(worldMin, worldCorner);
            worldMax = glm::max(worldMax, worldCorner);
        }
        
        // Update the world AABB with the new bounds
        m_WorldAABB = AABB(worldMin, worldMax);
        m_TransformDirty = false;
    }
    
    AABB m_LocalAABB;               // The local-space AABB
    glm::mat4 m_WorldTransform;     // The world transformation matrix
    mutable AABB m_WorldAABB;       // The world-space AABB (computed on demand)
    mutable bool m_TransformDirty;  // Flag indicating if the world AABB needs to be recomputed
};

} // namespace Math

#endif // BOUNDING_VOLUME_H