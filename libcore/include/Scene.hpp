#pragma once

#include "Core.hpp"
#include "Math.hpp"




class Heightmap
{
    private:
        int width, height;
        float *heightData;
        float maxHeight;

        u32 readRawValue(const u8* buffer, int bitDepth) const;
        void writeRawValue(u8* buffer, int bitDepth, float value) const;

    public:
        Heightmap(float heightScale = 100.0f);
        ~Heightmap();
        
        bool LoadImage(const char *filename);
        bool Load(const char *filename,int bitDepth);
        bool Save(const char *filename, int bitDepth);
        
        Vec3 GetNormal(int x, int y) ;
        int GetHeight() { return height; }

        float GetHeight(int x, int y);
         void SetHeight(int x, int z, float height); 
        int GetWidth() { return width; }
        float GetMaxHeight() { return maxHeight; }
        float GetInterpolatedHeight(float x, float z) ;
};

class Transform
{
private:
    Vec3 position;
    Quat rotation;
    Vec3 scale;

    Mat4 localMatrix;
    Mat4 worldMatrix;

    Transform *parent;
    bool isDirty;

public:
    Transform();
    ~Transform() = default;


    void SetPosition(const Vec3 &pos, bool global = false);
    void SetRotation(const Quat &rot, bool global = false);
    void SetScale(const Vec3 &scl, bool global = false);

    Vec3 GetPosition(bool global = false) const;
    Quat GetRotation(bool global = false) const;
    Vec3 GetScale(bool global = false) const;


    void Move(const Vec3 &delta, bool global = false);
    void Turn(const Quat &rotation, bool global = false);
    void LookTo(const Vec3 &target, const Vec3 &up = Vec3(0.0f, 1.0f, 0.0f));


    const Mat4 &GetLocalMatrix();
    const Mat4 &GetWorldMatrix();


    void SetParent(Transform *newParent);

private:
    void UpdateMatrices();
    void MarkDirty();
};

class Node3D
{

    protected:
        Node3D* Parent;
        std::string Name;
        Vec3 RelativeTranslation;
        Vec3 RelativeRotation;
        Vec3 RelativeScale;
        Mat4 AbsoluteTransformation;
    public:
        Node3D(const Vec3 &position = Vec3(0.0f, 0.0f, 0.0f), const Vec3 &rotation = Vec3(0.0f, 0.0f, 0.0f), const Vec3 &scale = Vec3(1.0f, 1.0f, 1.0f));
        virtual ~Node3D();

        void SetParent(Node3D* parent);
        Node3D* GetParent() { return Parent; }

        void SetName(const std::string& name) { Name = name; }
        const std::string& GetName() const { return Name; }

        void UpdateAbsolutePosition();
        const Mat4 GetAbsoluteTransformation() const { return AbsoluteTransformation; }
        Mat4 GetRelativeTransformation() const;

        const Vec3& GetScale() const { return RelativeScale; }
        const Vec3& GetRotation() const { return RelativeRotation; }
        const Vec3& GetPosition() const { return RelativeTranslation; }
        Vec3 GetAbsolutePosition();

        void SetScale(const Vec3& scale) { RelativeScale = scale; }
        void SetRotation(const Vec3& rotation) { RelativeRotation = rotation; }
        void SetPosition(const Vec3& position) { RelativeTranslation = position; }

        virtual void Render();
        virtual void Update(u32 time);
};

class Camera : public Node3D
{
    private:

        Vec3 Up;
        Vec3 Target;
        float FOV;
        float ZNear;
        float ZFar;
        float Aspect;
        bool TargetAndRotationAreBound;
        bool IsOrthogonal;

    private:
         void RecalculateProjectionMatrix();

    public:
        Camera(const Vec3 &position = Vec3(0.0f, 0.0f, 0.0f), const Vec3 &lookat = Vec3(0.0f, 0.0f, 1000.0f));

        ~Camera() = default;

        void SetProjectionMatrix(const Mat4& mat, bool isOrtho = false);
        const Mat4 &GetProjectionMatrix() const;
        void SetViewMatrix(const Mat4 &mat);
        const Mat4 &GetViewMatrix() const;
        void SetTargetAndRotation(bool bound);

        void SetTarget(const Vec3& target) { Target = target; }
        const Vec3& GetTarget() const { return Target; }
       
        void SetUp(const Vec3& up) { Up = up; }
        const Vec3& GetUp() const { return Up; }

        void SetRotation(const Vec3 &rotation);

        void SetFOV(float fov) { FOV = fov; }
        void SetZNear(float zNear) { ZNear = zNear; }
        void SetZFar(float zFar) { ZFar = zFar; }
        void SetAspect(float aspect) { Aspect = aspect; }

        float GetFOV() const { return FOV; }
        float GetZNear() const { return ZNear; }
        float GetZFar() const { return ZFar; }
        float GetAspect() const { return Aspect; }

        void Render() override;
};