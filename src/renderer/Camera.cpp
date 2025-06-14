#include "Camera.h"
#include <algorithm>

Camera::Camera(float aspectRatio) 
    : m_position(5.0f, 5.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_distance(8.66f)
    , m_yaw(-45.0f)
    , m_pitch(-35.26f)
    , m_fov(45.0f)
    , m_aspectRatio(aspectRatio)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
    , m_isDirty(true) {
    updatePosition();
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_distance = glm::length(m_position - m_target);
    m_isDirty = true;
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
    updatePosition();
    m_isDirty = true;
}

void Camera::setDistance(float distance) {
    m_distance = std::max(0.1f, distance);
    updatePosition();
    m_isDirty = true;
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;
    
    // Clamp pitch to avoid gimbal lock
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    
    updatePosition();
    m_isDirty = true;
}

void Camera::zoom(float delta) {
    m_distance *= (1.0f + delta * 0.3f); // Changed from 0.1f to 0.3f for 3x faster zoom
    m_distance = std::clamp(m_distance, 0.1f, 50.0f);
    updatePosition();
    m_isDirty = true;
}

void Camera::pan(float deltaX, float deltaY) {
    glm::vec3 right = glm::normalize(glm::cross(m_position - m_target, m_up));
    glm::vec3 up = glm::normalize(glm::cross(right, m_position - m_target));
    
    float panSpeed = m_distance * 0.001f;
    m_target += right * deltaX * panSpeed + up * deltaY * panSpeed;
    updatePosition();
    m_isDirty = true;
}

const glm::mat4& Camera::getViewMatrix() {
    if (m_isDirty) {
        updateMatrices();
    }
    return m_viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() {
    if (m_isDirty) {
        updateMatrices();
    }
    return m_projectionMatrix;
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
    m_isDirty = true;
}

void Camera::setFOV(float fov) {
    m_fov = std::clamp(fov, 10.0f, 120.0f);
    m_isDirty = true;
}

void Camera::updatePosition() {
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    m_position.x = m_target.x + m_distance * cos(pitchRad) * cos(yawRad);
    m_position.y = m_target.y + m_distance * sin(pitchRad);
    m_position.z = m_target.z + m_distance * cos(pitchRad) * sin(yawRad);
}

void Camera::updateMatrices() {
    m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    m_isDirty = false;
}
