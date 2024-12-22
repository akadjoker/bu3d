#pragma once
#include "Config.hpp"
#include "Math.hpp"
class Node;


class Node3D : public Node
{
public:
    Node3D();
    
    void Update(float dt) override;


    void setLocalPosition( const Vec3 &v );
	void setLocalScale( const Vec3 & v );
	void setLocalRotation( const Quaternion &q );
	void setLocalTform( const Mat4 &t );

	void setWorldPosition( const Vec3 &v );
	void setWorldScale( const Vec3 &v );
	void setWorldRotation( const Quaternion &q );
	void setWorldTform( const Mat4 &t );

    void Rotate(float p, float y, float r, bool global);
    void Turn(float x, float y, float z, bool global);
    void Scale(float x, float y, float z, bool global);
    void Position(float x, float y, float z, bool global);
    void Translate(float x, float y, float z, bool global);
    void Point(Node3D *target, float rool);
    void Move(float x, float y, float z);

    const Vec3 &getLocalPosition()const;
	const Vec3 &getLocalScale()const;
	const Quaternion &getLocalRotation()const;
    const Mat4 &getLocalTransform()const;

	const Vec3 &getWorldPosition()const;
	const Vec3 &getWorldScale()const;
	const Quaternion &getWorldRotation()const;
	const Mat4 &getWorldTransform()const;

	void setParent(Node3D *parent);
	Node3D *getParent()const;


protected:
    Node3D *m_parent;
	String m_parentName;
	Quaternion local_rot;
	Vec3 local_pos,local_scl;
    mutable  bool m_dirty;
	mutable Mat4 local_tform;
	mutable Quaternion world_rot;
	mutable Vec3 world_pos,world_scl;
	mutable Mat4 world_tform;

private:
    void updateTransform() const;
};
