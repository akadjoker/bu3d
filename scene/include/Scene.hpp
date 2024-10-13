#pragma once
#include "Config.hpp"
#include "Node.hpp"
#include "Node3D.hpp"
#include "Camera3D.hpp"
#include "Entity.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"

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
    static Scene* m_singleton;
    bool LoadModelShader();
    bool LoadDepthShader();
    bool LoadPostShader();
};