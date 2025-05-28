#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

// Enable GLM experimental extensions
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>

namespace Math {

/**
 * @brief Decomposes a transformation matrix into translation, rotation, and scale components
 * 
 * @param transform The transformation matrix to decompose
 * @param translation Output translation vector
 * @param rotation Output rotation vector in Euler angles (radians)
 * @param scale Output scale vector
 * @return true if decomposition was successful
 */
inline bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
    using namespace glm;
    mat4 localMatrix(transform);

    // Translation
    translation = vec3(localMatrix[3]);

    // Debug log for debugging purposes
    std::cout << "Decomposing matrix - Translation: " << translation.x << ", "
              << translation.y << ", " << translation.z << std::endl;

    // Extracting scale by getting the length of matrix column vectors
    scale.x = length(vec3(localMatrix[0]));
    scale.y = length(vec3(localMatrix[1]));
    scale.z = length(vec3(localMatrix[2]));
    
    // Debug log for scale
    std::cout << "Decomposing matrix - Scale: " << scale.x << ", "
              << scale.y << ", " << scale.z << std::endl;

    // Normalize the matrix columns to remove scale
    if (scale.x != 0) localMatrix[0] /= scale.x;
    if (scale.y != 0) localMatrix[1] /= scale.y;
    if (scale.z != 0) localMatrix[2] /= scale.z;

    // Extract rotation using Math method
    // This is Euler angles extraction from rotation matrix (gimbal lock prone)
    rotation.y = asin(-localMatrix[0][2]);
    
    // Check for gimbal lock
    const float cosY = cos(rotation.y);
    
    if (cosY > 0.0001f) {
        // No gimbal lock
        rotation.x = atan2(localMatrix[1][2], localMatrix[2][2]);
        rotation.z = atan2(localMatrix[0][1], localMatrix[0][0]);
    } else {
        // Gimbal lock case
        rotation.x = atan2(-localMatrix[2][0], localMatrix[1][1]);
        rotation.z = 0;
        std::cout << "Gimbal lock detected in decompose" << std::endl;
    }

    // Debug log for rotation
    std::cout << "Decomposing matrix - Rotation (rad): " << rotation.x << ", "
              << rotation.y << ", " << rotation.z << std::endl;
              
    std::cout << "Decomposing matrix - Rotation (deg): " << glm::degrees(rotation.x) << ", "
              << glm::degrees(rotation.y) << ", " << glm::degrees(rotation.z) << std::endl;

    return true;
}

/**
 * @brief Alternative decomposition method using GLM's built-in decompose function
 */
inline bool DecomposeTransformGLM(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
    glm::quat rotationQuat;
    glm::vec3 skew;
    glm::vec4 perspective;
    
    bool success = glm::decompose(transform, scale, rotationQuat, translation, skew, perspective);
    
    // Debug logs for glm::decompose result
    std::cout << "GLM Decompose success: " << (success ? "yes" : "no") << std::endl;
    
    if (success) {
        // Convert quaternion to Euler angles
        rotation = glm::eulerAngles(rotationQuat);
        
        std::cout << "GLM Decompose - Translation: " << translation.x << ", " 
                  << translation.y << ", " << translation.z << std::endl;
        std::cout << "GLM Decompose - Scale: " << scale.x << ", " 
                  << scale.y << ", " << scale.z << std::endl;
        std::cout << "GLM Decompose - Rotation (rad): " << rotation.x << ", " 
                  << rotation.y << ", " << rotation.z << std::endl;
        std::cout << "GLM Decompose - Rotation (deg): " << glm::degrees(rotation.x) << ", " 
                  << glm::degrees(rotation.y) << ", " << glm::degrees(rotation.z) << std::endl;
        
        return true;
    }
    return false;
}

/**
 * @brief Debug function to print a matrix
 */
inline void PrintMatrix(const glm::mat4& matrix, const std::string& name = "Matrix") {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "[ ";
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[j][i] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

} // namespace Math

#endif // TRANSFORM_UTILS_H