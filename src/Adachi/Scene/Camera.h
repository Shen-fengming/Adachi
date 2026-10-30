#pragma once

#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace  Adachi{
class Camera{
public:
    Camera()
    {
        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }
    const glm::mat4 GetViewMatrix() const{return m_View;}
    const glm::mat4 GetProjectionMatrix() const{return m_Projection;}

    void SetEye(const glm::vec3& targetPos)
    {
        m_Eye = targetPos;
        UpdateViewMatrix();
    }
    void SetCenter(const glm::vec3& cameraPos)
    {
        m_Center = cameraPos;
        UpdateViewMatrix();
    }
    void SetUp(const glm::vec3& cameraUpVec)
    {
        m_Up = cameraUpVec;
        UpdateViewMatrix();
    }

    void SetZ(float near, float far)
    {
        m_ZNear = near;
        m_ZFar = far;
        UpdateProjectionMatrix();
    }
    void SetFovy(float degrees)
    {
        m_Fovy = degrees;
        UpdateProjectionMatrix();
    }
    void SetAspect(float width_divides_height)
    { 
        m_Aspect = width_divides_height;
        UpdateProjectionMatrix();
    }
private:
    void UpdateViewMatrix()
    {
        m_View = glm::lookAt(
            m_Eye,
            m_Center,
            m_Up
	    );
    }
    void UpdateProjectionMatrix()
    {
        m_Projection = glm::perspective(
		m_Fovy,
		m_Aspect,
		m_ZNear,
		m_ZFar
	);
    }
private:
    glm::vec3 m_Eye =       glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 m_Center =    glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Up =        glm::vec3(0.0f, 1.0f, 0.0f);

    float m_Fovy =      glm::radians(45.0f);
    float m_Aspect =    800.0f / 600.0f;
    float m_ZNear =     0.1f;
    float m_ZFar =      200.0f;

    glm::mat4 m_View;
    glm::mat4 m_Projection;
};
}