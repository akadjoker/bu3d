#include "Scene.hpp"
#include "Scene.hpp"
#include <cmath>
#include "stb_image.h"

Heightmap::Heightmap(float heightScale)
{
    maxHeight = heightScale;
    width = 0;
    height = 0;
    heightData = nullptr;
}

Heightmap::~Heightmap()
{
    if (heightData)
    {
        delete[] heightData;
        heightData = nullptr;
    }
}

u32 Heightmap::readRawValue(const u8 *buffer, int bitDepth) const
{
    uint32_t value = 0;
       for (int i = 0; i < bitDepth / 8; ++i)
    {
        value |= buffer[i] << (i * 8);
    }
    return value;
}

void Heightmap::writeRawValue(u8 *buffer, int bitDepth, float value) const
{
    uint32_t intValue = static_cast<uint32_t>(value * ((1 << bitDepth) - 1));
    for (int i = 0; i < bitDepth / 8; ++i)
    {
        buffer[i] = (intValue >> (i * 8)) & 0xFF;
    }
}

bool Heightmap::LoadImage(const char *filename)
{
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_grey);

    if (data)
    {
        if (heightData)
        {
            delete[] heightData;
        }
        heightData = new float[width * height];
        for (int i = 0; i < width * height; i++)
        {
            // Normaliza para 0-1 e multiplica pela altura máxima
            heightData[i] = (data[i] / 255.0f) * maxHeight;
        }

        stbi_image_free(data);
        if (width <= 0 || height <= 0)
        {
            Utils::LogError("Invalid heightmap dimensions: %s", filename);
            return false;
        }

        Utils::LogInfo("Loaded heightmap: %s [%d x %d ] channels: %d", filename, width, height, channels);
        return true;
    }
    Utils::LogError("Failed to load heightmap: %s", filename);
    return false;
}

float Heightmap::GetHeight(int x, int y)
{
    if (x < 0)
        x = 0;
    if (x >= width)
        x = width - 1;
    if (y < 0)
        y = 0;
    if (y >= height)
        y = height - 1;

    return heightData[x + y * width];
}

void Heightmap::SetHeight(int x, int y, float value)
{
    if (x < 0)
        x = 0;
    if (x >= width)
        x = width - 1;
    if (y < 0)
        y = 0;
    if (y >= height)
        y = height - 1;

    heightData[x + y * width] = (value / 255.0f) * maxHeight;
}

float Heightmap::GetInterpolatedHeight(float x, float z)
{
    int x0 = static_cast<int>(x);
    int z0 = static_cast<int>(z);
    float fx = x - x0;
    float fz = z - z0;

    float h00 = GetHeight(x0, z0);
    float h10 = GetHeight(x0 + 1, z0);
    float h01 = GetHeight(x0, z0 + 1);
    float h11 = GetHeight(x0 + 1, z0 + 1);

    // Interpolação bilinear
    float h0 = h00 * (1 - fx) + h10 * fx;
    float h1 = h01 * (1 - fx) + h11 * fx;

    return h0 * (1 - fz) + h1 * fz;
}

Vec3 Heightmap::GetNormal(int x, int z)
{

    float hL = GetHeight(x - 1, z);
    float hR = GetHeight(x + 1, z);
    float hD = GetHeight(x, z - 1);
    float hU = GetHeight(x, z + 1);
    Vec3 normal = Vec3(hL - hR, 2.0f, hD - hU);
    normal.normalize();
    return normal;
}
bool Heightmap::Load(const char *filename, int bitDepth)
{
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32)
    {
        Utils::LogError("Unsupported bit depth: %d", bitDepth);
        return false;
    }

    SDL_RWops *file = SDL_RWFromFile(filename, "rb");
    if (!file)
    {
        Utils::LogError("Failed to open file: %s", filename);
        return false;
    }
    size_t fileSize = SDL_RWsize(file);
    size_t pixelSize = bitDepth / 8;

    if (fileSize % pixelSize != 0)
    {
        SDL_RWclose(file);
        Utils::LogError("Invalid file size: %s", filename);
        return false;
    }

    size_t numPixels = fileSize / pixelSize;
    width = static_cast<int>(std::sqrt(numPixels));
    height = width;

    if (width * height != static_cast<int>(numPixels))
    {
        SDL_RWclose(file);
        Utils::LogError("Invalid file size: %s", filename);
        return false;
    }

    if (heightData)
    {
        delete[] heightData;
    }

    std::vector<u8> buffer(fileSize);
    SDL_RWread(file, buffer.data(), fileSize, 1);
    SDL_RWclose(file);

    heightData = new float[numPixels];

    for (size_t i = 0; i < numPixels; ++i)
    {
        uint32_t value = readRawValue(buffer.data() + i * pixelSize, bitDepth);
        heightData[i] = (value / static_cast<float>((1 << bitDepth) - 1)) * maxHeight;
    }

    Utils::LogInfo("Heightmap loaded: %s [%dx%d, %d bits]", filename, width, height, bitDepth);
    return true;
}

