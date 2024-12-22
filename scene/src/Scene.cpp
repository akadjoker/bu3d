#include "Scene.hpp"
#include "Driver.hpp"

float lightLinear = 0.0334f;
float lightQuadratic = 0.0510f;
float lightIntensity = 1.1679f;
bool useBloom = true;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

Scene *Scene::m_singleton = 0x0;

Scene &Scene::Instance()
{
    DEBUG_BREAK_IF(!m_singleton);
    return (*m_singleton);
}
Scene *Scene::InstancePtr()
{
    return m_singleton;
}

Scene::Scene()
{
    m_singleton = this;
}

Scene::~Scene()
{
    m_singleton = 0x0;
}

void Scene::Init()
{

    Logger::Instance().Info("Init scene");

    updateInterval = 2.0f;
    updateTime = 0.0f;

    LoadModelShader();
    LoadDepthShader();
    LoadPostShader();

    IntRect viewport = Driver::Instance().GetViewport();
    m_mainCamera = AddCamera();

    depthBuffer.Init(SHADOW_MAP_CASCADE_COUNT, SHADOW_WIDTH, SHADOW_HEIGHT);
    quadRender.Init(viewport.width, viewport.height);
    renderTexture.Init(viewport.width, viewport.height);
    lightPosition.set(0.5f, 4.0f, 7.5f);
    updateCascades(m_mainCamera->getNear(), m_mainCamera->getFar(), lightPosition);
    firstRender = true;
}
void Scene::Release()
{
    Logger::Instance().Info("Release scene");
    for (u32 i = 0; i < m_statics.size(); i++)
    {
        m_statics[i]->Release();
        delete m_statics[i];
    }
    m_statics.clear();
    for (u32 i = 0; i < m_entities.size(); i++)
    {
        m_entities[i]->Release();
        delete m_entities[i];
    }
    m_entities.clear();
    for (u32 i = 0; i < m_cameras.size(); i++)
    {
        m_cameras[i]->Release();
        delete m_cameras[i];
    }
    m_cameras.clear();
    m_quadShader.Release();
    m_debugShader.Release();
    depthBuffer.Release();
    m_depthShader.Release();
    m_modelShader.Release();
    renderTexture.Release();
}

Model *Scene::AddModel()
{
    Model *model = new Model();
    m_statics.push_back(model);
    return model;
}

Model *Scene::AddModel(Mesh *mesh)
{
    Model *model = new Model();
    model->AddMesh(mesh);
    m_statics.push_back(model);
    return model;
}

Model *Scene::LoadModel(const String &fileName, const VertexFormat &vertexFormat)
{
    Model *model = new Model();
    model->Load(fileName, vertexFormat);
    m_statics.push_back(model);
    return model;
}

Camera3D *Scene::AddCamera()
{
    Camera3D *camera = new Camera3D();
    m_cameras.push_back(camera);
    return camera;
}

Camera3D *Scene::GetMainCamera() const
{
    return m_mainCamera;
}

void Scene::SetCamera(Camera3D *camera)
{
    m_mainCamera = camera;
}

