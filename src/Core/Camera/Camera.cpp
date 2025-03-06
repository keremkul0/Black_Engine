#include "Camera.h"
#include <GLFW/glfw3.h>  // For key definitions

// Directions
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
    return glm::lookAt(position, position + front, up);
}

void Camera::LookAt(const glm::vec3 &target) {
    front = glm::normalize(target - position);
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

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
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    float sensitivity = 0.1f;
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

void Camera::ProcessMouseScroll(float yoffset) {
    position += front * yoffset * 0.5f;
}

void Camera::UpdateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::Pan(const glm::vec3 &offset) {
    position += offset;
}

void Camera::SetPosition(const glm::vec3 &position) {
    // Set the camera position
    this->position = position;
}

void Camera::SetTarget(const glm::vec3 &target) {
    // Set where the camera is looking at
    this->front = glm::normalize(target - this->position);
}