bool Heightmap::Save(const char *filename, int bitDepth)
{
    if (bitDepth != 16 && bitDepth != 24 && bitDepth != 32)
    {
        Utils::LogError("Unsupported bit depth: %d", bitDepth);
        return false;
    }
    SDL_RWops *file = SDL_RWFromFile(filename, "wb");
    if (!file)
    {
        Utils::LogError("Failed to save file: %s", filename);
        return false;
    }

    size_t bufferSize = (bitDepth / 8) * width * height;
    std::vector<uint8_t> buffer(bufferSize);

    for (size_t i = 0; i < (size_t)(width * height); ++i)
    {
        writeRawValue(buffer.data() + i * (bitDepth / 8), bitDepth, heightData[i]);
    }

    SDL_RWwrite(file, buffer.data(), buffer.size(), 1);
    SDL_RWclose(file);
    return true;
}

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f),
      rotation(Quat(0.0f, 0.0f, 0.0f, 1.0f)),
      scale(1.0f, 1.0f, 1.0f),
      localMatrix(Mat4::Identity()),
      worldMatrix(Mat4::Identity()),
      parent(nullptr),
      isDirty(true) {}

void Transform::SetPosition(const Vec3 &pos, bool global)
{
    if (global && parent)
    {
        Vec3 parentWorldPos;
        parent->GetWorldMatrix().transformPoint(parentWorldPos);
        position = pos - parentWorldPos; // Calcula posição relativa
    }
    else
    {
        position = pos;
    }
    MarkDirty();
}

void Transform::SetRotation(const Quat &rot, bool global)
{
    if (global && parent)
    {
        Quat parentRotation = parent->GetRotation(true);
        rotation = parentRotation.inverse() * rot;
    }
    else
    {
        rotation = rot;
    }
    MarkDirty();
}

void Transform::SetScale(const Vec3 &scl, bool global)
{
    if (global && parent)
    {
        Vec3 parentScale = parent->GetScale(true);
        scale = Vec3(scl.x / parentScale.x, scl.y / parentScale.y, scl.z / parentScale.z);
    }
    else
    {
        scale = scl;
    }
    MarkDirty();
}

Vec3 Transform::GetPosition(bool global) const
{
    if (global && parent)
    {
        Vec3 worldPosition = position;
        parent->GetWorldMatrix().transformPoint(worldPosition);
        return worldPosition;
    }
    return position;
}

Quat Transform::GetRotation(bool global) const
{
    if (global && parent)
    {
        return parent->GetRotation(true) * rotation;
    }
    return rotation;
}

Vec3 Transform::GetScale(bool global) const
{
    if (global && parent)
    {
        Vec3 parentScale = parent->GetScale(true);
        return Vec3(scale.x * parentScale.x, scale.y * parentScale.y, scale.z * parentScale.z);
    }
    return scale;
}

void Transform::Move(const Vec3 &delta, bool global)
{
    if (global)
    {
        SetPosition(GetPosition(true) + delta, true);
    }
    else
    {
        SetPosition(position + delta, false);
    }
}

void Transform::Turn(const Quat &deltaRotation, bool global)
{
    if (global)
    {
        SetRotation(deltaRotation * GetRotation(true), true);
    }
    else
    {
        SetRotation(rotation * deltaRotation, false);
    }
}

