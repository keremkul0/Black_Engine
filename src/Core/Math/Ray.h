#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

namespace Math {

/**
 * @brief Represents a ray in 3D space with an origin and direction
 */
class Ray {
public:
    /**
     * @brief Default constructor - creates a ray pointing along the negative Z axis
     */
    Ray() : m_Origin(0.0f), m_Direction(0.0f, 0.0f, -1.0f) {}
    
    /**
     * @brief Construct a ray with specific origin and direction
     * @param origin Ray origin point
     * @param direction Ray direction vector (will be normalized)
     */
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : m_Origin(origin), m_Direction(glm::normalize(direction)) {}
    
    /**
     * @brief Get the origin point of the ray
     */
    [[nodiscard]] const glm::vec3& GetOrigin() const { return m_Origin; }
    
    /**
     * @brief Get the normalized direction vector of the ray
     */
    [[nodiscard]] const glm::vec3& GetDirection() const { return m_Direction; }
    
    /**
     * @brief Set the origin point of the ray
     */
    void SetOrigin(const glm::vec3& origin) { m_Origin = origin; }
    
    /**
     * @brief Set the direction vector of the ray (will be normalized)
     */
    void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }
    
    /**
     * @brief Get the point at distance t along the ray
     * @param t Distance along the ray
     * @return Point at distance t from origin along the direction
     */
    [[nodiscard]] glm::vec3 GetPoint(float t) const {
        return m_Origin + m_Direction * t;
    }
    
    /**
     * @brief Get the point at distance t along the ray (alias for GetPoint)
     * @param t Distance along the ray
     * @return Point at distance t from origin along the direction
     */
    [[nodiscard]] glm::vec3 GetPointAtDistance(float t) const {
        return GetPoint(t);
    }

private:
    glm::vec3 m_Origin;     // Origin point of the ray
    glm::vec3 m_Direction;  // Normalized direction vector
};

} // namespace Math

#endif // RAY_H