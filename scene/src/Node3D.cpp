
#include "Node.hpp"
#include "Node3D.hpp"




Node3D::Node3D()
{
    m_parent = nullptr;
    m_dirty = true;
    local_pos = Vec3(0.0f, 0.0f, 0.0f);
    local_scl = Vec3(1.0f, 1.0f, 1.0f);
    local_rot = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

    world_pos = Vec3(0.0f, 0.0f, 0.0f);
    world_scl = Vec3(1.0f, 1.0f, 1.0f);
    world_rot = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

const Vec3 &Node3D::getLocalPosition() const
{
    return local_pos;
}

const Vec3 &Node3D::getWorldPosition() const
{
    const Mat4 &t = getWorldTransform();
    world_pos = Vec3(t.c[3][0], t.c[3][1], t.c[3][2]);
    return  world_pos;
}

const Vec3 &Node3D::getLocalScale() const
{
    return local_scl;
}

const Vec3 &Node3D::getWorldScale() const
{
    world_scl = (m_parent == nullptr) ? local_scl : m_parent->getWorldScale() * local_scl;
    return world_scl;
}

const Quaternion &Node3D::getLocalRotation() const
{
    return local_rot;
}

const Quaternion &Node3D::getWorldRotation() const
{
    world_rot = (m_parent == nullptr) ? local_rot : m_parent->getWorldRotation() * local_rot;
    return world_rot;
}

const Mat4 &Node3D::getLocalTransform() const
{
    if (m_dirty)
    {
        updateTransform();
        m_dirty = false;
    }

    return local_tform;
}

const Mat4 &Node3D::getWorldTransform() const
{
    if (m_dirty)
    {
        updateTransform();
        world_tform = (m_parent == nullptr) ? getLocalTransform() : m_parent->getWorldTransform() * getLocalTransform();
        m_dirty = false;
    }
    return world_tform;
}

void Node3D::setParent(Node3D *parent)
{
    m_parent = parent;
    m_dirty = true;
}

Node3D *Node3D::getParent() const
{
    return  m_parent;
}

 void Node3D::updateTransform() const
{
    Mat4 translation = Mat4::Translate(local_pos.x, local_pos.y, local_pos.z);
    Mat4 rotationMatrix = Mat4(local_rot);
    Mat4 scaling = Mat4::Scale(local_scl.x, local_scl.y, local_scl.z);
    local_tform = translation * rotationMatrix * scaling;
}

void Node3D::Update(float dt)
{
    (void)dt;
}

void Node3D::setLocalPosition(const Vec3 &v)
{
    local_pos = v;
    m_dirty = true;
}

void Node3D::setLocalScale(const Vec3 &v)
{
    local_scl = v;
    m_dirty = true;
}

void Node3D::setLocalRotation(const Quaternion &q)
{
    local_rot = q;
    m_dirty = true;
}

void Node3D::setLocalTform(const Mat4 &t)
{
    Vec3 r;
    t.decompose(local_scl, r, local_pos);
    local_rot.FromEuler(r);
    local_tform = t;
    m_dirty = true;
}

void Node3D::setWorldPosition(const Vec3 &v)
{
    if (m_parent != nullptr)
    {
        // setLocalPosition(m_parent->getWorldTransform() * v);
        Vec3 localPosition = m_parent->getWorldTransform().inverted() * v;
        setLocalPosition(localPosition);
    }
    else
    {
        setLocalPosition(v);
    }
}

void Node3D::setWorldScale(const Vec3 &v)
{
    if (m_parent != nullptr)
    {
        Vec3 localScale = v / m_parent->getWorldScale();
        setLocalScale(localScale);
    }
    else
    {
        setLocalScale(v);
    }
}

void Node3D::setWorldRotation(const Quaternion &q)
{
    if (m_parent != nullptr)
    {
        Quaternion localRotation = m_parent->getWorldRotation().inverted() * q;
        setLocalRotation(localRotation);

        // Mat4 worldTransform = m_parent->getWorldTransform();
        // Mat4 rotation = Mat4(q);
        // Mat4 combinedMatrix  = worldTransform * rotation;
        // setLocalRotation(combinedMatrix.toQuaternion());
    }
    else
    {
        setLocalRotation(q);
    }
}

void Node3D::setWorldTform(const Mat4 &t)
{
    if (m_parent != nullptr)
    {
        setLocalTform(m_parent->getWorldTransform() * t);
    }
    else
    {
        setLocalTform(t);
    }
}


inline Quaternion pitchQuaternion(float p)
{
    return Quaternion(sinf(p / -2), 0, 0,cosf(p / -2));
}

inline Quaternion yawQuaternion(float y)
{
    return Quaternion(0, sinf(y / 2), 0,cosf(y / 2));
}

inline Quaternion rollQuaternion(float r)
{
    return Quaternion(0, 0, sinf(r / -2),cosf(r / -2));
}
Quaternion rotationQuaternion(float p, float y, float r)
{
    return yawQuaternion(y) * pitchQuaternion(p) * rollQuaternion(r);
}
void Node3D::Rotate(float p, float y, float r, bool global)
{
    if (global)
    {
        setWorldRotation(rotationQuaternion( p*dtor,y*dtor,r*dtor));
    }
    else
    {
        setLocalRotation(rotationQuaternion( p*dtor,y*dtor,r*dtor));
    }
}

void Node3D::Turn(float p, float y, float r, bool global)
{
    if (global)
    {
        setWorldRotation(rotationQuaternion( p*dtor,y*dtor,r*dtor)* getWorldRotation());
    }
    else
    {

        setLocalRotation(getLocalRotation() * rotationQuaternion( p*dtor,y*dtor,r*dtor));
    }
}

void Node3D::Scale(float x, float y, float z, bool global)
{
    if (global)
    {
        setWorldScale(Vec3(x, y, z));
    }
    else
    {
        setLocalScale(Vec3(x, y, z));
    }
}

void Node3D::Position(float x, float y, float z, bool global)
{
    if (global)
    {
        setWorldPosition(Vec3(x, y, z));
    }
    else
    {
        setLocalPosition(Vec3(x, y, z));
    }
}

void Node3D::Translate(float x, float y, float z, bool global)
{
    if (global)
    {
        setWorldPosition(getWorldPosition() + Vec3(x, y, z));
    }
    else
    {
        setLocalPosition(getLocalPosition() + Vec3(x, y, z));
    }
}

void Node3D::Point(Node3D *target, float roll)
{
    Vec3 targetPos = target->getWorldTransform().getTrans();
    Vec3 currentPos = getWorldTransform().getTrans();
    Vec3 direction = (targetPos - currentPos).normalized();

    // Criar uma matriz LookAt baseada na direção
    Mat4 lookAtMatrix = Mat4::LookAt(currentPos, targetPos, Vec3(0, 1, 0)); // Supondo Y-Up
    Quaternion lookAtRotation = lookAtMatrix.toQuaternion();

    // Aplicar o roll ao quaternion
    setWorldRotation(lookAtRotation * Quaternion(0, 0, roll * dtor));
    // Vec3 b = target->getWorldTransform().getTrans();
    // Vec3 a = getWorldTransform().getTrans();
    // Vec3 v = a - b;
    // setWorldRotation(Quaternion(v.x, v.y, rool * dtor) );
}

void Node3D::Move(float x, float y, float z)
{
    Vec3 translation(x, y, z);
    Vec3 rotatedTranslation = getLocalRotation() * translation;
    setLocalPosition(getLocalPosition() + rotatedTranslation);
}