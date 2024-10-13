#pragma once
#include "Config.hpp"
#include "Node.hpp"
#include "Node3D.hpp"
#include "Mesh.hpp"
#include "Math.hpp"
#include "Node3D.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include "Map.hpp"

const u32 MAX_BONES = 80;

class Joint;
struct KeyFrame;
class Animator;
class Entity;

struct PosKeyFrame
{
    Vec3 pos;
    float frame;

    PosKeyFrame() 
    {
        pos.set(0.0f, 0.0f, 0.0f);
        frame = 0.0f;
    }

    PosKeyFrame(const Vec3 &p, float f)
    {
        pos.set(p.x, p.y, p.z);
        frame = f;
    }
};

struct RotKeyFrame
{
    Quaternion rot;
    float frame;
    RotKeyFrame() 
    {
        rot.identity();
        frame = 0.0f;
    }
    RotKeyFrame(const Quaternion &r, float f)
    {
        rot.set(r.x, r.y, r.z, r.w);
        frame = f;
    }
};

struct KeyFrame
{

    Vector<PosKeyFrame> positionKeyFrames;
    Vector<RotKeyFrame> rotationKeyFrames;

    KeyFrame() {};
    KeyFrame(KeyFrame *t);

    int GetPositionIndex(float animationTime);
    int GetRotationIndex(float animationTime);
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    Quaternion AnimateRotation(float movetime);

    Vec3 AnimatePosition(float movetime);
    u32 numRotationKeys() const { return (u32) rotationKeyFrames.size(); }
    u32 numPositionKeys() const { return (u32)positionKeyFrames.size(); }

    void setPositionKey(int frame, const Vec3 &p);
    void setRotationKey(int frame, const Quaternion &q);
    void AddPositionKeyFrame(float frame, const Vec3 &pos);
    void AddRotationKeyFrame(float frame, const Quaternion &rot);
};

struct Frame
{
    String name;
    Vec3 position;
    Quaternion orientation;
    KeyFrame keys;
    Vec3 src_pos, dest_pos;
    Quaternion src_rot, dest_rot;
    bool pos;
    bool rot;
    bool IgnorePosition;
    bool IgnoreRotation;
    Frame()
    {
        pos = false;
        rot = false;
        IgnorePosition = false;
        IgnoreRotation = false;
    }
};

class Animation
{
private:
    u64 n_frames;
    u64 state;
    u64 method;
    float currentTime;
    float duration;
    float fps;
    int mode;
    bool isEnd;
    Vector<Frame *> frames;
    Map<String, Frame *> framesMap;

public:
    enum
    {
        LOOP = 0,
        PINGPONG = 1,
        ONESHOT = 2
    };
    enum
    {
        Stoped = 0,
        Looping = 1,
        Playing = 2
    };
    String name;

    Animation(const String &name);
    ~Animation();

    void Force();

    bool Save(const String &name);
    bool Load(const String &name);

    float GetDuration();
    float GetTime();
    float GetFPS();
    int GetMode();
    u64 GetState();

    String GetName() const { return name; }

    bool Play(u32 mode, float fps);
    bool Stop();
    bool IsEnded();

    Frame *AddFrame(const String &name);
    Frame *GetFrame(const String &name);
    Frame *GetFrame(int index);

    void Update(float elapsed);
};

class Animator
{
public:
    Animator(Entity *parent);
    ~Animator();

    void Update(float elapsed);

    void SaveAllFrames(const String &path);

    Animation *LoadAnimation(const String &name);

    Animation *AddAnimation(const String &name);

    Animation *GetAnimation(const String &name);

    Animation *GetAnimation(int index);

    u32 GetTotalAnimations() const;

    void SetIgnorePosition(const String &name, bool ignore);
    void SetIgnoreRotation(const String &name, bool ignore);

    bool Play(const String &name, int mode = Animation::LOOP, float blendTime = 0.25f);

    void Stop();
    bool IsEnded();
    bool IsPlaying();

    String GetCurrentAnimationName();
    float GetCurrentFrame();

private:
    Vector<Animation *> m_animations;
    Map<String, Animation *> m_animations_map;
    Entity *entity;

    Animation *currentAnimation;

    String currentAnimationName;

    float blendFactor;
    float blendTime;
    bool blending;

    void beginTrans();
    void updateTrans(float blend);
    void updateAnim(float elapsed);
};

class Joint : public Node3D
{
public:
    Mat4 offset;
    String parentName;
    Joint() : Node3D() {}
    virtual ~Joint() {}
};


class SkinSurface
{
    friend class Mesh;
    friend class Entity;

    Trivial<u32> faces;
    Vector<SkinVertex> vertices;
    int material;
    u32 VAO;
    u32 VBO, EBO;
    String name;

public:
    SkinSurface(int material = 0) { this->material = material; }

    void SetMaterial(int material) { this->material = material; }

    
    const Vec3 GetVertex(int id) { return vertices[id].pos; }
    void SetVertex(int id, const Vec3 &v) { vertices[id].pos = v; }
    void SetVertex(int id, float x, float y, float z) { vertices[id].pos.set(x, y, z); }

    void AddVertex(const SkinVertex &vertex);

    void AddVertex(const Vec3 &v);

    void AddVertex(const Vec3 &v, const Vec2 &uv);

    void AddVertex(const Vec3 &v, const Vec2 &uv, const Vec3 &normal);

    void AddFace(int a, int b, int c);

    void AddIndex(int a);

    void Render() ;

    void Build();

    void Release();
};

class Entity : public Node3D
{

    Vector<Joint *> joints;
    Vector<SkinSurface *> surfaces;
    Vector<Material *> materials;
    Vector<Mat4> bones;
    Animator *animator;

    friend class Animation;
    friend class Animator;

public:
    Entity();

    void UpdateAnimation(float dt);

    Animation *LoadAnimation(const String &name);

    void Play(const String &name, int mode = Animation::LOOP, float blendTime = 0.25f);

    void SetTexture(int layer = 0, Texture2D *texture = nullptr);

    u32 GetTotalJoints() const;
    u32 GetTotalSurfaces() const;
    u32 GetTotalBones() const;
    u32 GetTotalMaterials() const;

    Mat4 GetBone(u32 index) const;

    Material *AddMaterial();

    void AddJoint(Joint *joint);

    SkinSurface *AddSurface();

    Animator *GetAnimator();

    Joint *GetJoint(u32 id);
    Joint *FindJoint(const char *name);
    int GetJointIndex(const char *name);
    
    void Render(Shader *shader) override;
    void Release() override;

    void RenderNoMaterial(Shader *shader);

    bool Save(const String &name);

    bool Load(const String &name);
};
