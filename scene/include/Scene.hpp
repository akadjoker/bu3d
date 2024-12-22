#pragma once
#include "Config.hpp"
#include "Node.hpp"
#include "Node3D.hpp"
#include "Camera3D.hpp"
#include "Entity.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Model.hpp"
#include "Entity.hpp"

const int SHADOW_MAP_CASCADE_COUNT = 4;

class   Scene
{
public:

    Scene();
    ~Scene();

    void Init();

    void Render();

    void Update(float elapsed);


    void Release();

    Model *AddModel();
    Model *AddModel(Mesh* mesh);
    Model *LoadModel(const String& fileName,const VertexFormat& vertexFormat);

    Camera3D *AddCamera();
    Camera3D *GetMainCamera() const;
    void SetCamera(Camera3D *camera);

    static Scene& Instance();
    static Scene* InstancePtr();

private:
    struct Cascade
    {
        float splitDepth;
        Mat4 viewProjMatrix;
    };
    CascadeShadow depthBuffer;
    TextureBuffer renderTexture;
    RenderQuad  quadRender;
    Shader m_modelShader;
    Shader m_quadShader;
    Shader m_depthShader;
    Shader m_debugShader;
    Cascade cascades[SHADOW_MAP_CASCADE_COUNT];
    Vector<Model*> m_statics;
    Vector<Camera3D*> m_cameras;
    Vector<Entity*> m_entities;
    Camera3D* m_mainCamera;
    Mat4 projectionMatrix;
    Mat4 viewMatrix;
    Mat4 viewShaderMatrix;
    Vec3 lightPosition;
    float updateInterval;
    float updateTime;
    bool firstRender;

    static Scene* m_singleton;
    bool LoadModelShader();
    bool LoadDepthShader();
    bool LoadPostShader();
    void updateCascades(float nearClip, float farClip, const Vec3 &lightPos);
};