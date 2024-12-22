#pragma once
#include "Config.hpp"
#include "Math.hpp"
class Node;
class Camera3D;

class Camera3D : public Node3D
{
public:
    Camera3D();


    void setPerspective(float fov, float aspectRatio, float _near, float _far);
    void setView(const Vec3 &position, const Vec3 &target, const Vec3 &up);
    Mat4 getProjection() const;
    Mat4 getView() const;
    float getFov() const;
    float getAspectRatio() const;
    float getNear() const;
    float getFar() const;

private:
    float m_fov;
    float m_aspectRatio;
    float m_near;
    float m_far;

    mutable Mat4 m_proj;
    mutable Mat4 m_view;

};