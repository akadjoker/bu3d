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

    LoadModelShader();
    LoadDepthShader();
    LoadPostShader();

    IntRect viewport = Driver::Instance().GetViewport();

    depthBuffer.Init(SHADOW_MAP_CASCADE_COUNT,SHADOW_WIDTH, SHADOW_HEIGHT);
    quadRender.Init(viewport.width, viewport.height);
    renderTexture.Init(viewport.width, viewport.height);


}
void Scene::Release()
{
    Logger::Instance().Info("Release scene");
    m_quadShader.Release();
    m_debugShader.Release();
    depthBuffer.Release();
    m_depthShader.Release();
    m_modelShader.Release();
    renderTexture.Release();
}

void Scene::Render()
{
}

void Scene::Update(float elapsed)
{
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

        void main() 
        {
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

        struct CascadeShadow 
        {
            mat4 projViewMatrix;
            float splitDistance;
        };

      

        uniform sampler2D diffuseTexture;
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

        float CalculateShadow(vec4 worldPosition, int idx) 
        {
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
            shadow /= 10.0;

            return shadow;
        }

        void main() 
        {
            vec3 color = texture(diffuseTexture, TexCoords).rgb;
            vec3 viewDir = normalize(viewPos - FragPos);

            int cascadeIndex = 0;
            for (int i = 0; i < NUM_CASCADES - 1; i++)
            {
                if (outViewPosition.z < cascadeshadows[i].splitDistance)
                {
                    cascadeIndex = i + 1;
                }
            }



            // luz para shadow "sol"
            vec3 normal = normalize(Normal);
            vec3 lightColor = vec3(0.4);
            vec3 ambient = 0.2 * color;
            float diff = max(dot(lightDir, normal), 0.0);
            vec3 diffuse = diff * lightColor;

            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = 0.0;
            vec3 halfwayDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
            vec3 specular = spec * lightColor;

            float shadow = CalculateShadow(outWorldPosition, cascadeIndex);

            vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
      

            float fogDistance = length(viewPos - FragPos);
            

            vec3 fogColor    = vec3(0.5, 0.5, 0.5);
            float fogDensity = 0.008;
            float fogFactor  = 1.0 / pow(E, fogDistance * fogDensity);

            fogFactor = clamp(fogFactor, 0.0, 1.0);

            vec3 fogColorFinal = fogFactor * lighting + (1.0 - fogFactor) * fogColor;

            FragColor = vec4(fogColorFinal, 1.0);

            
        }

    );
    if (!m_modelShader.Create(vShader, fShader))
    {
        DEBUG_BREAK_IF(true);
    }
    m_modelShader.LoadDefaults();
    m_modelShader.SetInt("diffuseTexture", 0);
    m_modelShader.SetInt("isStatic", 1);
    m_modelShader.SetFloat("viewPos", 0.0f, 0.0f, 0.0f);

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
            float LinearizeDepth(float depth) 
            {
                float z = depth * 2.0 - 1.0; // Back to NDC
                return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
            }

            void main() 
            {
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

        void main() 
        {
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

        void main() 
        {
            // FragColor = texture(textureMap, TexCoords);
            FragColor = Bloom(Samples, Quality);
        } 
        
        vec4 Bloom(float samples, float quality) 
        {
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
        } 
        vec4 Blur() 
        {
            vec3 texelColor = texture(textureMap, TexCoords).rgb * weight[0];
            vec2 size = textureSize(textureMap, 0);

            for (int i = 1; i < 3; i++)
            {
                texelColor += texture(textureMap, TexCoords + vec2(offset[i]) / size.x, 0.0).rgb * weight[i];
                texelColor += texture(textureMap, TexCoords - vec2(offset[i]) / size.x, 0.0).rgb * weight[i];
            }

            return vec4(texelColor, 1.0);
        }
        );

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