const Mat4 &Transform::GetLocalMatrix()
{
    if (isDirty)
    {
        localMatrix = Mat4::Identity();
        localMatrix = Mat4::Translate(localMatrix, position);
        localMatrix = rotation.toMat4() * localMatrix;
        localMatrix = Mat4::Scale(localMatrix, scale);
        isDirty = false;
    }
    return localMatrix;
}

const Mat4 &Transform::GetWorldMatrix()
{
    if (isDirty || (parent && parent->isDirty))
    {
        if (parent)
        {
            worldMatrix = parent->GetWorldMatrix() * GetLocalMatrix();
        }
        else
        {
            worldMatrix = GetLocalMatrix();
        }
        isDirty = false;
    }
    return worldMatrix;
}

void Transform::SetParent(Transform *newParent)
{
    if (parent == newParent)
        return;

    parent = newParent;

    MarkDirty();
}

void Transform::MarkDirty()
{
    isDirty = true;
    if (parent)
        parent->MarkDirty();
}

//***************************************************************************************************************
// Node3D
//***************************************************************************************************************

Node3D::Node3D(const Vec3 &position, const Vec3 &rotation, const Vec3 &scale)
    : RelativeTranslation(position), RelativeRotation(rotation), RelativeScale(scale)
{
    Name = "Node3D";
    UpdateAbsolutePosition();

}

Node3D::~Node3D()
{
}

void Node3D::SetParent(Node3D *parent)
{
    this->Parent = parent;
    UpdateAbsolutePosition();
}

void Node3D::UpdateAbsolutePosition()
{
    if (Parent)
    {
        AbsoluteTransformation = Parent->GetAbsoluteTransformation() * GetRelativeTransformation();
    }
    else
    {
        AbsoluteTransformation = GetRelativeTransformation();
    }
}

Mat4 Node3D::GetRelativeTransformation() const
{
    Mat4 mat;
    mat.setRotationDegrees(RelativeRotation);
    mat.setTranslation(RelativeTranslation);
    if (RelativeScale != Vec3(1.0f, 1.0f, 1.0f))
    {
        Mat4 scaleMat;
        scaleMat.setScale(RelativeScale);
        mat *= scaleMat;
    }
    return mat; 
}

Vec3 Node3D::GetAbsolutePosition()
{
    return AbsoluteTransformation.getTranslation();
}

void Node3D::Render()
{
}

void Node3D::Update(u32 time)
{
    (void)time;
}


//***************************************************************************************************************
// Camera
//***************************************************************************************************************


Camera::Camera(const Vec3 &position, const Vec3 &lookat)
    : Node3D(position), Target(lookat), Up(Vec3(0.0f, 1.0f, 0.0f)), ZNear(1.0f), ZFar(3000.0f)
{
    IsOrthogonal = false;
    FOV = Pi / 2.5f;
    Aspect = Driver::Instance().GetWidth() / (float) Driver::Instance().GetHeight();
 	RecalculateProjectionMatrix();   
}

void Camera::SetProjectionMatrix(const Mat4 &mat, bool isOrtho)
{
    IsOrthogonal = isOrtho;
    Driver::Instance().SetTransform(PROJECTION_MATRIX, mat);
}

const Mat4 &Camera::GetProjectionMatrix() const
{
    return Driver::Instance().GetTransform(PROJECTION_MATRIX);
}

void Camera::SetViewMatrix(const Mat4 &mat)
{
    Driver::Instance().SetTransform(VIEW_MATRIX, mat);
}

const Mat4 &Camera::GetViewMatrix() const
{
    return Driver::Instance().GetTransform(VIEW_MATRIX);
}

void Camera::Render()
{
    Vec3 pos = GetAbsolutePosition();
    Vec3 tgtv = Target - pos;
    tgtv.normalize();
    Vec3 upv = Up;
    upv.normalize();

    float dp = tgtv.dot(upv);

    if (IsEqual(dp,1.f)) 
    {
       Up.x += 0.5f;
    }
    
    Mat4 view = Mat4::LookAt(pos, Target, Up);
    Driver::Instance().SetTransform(VIEW_MATRIX, view);
    
}

void Camera::RecalculateProjectionMatrix()
{
    Driver::Instance().SetTransform(PROJECTION_MATRIX, Mat4::Perspective(FOV, Aspect, ZNear, ZFar));
}
