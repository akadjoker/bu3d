
#include "Node.hpp"
#include "Node3D.hpp"
#include "Camera3D.hpp"


Camera3D::Camera3D()
{
    m_fov = 45.0f;
    m_aspectRatio = 1.0f;
    m_near = 0.01f;
    m_far = 1000.0f;
    m_proj = Mat4::Perspective(m_fov, m_aspectRatio, m_near, m_far);
    m_view = Mat4();
}

void Camera3D::setPerspective(float fov, float aspectRatio, float _near, float _far)
{
    m_fov = fov;
    m_aspectRatio = aspectRatio;
    m_near = _near;
    m_far = _far;
    m_proj = Mat4::Perspective(m_fov, m_aspectRatio, m_near, m_far);
}

void Camera3D::setView(const Vec3 &position, const Vec3 &target, const Vec3 &up)
{
    m_view = Mat4::LookAt(position, target, up);
}

Mat4 Camera3D::getProjection() const
{
    return m_proj;
}

Mat4 Camera3D::getView() const
{
    m_view = getWorldTransform().inverted();
    return m_view;
}

float Camera3D::getFov() const
{
    return m_fov;
}

float Camera3D::getAspectRatio() const
{
    return m_aspectRatio;
}

float Camera3D::getNear() const
{
    return m_near;
}

float Camera3D::getFar() const  
{
    return m_far;
}


