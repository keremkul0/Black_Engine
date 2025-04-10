#include "Camera.h"

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

Camera::Camera(glm::vec3 position) : position(position),
                                     front(glm::vec3(0.0f, 0.0f, -1.0f)),
                                     worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
                                     yaw(-90.0f),
                                     pitch(0.0f) {
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return lookAt(position, position + front, up);
}

void Camera::LookAt(const glm::vec3 &target) {
    front = normalize(target - position);
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(const int direction, const float deltaTime) {
    const float velocity = MovementSpeed * deltaTime;

    switch (direction) {
        case FORWARD:
            position += front * velocity;
            break;
        case BACKWARD:
            position -= front * velocity;
            break;
        case LEFT:
            position -= right * velocity;
            break;
        case RIGHT:
            position += right * velocity;
            break;
        default: ;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, const bool constrainPitch) {
    constexpr float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(const float yoffset) {
    position += front * yoffset * 0.5f;
}

void Camera::UpdateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
    newFront.y = static_cast<float>(sin(glm::radians(pitch)));
    newFront.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

    front = normalize(newFront);
    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));
}

void Camera::Pan(const glm::vec3 &offset) {
    position += offset;
}

void Camera::SetPosition(const glm::vec3 &position) {
    this->position = position;
}

void Camera::SetTarget(const glm::vec3 &target) {
    this->front = normalize(target - this->position);
}