Vec3 lerp(const Vec3 &a, const Vec3 &b, float t)
{
    return Vec3::Lerp(a, b, t);
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

Vec3 stabilizeShadowFrustum(Vec3 frustumCenter, float radius, float shadowMapResolution)
{
    // Calcula o tamanho de cada texel em unidades do mundo
    float worldUnitsPerTexel = (radius * 2.0f) / shadowMapResolution;

    // Arredonda o centro do frustum para a grade mais próxima
    frustumCenter.x = floor(frustumCenter.x / worldUnitsPerTexel) * worldUnitsPerTexel;
    frustumCenter.y = floor(frustumCenter.y / worldUnitsPerTexel) * worldUnitsPerTexel;

    return frustumCenter;
}
void Scene::updateCascades(float nearClip, float farClip, const Vec3 &lightPos)
{
    float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
    float cascadeSplitLambda = 0.95f;

    float clipRange = farClip - nearClip;

    float minZ = nearClip;
    float maxZ = nearClip + clipRange;

    float range = maxZ - minZ;
    float ratio = maxZ / minZ;

    for (u32 i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
    {
        float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
        float log = minZ * Pow(ratio, p);
        float uniform = minZ + range * p;
        float d = cascadeSplitLambda * (log - uniform) + uniform;
        cascadeSplits[i] = (d - nearClip) / clipRange;
    }

    // Calculate orthographic projection matrix for each cascade
    float lastSplitDist = 0.0;
    for (u32 i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
    {
        float splitDist = cascadeSplits[i];

        Vec3 frustumCorners[8] =
            {
                Vec3(-1.0f, 1.0f, 0.0f),
                Vec3(1.0f, 1.0f, 0.0f),
                Vec3(1.0f, -1.0f, 0.0f),
                Vec3(-1.0f, -1.0f, 0.0f),
                Vec3(-1.0f, 1.0f, 1.0f),
                Vec3(1.0f, 1.0f, 1.0f),
                Vec3(1.0f, -1.0f, 1.0f),
                Vec3(-1.0f, -1.0f, 1.0f),
            };

        // Project frustum corners into world space

        Mat4 invCam = Mat4::Inverse(projectionMatrix * viewShaderMatrix);
        for (u32 j = 0; j < 8; j++)
        {
            Vec4 invCorner = invCam * Vec4(frustumCorners[j], 1.0f);
            Vec4 div = invCorner / invCorner.w;
            frustumCorners[j] = Vec3(div.x, div.y, div.z);
        }

        for (u32 j = 0; j < 4; j++)
        {
            Vec3 dist = frustumCorners[j + 4] - frustumCorners[j];
            frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
            frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
        }

        // Get frustum center
        Vec3 frustumCenter = Vec3(0.0f);
        for (u32 j = 0; j < 8; j++)
        {
            frustumCenter += frustumCorners[j];
        }
        frustumCenter /= 8.0f;

        // static Vec3 previousFrustumCenter = frustumCenter;
        // float smoothingFactor = 0.99f;
        // frustumCenter = lerp(previousFrustumCenter, frustumCenter, smoothingFactor);
        // previousFrustumCenter = frustumCenter;


        float radius = 0.0f;
        for (u32 j = 0; j < 8; j++)
        {
            float distance = Vec3::Length(frustumCorners[j] - frustumCenter);
            radius = Max(radius, distance);
        }
        radius = Ceil(radius * 16.0f) / 16.0f;

        // static float previousRadius = radius;
        // radius = lerp(previousRadius, radius, smoothingFactor);
        // previousRadius = radius;

        frustumCenter = stabilizeShadowFrustum(frustumCenter, radius, SHADOW_WIDTH);

        Vec3 maxExtents = Vec3(radius);
        Vec3 minExtents = -maxExtents;

        Vec3 lightDir = Vec3::Normalize(-lightPos);
        Mat4 lightViewMatrix = Mat4::LookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, Vec3(0.0f, 1.0f, 0.0f));
        Mat4 lightOrthoMatrix = Mat4::Ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

        // Store split distance and matrix in cascade
        cascades[i].splitDepth = (nearClip + splitDist * clipRange) * -1.0f;
        cascades[i].viewProjMatrix = lightOrthoMatrix * lightViewMatrix;

        lastSplitDist = cascadeSplits[i];
    }
}

void Scene::Render()
{

    Vec3 eye = m_mainCamera->getLocalPosition();

    projectionMatrix = m_mainCamera->getProjection();
    viewMatrix = m_mainCamera->getView();
    updateCascades(m_mainCamera->getNear(), m_mainCamera->getFar(), lightPosition);
    if (firstRender)
    {
        firstRender = false;
        viewShaderMatrix = m_mainCamera->getView();
    }   

    if (updateTime > updateInterval)
    {
        updateTime = 0.0f;
        viewShaderMatrix = m_mainCamera->getView();
    }

    Mat4 lightProjection;
    Mat4 lightView;
    Mat4 lightSpaceMatrix;

    Driver::Instance().SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    Driver::Instance().SetBlend(true);
    Driver::Instance().SetBlendMode(BlendMode::BLEND);
    Driver::Instance().SetDepthTest(true);
    Driver::Instance().SetDepthClamp(true);
    Driver::Instance().SetCullFace(true);
    Driver::Instance().SetCullFaceMode(GL_FRONT);

    m_depthShader.Use();

    depthBuffer.Begin();

    for (u32 j = 0; j < SHADOW_MAP_CASCADE_COUNT; j++)
    {

        depthBuffer.Set(j);
        lightSpaceMatrix = cascades[j].viewProjMatrix;
        m_depthShader.SetMatrix4("lightSpaceMatrix", &lightSpaceMatrix.c[0][0]);
        m_depthShader.SetInt("isStatic", 1);
        for (u32 i = 0; i < m_statics.size(); i++)
        {
            Model *model = m_statics[i];
            if (!model->GetVisible() || !model->GetCastShadows())
                continue;
            const Mat4 transform = model->getWorldTransform();
            m_depthShader.SetMatrix4("model", &transform.c[0][0]);
            model->RenderNoMaterial(&m_modelShader);
        }

        m_depthShader.SetInt("isStatic", 0);

        for (u32 i = 0; i < m_entities.size(); i++)
        {
            Entity *entity = m_entities[i];

            if (!entity->GetVisible() || !entity->GetCastShadows())
                continue;
            const Mat4 model = entity->getWorldTransform();
            m_depthShader.SetMatrix4("model", &model.c[0][0]);
            for (u32 b = 0; b < entity->GetTotalBones(); b++)
            {
                const Mat4 &m = entity->GetBone(b);
                m_depthShader.SetMatrix4(FormatText("Joints[%d]", b), m.c[0]);
            }
            entity->RenderNoMaterial(&m_depthShader);
        }
    }

    depthBuffer.End();

    IntRect viewport = Driver::Instance().GetViewport();
    Driver::Instance().SetViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    Driver::Instance().Clear();
    Driver::Instance().SetCullFace(true);
    Driver::Instance().SetCullFaceMode(GL_BACK);

    lightPosition = Vec3::Normalize(Vec3(-20.0f, 50.0f, 20.0f));

    m_modelShader.Use();
    m_modelShader.SetMatrix4("view", &viewMatrix.c[0][0]);
    m_modelShader.SetMatrix4("projection", &projectionMatrix.c[0][0]);
    m_modelShader.SetFloat("viewPos", eye.x, eye.y, eye.z);
    m_modelShader.SetFloat("lightDir", lightPosition.x, lightPosition.y, lightPosition.z);
    m_modelShader.SetFloat("farPlane", m_mainCamera->getFar());

    depthBuffer.BindTextures(1);
    for (u32 i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
    {
        m_modelShader.SetMatrix4(FormatText("cascadeshadows[%d].projViewMatrix", i), &cascades[i].viewProjMatrix.c[0][0]);
        m_modelShader.SetFloat(FormatText("cascadeshadows[%d].splitDistance", i), cascades[i].splitDepth);
    }

    // if (useBloom)
    //    renderTexture.Begin();

    m_modelShader.SetInt("isStatic", 1);
    for (u32 i = 0; i < m_statics.size(); i++)
    {
        Model *model = m_statics[i];
        if (!model->GetVisible())
            continue;

        const Mat4 transform = m_statics[i]->getWorldTransform();
        m_modelShader.SetMatrix4("model", &transform.c[0][0]);
        model->Render(&m_modelShader);
    }

    m_modelShader.SetInt("isStatic", 0);
    for (u32 i = 0; i < m_entities.size(); i++)
    {
        const Mat4 model = m_entities[i]->getWorldTransform();
        m_modelShader.SetMatrix4("model", &model.c[0][0]);
        for (u32 b = 0; b < m_entities[i]->GetTotalBones(); b++)
        {
            const Mat4 &m = m_entities[i]->GetBone(b);
            m_modelShader.SetMatrix4(FormatText("Joints[%d]", b), m.c[0]);
        }
        m_entities[i]->Render(&m_modelShader);
    }

    Driver::Instance().SetTextureId(1, 0);
    Driver::Instance().SetTextureId(2, 0);
    Driver::Instance().SetTextureId(3, 0);
    Driver::Instance().SetTextureId(4, 0);

    Driver::Instance().SetBlendMode(BlendMode::BLEND);
    Driver::Instance().SetViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    //   Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    ////   Driver::Instance().Clear();

    //  renderTexture.End();

    //  renderTexture.BindTexture(0);
    //  m_quadShader.Use();
    //  Driver::Instance().SetViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    //  quadRender.Render();
}

void Scene::Update(float elapsed)
{
    updateTime += elapsed;
 
}

bool Scene::LoadModelShader()
{
    const char *vShader = GLSL(
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoords;
        layout(location = 2) in vec3 aNormal;

        layout(location = 5) in ivec4 aBones;
        layout(location = 6) in vec4 aWeights;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;

        out vec3 outViewPosition;
        out vec4 outWorldPosition;

        uniform vec3 viewPos;

        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;

        uniform int isStatic;

        const int MAX_BONES = 80;
        const int MAX_BONE_INFLUENCE = 4;
        uniform mat4 Joints[MAX_BONES];

        void main() {
            if (isStatic == 1)
            {
                vec4 initPos = vec4(aPos, 1.0);

                FragPos = vec3(model * initPos);
                Normal = transpose(inverse(mat3(model))) * aNormal;

                mat4 modelViewMatrix = view * model;
                vec4 mvPosition = modelViewMatrix * initPos;
                outViewPosition = mvPosition.xyz;
                outWorldPosition = model * initPos;

                gl_Position = projection * view * model * initPos;
            }
            else
            {
                vec4 totalPosition = vec4(0.0f);
                vec4 totalNormal = vec4(0.0f);
                for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
                {
                    if (aBones[i] == -1)
                        continue;
                    if (aBones[i] >= MAX_BONES)
                    {
                        totalPosition = vec4(aPos, 1.0f);
                        break;
                    }
                    vec4 localPosition = Joints[aBones[i]] * vec4(aPos, 1.0f);
                    totalPosition += localPosition * aWeights[i];
                    vec4 localNormal = Joints[aBones[i]] * vec4(aNormal, 0.0f);
                    totalNormal += localNormal * aWeights[i];
                }

                mat4 viewModel = view * model;

                Normal = mat3(transpose(inverse(viewModel))) * totalNormal.xyz;

                FragPos = (model * totalPosition).xyz;

                mat4 modelViewMatrix = view * model;
                vec4 mvPosition = modelViewMatrix * totalPosition;
                outViewPosition = mvPosition.xyz;
                outWorldPosition = model * totalPosition;

                gl_Position = projection * viewModel * totalPosition;
            }

            TexCoords = aTexCoords;
        });

    const char *fShader = GLSL(

        const int NUM_CASCADES = 4;

        out vec4 FragColor;

        in vec3 FragPos;
        in vec2 TexCoords;
        in vec3 Normal;

        in vec3 outViewPosition;
        in vec4 outWorldPosition;

        struct CascadeShadow {
            mat4 projViewMatrix;
            float splitDistance;
        };

        uniform sampler2D texture0;
        uniform CascadeShadow cascadeshadows[NUM_CASCADES];
        uniform sampler2D shadowMap[NUM_CASCADES];

        uniform vec3 lightDir;
        uniform float farPlane;
        uniform vec3 viewPos;

        const float constant = 1.0;
        const float linear = 0.09;
        const float quadratic = 0.032;
        const float shininess = 32.0;
        const float E = 2.71828;

        vec2 poissonDisk[16] = vec2[](
            vec2(-0.94201624, -0.39906216),
            vec2(0.94558609, -0.76890725),
            vec2(-0.094184101, -0.92938870),
            vec2(0.34495938, 0.29387760),
            vec2(-0.91588581, 0.45771432),
            vec2(-0.81544232, -0.87912464),
            vec2(-0.38277543, 0.27676845),
            vec2(0.97484398, 0.75648379),
            vec2(0.44323325, -0.97511554),
            vec2(0.53742981, -0.47373420),
            vec2(-0.26496911, -0.41893023),
            vec2(0.79197514, 0.19090188),
            vec2(-0.24188840, 0.99706507),
            vec2(-0.81409955, 0.91437590),
            vec2(0.19984126, 0.78641367),
            vec2(0.14383161, -0.14100790));

        float CalculateShadowPoissonDisk(vec4 worldPosition, int idx);
        float CalculateShadowPCF(vec4 worldPosition, int idx);

        void main() {
            vec3 color = texture(texture0, TexCoords).rgb;

            int cascadeIndex = 0;
            for (int i = 0; i < NUM_CASCADES - 1; i++)
            {
                if (outViewPosition.z < cascadeshadows[i].splitDistance)
                {
                    cascadeIndex = i + 1;
                }
            }
            cascadeIndex = clamp(cascadeIndex, 0, NUM_CASCADES - 1);

            vec3 lightColor = vec3(0.4);
            vec3 normal = normalize(Normal);

            // luz para shadow "sol"
            vec3 lightDirection = normalize(lightDir);
            float diff = max(dot(normal, lightDirection), 0.0);
            vec3 diffuse = diff * lightColor;

            // Cálculo da luz especular (reflexão)
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDirection, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // shininess = 32
            vec3 specular = spec * lightColor;

            // Componente ambiente (luz ambiente constante)
            vec3 ambient = 0.4 * lightColor;

            float shadow = CalculateShadowPoissonDisk(outWorldPosition, cascadeIndex);

            vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

            // FragColor = vec4(lighting, 1.0);

            float fogDistance = length(viewPos - FragPos);

            vec3 fogColor = vec3(0.5, 0.5, 0.5);
            float fogDensity = 0.0008;
            float fogFactor = 1.0 / pow(E, fogDistance * fogDensity);

            fogFactor = clamp(fogFactor, 0.0, 1.0);

            vec3 fogColorFinal = fogFactor * lighting + (1.0 - fogFactor) * fogColor;

            FragColor = vec4(fogColorFinal, 1.0);
        }

        float CalculateShadowPCF(vec4 worldPosition, int idx) {
            vec2 texelSize = 1.0 / textureSize(shadowMap[idx], 0);
            vec4 shadowMapPosition = cascadeshadows[idx].projViewMatrix * worldPosition;
            vec4 projCoords = (shadowMapPosition / shadowMapPosition.w) * 0.5 + 0.5;

            float currentDepth = projCoords.z;

            // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
            if (currentDepth > 1.0)
            {
                return 0.0;
            }

            vec3 normal = normalize(Normal);
            float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
            const float biasModifier = 0.5f;

            if (idx == NUM_CASCADES)
            {
                bias *= 1 / (farPlane * biasModifier);
            }
            else
            {
                bias *= 1 / (cascadeshadows[idx].splitDistance * biasModifier);
            }

            float shadow = 0.0;
            for (int x = -1; x <= 1; ++x)
            {
                for (int y = -1; y <= 1; ++y)
                {
                    vec2 texel = projCoords.xy + vec2(x, y) * texelSize;
                    float pcfDepth = texture(shadowMap[idx], texel).r;
                    shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;

            return shadow;
        }

        float dynamicBias(vec3 normal, vec3 lightDir, float cascadeSplitDistance, float baseBias) {
            // Calcula o ângulo entre a direção da luz e a superfície
            float bias = max(baseBias * (1.0 - dot(normal, lightDir)), baseBias);

            // Reduz o bias com base na distância da cascata (splitDistance)
            bias *= 1.0 / (cascadeSplitDistance * 0.5);

            return bias;
        }

        float CalculateShadowPoissonDisk(vec4 worldPosition, int idx) {
            vec2 texelSize;
            vec4 shadowMapPosition;
            float currentDepth;

            if (idx == 0)
            {
                texelSize = 1.0 / textureSize(shadowMap[0], 0);
                shadowMapPosition = cascadeshadows[0].projViewMatrix * worldPosition;
            }
            else if (idx == 1)
            {
                texelSize = 1.0 / textureSize(shadowMap[1], 0);
                shadowMapPosition = cascadeshadows[1].projViewMatrix * worldPosition;
            }
            else if (idx == 2)
            {
                texelSize = 1.0 / textureSize(shadowMap[2], 0);
                shadowMapPosition = cascadeshadows[2].projViewMatrix * worldPosition;
            }
            else
            {
                texelSize = 1.0 / textureSize(shadowMap[3], 0);
                shadowMapPosition = cascadeshadows[3].projViewMatrix * worldPosition;
            }

            vec4 projCoords = (shadowMapPosition / shadowMapPosition.w) * 0.5 + 0.5;
            currentDepth = projCoords.z;

            if (currentDepth > 1.0)
                return 0.0;

            vec3 normal = normalize(Normal);
            float bias = dynamicBias(normal, lightDir, cascadeshadows[idx].splitDistance, 0.005);
            float distance = length(viewPos - worldPosition.xyz);

            float shadow = 0.0;

            int samples = 16; // Podes ajustar o número de amostras aqui

            if (distance > farPlane * 0.5)
                samples = 8; // Reduz para 8 amostras em distâncias maiores
            if (distance > farPlane * 0.75)
                samples = 4; // Reduz ainda mais para 4 amostras

            for (int i = 0; i < samples; ++i)
            {
                vec2 offset = poissonDisk[i] * texelSize;
                float pcfDepth = texture(shadowMap[idx], projCoords.xy + offset).r;
                shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
            }

            shadow /= float(samples);

            return shadow;
        }

    );
    if (!m_modelShader.Create(vShader, fShader))
    {
        DEBUG_BREAK_IF(true);
    }
    m_modelShader.LoadDefaults();
    m_modelShader.SetInt("texture0", 0);
    m_modelShader.SetInt("isStatic", 1);
    m_modelShader.SetFloat("viewPos", 0.0f, 0.0f, 0.0f);
    for (u32 i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
    {
        const char *value = FormatText("shadowMap[%d]", i);
        m_modelShader.SetInt(value, i + 1);
    }

    return false;
}

bool Scene::LoadDepthShader()
{

    {

        const char *vertexShaderSource = GLSL(
            layout(location = 0) in vec3 aPos;
            layout(location = 5) in ivec4 aBones;
            layout(location = 6) in vec4 aWeights;

            uniform mat4 model;
            uniform int isStatic;

            const int MAX_BONES = 80;
            const int MAX_BONE_INFLUENCE = 4;
            uniform mat4 Joints[MAX_BONES];

            uniform mat4 lightSpaceMatrix;

            void main() {
                if (isStatic == 1)
                {
                    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
                }
                else
                {
                    vec4 totalPosition = vec4(0.0f);
                    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
                    {
                        if (aBones[i] == -1)
                            continue;
                        if (aBones[i] >= MAX_BONES)
                        {
                            totalPosition = vec4(aPos, 1.0f);
                            break;
                        }
                        vec4 localPosition = Joints[aBones[i]] * vec4(aPos, 1.0f);
                        totalPosition += localPosition * aWeights[i];
                    }

                    gl_Position = lightSpaceMatrix * model * totalPosition;
                }
            }

        );

        const char *fragmentShaderSource = GLSL(

            void main(){

            });

        LogWarning("Loading Depth Shader");

        if (!m_depthShader.Create(vertexShaderSource, fragmentShaderSource))
        {
            DEBUG_BREAK_IF(true);
            return false;
        }
        m_depthShader.LoadDefaults();
        m_depthShader.SetInt("isStatic", 1);
    }

    {
        LogWarning("Loading Debug Depth Shader");
        const char *vertexShaderSource = GLSL(
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec2 aTexCoords;

            out vec2 TexCoords;

            void main() {
                TexCoords = aTexCoords;
                gl_Position = vec4(aPos, 1.0);
            });

        const char *fragmentShaderSource = GLSL(
            out vec4 FragColor;

            in vec2 TexCoords;

            uniform sampler2D depthMap;
            uniform float near_plane;
            uniform float far_plane;

            // required when using a perspective projection matrix
            float LinearizeDepth(float depth) {
                float z = depth * 2.0 - 1.0; // Back to NDC
                return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
            }

            void main() {
                float depthValue = texture(depthMap, TexCoords).r;
                // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
                FragColor = vec4(vec3(depthValue), 1.0); // orthographic
            });

        if (!m_debugShader.Create(vertexShaderSource, fragmentShaderSource))
        {
            DEBUG_BREAK_IF(true);
            return false;
        }
        m_debugShader.LoadDefaults();
        m_debugShader.SetInt("depthMap", 0);
    }

    return true;
}

bool Scene::LoadPostShader()
{

    const char *vertexShaderSource = GLSL(
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoords;

        out vec2 TexCoords;

        void main() {
            TexCoords = aTexCoords;
            gl_Position = vec4(aPos, 1.0);
        });

    const char *fragmentShaderSource = GLSL(
        out vec4 FragColor;

        in vec2 TexCoords;

        uniform sampler2D textureMap;

        // Bloom
        const float Samples = 8.0; // Pixels per axis; higher = bigger glow, worse performance
        const float Quality = 3.0; // Defines size factor: Lower = smaller glow, better quality

        // blur
        float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
        float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

        vec4 Bloom(float samples, float quality);
        vec4 Blur();

        void main() {
            // FragColor = texture(textureMap, TexCoords);
            FragColor = Bloom(Samples, Quality);
        }

        vec4 Bloom(float samples, float quality) {
            vec2 size = textureSize(textureMap, 0);
            vec4 sum = vec4(0);
            vec2 sizeFactor = vec2(1) / size * quality;
            vec4 colDiffuse = vec4(1, 1, 1, 1);

            vec4 source = texture(textureMap, TexCoords);

            const int range = 2; // should be = (samples - 1)/2;

            for (int x = -range; x <= range; x++)
            {
                for (int y = -range; y <= range; y++)
                {
                    sum += texture(textureMap, TexCoords + vec2(x, y) * sizeFactor);
                }
            }

            vec4 color = ((sum / (samples * samples)) + source) * colDiffuse;
            return color;
        } vec4 Blur() {
            vec3 texelColor = texture(textureMap, TexCoords).rgb * weight[0];
            vec2 size = textureSize(textureMap, 0);

            for (int i = 1; i < 3; i++)
            {
                texelColor += texture(textureMap, TexCoords + vec2(offset[i]) / size.x, 0.0).rgb * weight[i];
                texelColor += texture(textureMap, TexCoords - vec2(offset[i]) / size.x, 0.0).rgb * weight[i];
            }

            return vec4(texelColor, 1.0);
        });

    LogWarning("Loading Quad Shader");
    if (!m_quadShader.Create(vertexShaderSource, fragmentShaderSource))
    {
        DEBUG_BREAK_IF(true);
        return false;
    }
    m_quadShader.LoadDefaults();
    m_quadShader.SetInt("textureMap", 0);

    return true;
}
