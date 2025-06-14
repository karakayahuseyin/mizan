#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    
    float m_distance;
    float m_yaw;
    float m_pitch;
    
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    bool m_isDirty;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

public:
    Camera(float aspectRatio = 16.0f/9.0f);
    
    // Camera controls
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setDistance(float distance);
    void rotate(float deltaYaw, float deltaPitch);
    void zoom(float delta);
    void pan(float deltaX, float deltaY);
    
    // Getters
    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getTarget() const { return m_target; }
    float getDistance() const { return m_distance; }
    
    // Matrix getters
    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();
    
    // Settings
    void setAspectRatio(float aspectRatio);
    void setFOV(float fov);
    
private:
    void updatePosition();
    void updateMatrices();
};

#endif // CAMERA_H
