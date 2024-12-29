#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"

int main()
{

    Device device;
    device.Init("OpenGL Device", 800, 600, true);



    RenderBatch batch;
    batch.Init(1, 1024);
    Assets::Instance().SetFlipTexture(false);
    Shader *shader = Assets::Instance().GetShader("default");

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

    Driver::Instance().SetClearColor(0.1f, 0.1f, 0.1f);

    BoundingBox box;
    box.min = Vec3(-0.5f, -0.5f, -0.5f);
    box.max = Vec3(0.5f, 0.5f, 0.5f);

    BoundingBox box2;


    while (device.Running())
    {
        Driver::Instance().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Driver::Instance().SetViewport(0, 0, device.GetWidth(), device.GetHeight());
        
        

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
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

          //  box2.expand(box);

      //  box2.min = box2.min;
     //   box2.max = box2.max;

        Mat4 transform;
        transform.identity();
      //  transform.translate(0, 1.5, 0);

        //   transform = Mat4::RotateY(transform, device.GetTime());
          transform = Mat4::Translate(transform, Vec3(0.0f, 5.0f, 0.0f));
          transform = Mat4::Scale(transform, Vec3(0.2f, 0.2f, 0.2f));
         transform = Mat4::Rotate(transform,device.GetTime(), Vec3(1.0f, 0.0f, 0.0f));
         transform = Mat4::Rotate(transform, device.GetTime(), Vec3(0.0f, 1.0f, 0.0f));
         transform = Mat4::Rotate(transform, device.GetTime(), Vec3(0.0f, 0.0f, 1.0f));

        box.transform(box2,transform);

        shader->Bind();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);

        batch.Box(box.min, box.max);
        batch.Box(box2.min, box2.max);
        batch.Grid(20, 0.1f, true);
        batch.Render();

        model.identity();
        view.identity();
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

        bool cull = Driver::Instance().IsInFrustum(box.min, box.max);

        font.Print(10, 20, " %d  %f", device.GetFPS(), device.GetFrameTime());
        font.Print(10, 40, "Cull: %d", cull);

        batch.Render();

        device.Swap();
    }

    batch.Release();
    font.Release();
    
    device.Close();

    return 0;
}
