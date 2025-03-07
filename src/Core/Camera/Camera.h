#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));

    void ProcessKeyboard(int direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    void ProcessMouseScroll(float yoffset);

    void Pan(const glm::vec3 &offset);

    void SetPosition(const glm::vec3 &position);

    void LookAt(const glm::vec3 &target);

    void SetTarget(const glm::vec3 &target);
    void SetFront(const glm::vec3& front) { m_Front = front; }
    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    [[nodiscard]] glm::vec3 GetPosition() const { return position; }
    [[nodiscard]] glm::vec3 GetFront() const { return front; }

    // Camera options
    float MovementSpeed = 2.5f;

private:
    void UpdateCameraVectors();

    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up{};
    glm::vec3 right{};
    glm::vec3 worldUp;
    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    // Euler angles
    float yaw;
    float pitch;
};
