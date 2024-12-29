#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void PrintMatrix(const Mat4& matrix)
{
    printf("Mat4: \n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", matrix.at(i, j));
        }
        printf("\n");
    }
    printf("\n");
}

void PrintGlmMatrix(const glm::mat4& matrix)
{
    printf("glm: \n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

Mat4 calculateReflectionMatrix(const Vec3& point , const Vec3& normal)
{
    Vec3 N = Vec3::Normalize(normal);
    float D = -N.dot(point);

    Mat4 M = Mat4(1.0f);

// Matriz de reflexão usando equação do plano (Nx*x + Ny*y + Nz*z + D = 0)
    M.at(0,0) = 1.0f - 2.0f * N.x * N.x;
    M.at(0,1) = -2.0f * N.x * N.y;
    M.at(0,2) = -2.0f * N.x * N.z;
    M.at(0,3) = -2.0f * N.x * D;

    M.at(1,0) = -2.0f * N.y * N.x;
    M.at(1,1) = 1.0f - 2.0f * N.y * N.y;
    M.at(1,2) = -2.0f * N.y * N.z;
    M.at(1,3) = -2.0f * N.y * D;

    M.at(2,0) = -2.0f * N.z * N.x;
    M.at(2,1) = -2.0f * N.z * N.y;
    M.at(2,2) = 1.0f - 2.0f * N.z * N.z;
    M.at(2,3) = -2.0f * N.z * D;

    M.at(3,0) = 0.0f;
    M.at(3,1) = 0.0f;
    M.at(3,2) = 0.0f;
    M.at(3,3) = 1.0f;

    return M;
}

Vec4 calculateReflectionPlane(const Vec3& point , const Vec3& normal)
{
    Vec3 N = Vec3::Normalize(normal);
    float D = -N.dot(point);
    Vec4 plane = Vec4(N.x, N.y, N.z, D);
    return plane;
}

Vec3 calculateReflectionVector(const Vec3& incident , const Vec3& normal)
{
    return incident - 2.0f * Vec3::Dot(incident, normal) * normal;
}



bool MirrorMatrix (const Vec3& eyePos, const Vec3& mirrorPos, const Vec3& mirrorNormal, float mirrorSize, Mat4& outMirrorProjMatrix, Mat4& outMirrorViewMatrix)
{
    Vec3 eyeV (mirrorPos-eyePos); 

    float dist =eyeV.length();
    eyeV = Vec3::Normalize(eyeV);

    Vec3 N = Vec3::Normalize (mirrorNormal);

    if (Vec3::Dot(N,eyeV) <= 0.0)   
        return false;                       // mirror looking backguards 

    Vec3 R = calculateReflectionVector (eyeV, N);

    // Position of the 'eye' inside the mirror.
    Vec3 mirrorEyePos = mirrorPos - (R * dist);

    float fovAngleRad = 2.0f * atanf ((mirrorSize/2.0f) / dist);

    // Final matrix
    outMirrorProjMatrix = Mat4::Perspective (fovAngleRad, 1.0f, dist, 100.0f);
    outMirrorViewMatrix = Mat4::LookAt (mirrorEyePos, mirrorPos, Vec3(0.0f,1.0f,0.0f));
    return true;
}


int main()
{

    Device device;
    device.Init("OpenGL Device", 800, 600, true);

    Shader renderShader;
    Shader mirrorShader;

    {

        const char *vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTex0;
        out vec2 TexCoord0;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() 
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord0 = aTex0;
        }
    )";

        const char *fragmentSrc = R"(
        #version 330 core
        
        out vec4 FragColor;

        
        in vec2 TexCoord0;

        uniform sampler2D Texture0;


        void main()
        {

           
            FragColor = texture(Texture0, TexCoord0);
        }
    )";

        renderShader.Create(vertexSrc, fragmentSrc);
        renderShader.LoadDefaults();
    }

    {
        const char *vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTex0;
        
        out vec2 TexCoord0;
   

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() 
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord0 = aTex0;
        }
    )";

        const char *fragmentSrc = R"(
        #version 330 core
        
        out vec4 FragColor;
        
        in vec2 TexCoord0;


        uniform sampler2D Texture;
        uniform sampler2D Reflection;
        uniform float reflectFactor;
        void main()
        {


                vec4 baseColor = texture(Texture, TexCoord0);
                vec4 reflectionColor = texture(Reflection, TexCoord0);


                    
                 FragColor =  mix(baseColor, reflectionColor, reflectFactor);
        }
    )";

        if (!mirrorShader.Create(vertexSrc, fragmentSrc))
        {
            Utils::LogError("Error creating mirror shader");
            return 1;
        }
        mirrorShader.LoadDefaults();
    }

    RenderBatch batch;
    batch.Init(1, 1024);
    Assets::Instance().SetFlipTexture(false);
    Shader *shader = Assets::Instance().GetShader("default");
    Texture2D *texture0 = Assets::Instance().LoadTexture("assets/align.jpg");

    Font font;

    font.LoadDefaultFont();
    font.SetBatch(&batch);
    font.SetSize(12);

    Vec3 cameraPos = Vec3(0.0f, 2.0f, 3.0f);
    Vec3 cameraFront = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = Input::GetMouseX();
    float lastY = Input::GetMouseY();

    Mesh *floor = MeshManager::Instance().CreatePlane(8, 8, 1, 1);
    Mesh *box = MeshManager::Instance().CreateBox(1.0f, 1.0f, 1.0f);
    Driver::Instance().SetClearColor(0.1f, 0.1f, 0.1f);

    RenderTexture rtt;
    rtt.Init(1024, 1024);
    rtt.addColorBuffer();
    rtt.addDepthBuffer();
    if (!rtt.Process())
    {
        Utils::LogError("Error creating render texture");
    };

    Plane mirrorPlane(Vec3(0, 1, 0), 0);

    while (device.Running())
    {

        float cameraSpeed = 35.5f * device.GetFrameTime();

        if (Input::IsMouseButtonDown(1))
        {
            float xpos = Input::GetMouseX();
            float ypos = Input::GetMouseY();
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            Vec3 front;
            front.x = Cos(yaw) * Cos(pitch);
            front.y = Sin(pitch);
            front.z = Sin(yaw) * Cos(pitch);
            cameraFront = Vec3::Normalize(front);
        }

        lastX = Input::GetMouseX();
        lastY = Input::GetMouseY();

        if (Input::IsKeyDown(SDLK_w))
        {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (Input::IsKeyDown(SDLK_s))
        {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (Input::IsKeyDown(SDLK_a))
        {
            cameraPos -= Vec3::Normalize(Vec3::Cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (Input::IsKeyDown(SDLK_d))
        {
            cameraPos += Vec3::Normalize(Vec3::Cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        //  position.x -= 0.1f;

        Mat4 model;
        Mat4 view = Mat4::LookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        Mat4 projection = Mat4::Perspective(45.0f, (float)device.GetWidth() / (float)device.GetHeight(), 0.1f, 1000.0f);

        Driver::Instance().SetTransform(VIEW_MATRIX, view);
        Driver::Instance().SetTransform(PROJECTION_MATRIX, projection);
        Driver::Instance().EnableBlend(false);
        Driver::Instance().EnableDepthTest(true);
        Driver::Instance().EnableCullFace(true);
        Driver::Instance().UpdateFrustum();

        // render mirror Firt pass
        Driver::Instance().SetClearColor(0.0f, 0.0f, 0.0f);

        renderShader.Bind();
        renderShader.SetMatrix4("projection", projection.m);
        renderShader.SetMatrix4("view", view.m);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        rtt.Begin();

        model.identity();
        model = Mat4::Translate(model, Vec3(0.0f, 1.0f, 0.0f));
        model = Mat4::Rotate(model, device.GetTime(), Vec3(0.0f, 1.0f, 0.0f));

        Vec3 mirrorNormal =Vec3::Normalize(Mat4::ToMat3(model) * Vec3(0.0f, 1.0f, 0.0f));
        Vec3 mirrorPos = model.getPosition();

        Mat4 reflectionMatrix = calculateReflectionMatrix(mirrorPos, mirrorNormal);
        Mat4 reflectedModel =model * reflectionMatrix;

        Mat4 mirrorProj;
        Mat4 mirrorView;

        MirrorMatrix(cameraPos, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), 100.0f,  mirrorProj, mirrorView);

        renderShader.SetMatrix4("projection", mirrorProj.m);
        renderShader.SetMatrix4("view", mirrorView.m);
        renderShader.SetMatrix4("model", reflectedModel.m);

        
        //Mat4 viewMIrror = Mat4::LookAt(Vec3(0.0f, 4.0f, 0.0f), Vec3(0.0f, 0.7f, 0.2f), Vec3(0.0f, 1.0f, 0.0f));
        //renderShader.SetMatrix4("model", reflectedModel.m);
        //renderShader.SetMatrix4("view", viewMIrror.m);
        texture0->Bind(0);
        box->Render(GL_TRIANGLES);

        rtt.End();

        glCullFace(GL_BACK);

        Driver::Instance().SetClearColor(0.1f, 0.1f, 0.1f);
        Driver::Instance().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderShader.SetMatrix4("projection", projection.m);
        renderShader.SetMatrix4("view", view.m);


        // render normal Second pass
        texture0->Bind(0);
        model.identity();
        model = Mat4::Translate(model, Vec3(0.0f, 1.0f, 0.0f));
        model = Mat4::Rotate(model, device.GetTime(), Vec3(0.0f, 1.0f, 0.0f));
        mirrorShader.SetMatrix4("model", model.m);
       // renderShader.SetMatrix4("model", glm::value_ptr(obj));
        box->Render(GL_TRIANGLES);

        // espelho

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rtt.GetColorBuffer(0));
        Driver::Instance().EnableBlend(true);
        Driver::Instance().SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        model.identity();
        model.scale(10.0f, 0.1f, 10.0f);
        mirrorShader.Bind();
        mirrorShader.SetMatrix4("model", model.m);
        mirrorShader.SetMatrix4("projection", projection.m);
        mirrorShader.SetMatrix4("view", view.m);
        mirrorShader.SetFloat("reflectFactor", 0.5f);
        mirrorShader.SetInt("Texture", 0);
        mirrorShader.SetInt("Reflection", 1);
        floor->Render(GL_TRIANGLES);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        Driver::Instance().EnableBlend(false);

        // batch -----------------------------------------------------------------------------
        shader->Bind();
        model.identity();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);

        batch.Grid(10, 0.1f, true);
        batch.Render();

        model = Mat4::Identity();
        view = Mat4::Identity();
        projection = Mat4::Orthographic(0.0f, (float)device.GetWidth(), (float)device.GetHeight(), 0.0f, -1.0f, 1.0f);

        shader->Bind();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);

        Driver::Instance().EnableBlend(true);
        Driver::Instance().EnableDepthTest(false);
        Driver::Instance().EnableCullFace(false);
        Driver::Instance().SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        batch.SetColor(1, 1, 1);
        font.SetSize(16);

        u64 drawcalls = Driver::Instance().GetTotalDrawCalls();
        font.Print(10, 20, " %d  %ld", device.GetFPS(), drawcalls);
        u64 triangles = Driver::Instance().GetTotalTriangles();
        u64 vertices = Driver::Instance().GetTotalVertices();
        font.Print(10, 40, "Triangles %ld  Vertices %ld", triangles, vertices);

        font.Print(10, 60, "Camera Pos: %f %f %f", cameraPos.x, cameraPos.y, cameraPos.z);

        batch.Render();

        device.Swap();
    }

    mirrorShader.Release();
    rtt.Release();
    renderShader.Release();
    batch.Release();
    font.Release();

    device.Close();

    return 0;
}
