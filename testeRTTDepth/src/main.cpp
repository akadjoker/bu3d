#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"


int main()
{

    Device device;
    device.Init("OpenGL Device", 800, 600, true);

    Shader renderShader;

    const char *vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;

        
 


            
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;


       
        void main() 
        {
            gl_Position = projection * view * model * vec4( aPos, 1.0);

            
        }
    )";

    const char *fragmentSrc = R"(
        #version 330 core
       
        


        out vec4 FragColor;

        void main()
         {
            
          //  FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    renderShader.Create(vertexSrc, fragmentSrc);
    renderShader.LoadDefaults();





    Vec3 cameraPos = Vec3(0.0f, 2.0f, 3.0f);
    Vec3 cameraFront = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = Input::GetMouseX();
    float lastY = Input::GetMouseY();

    Mesh *floor = MeshManager::Instance().CreatePlane(8,8, 2, 2);
    Driver::Instance().SetClearColor(0.1f, 0.1f, 0.1f);

     RenderTexture rtt;
    rtt.Init(1024, 1024); 
    rtt.addDepthTexture();
    if (!rtt.Process()) 
    {
        Utils::LogError("Error creating render texture");
    };

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



      

        Mat4 model;
        Mat4 view = Mat4::LookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        Mat4 projection = Mat4::Perspective(45.0f, (float)device.GetWidth() / (float)device.GetHeight(), 0.1f, 1000.0f);

        Driver::Instance().SetTransform(VIEW_MATRIX, view);
        Driver::Instance().SetTransform(PROJECTION_MATRIX, projection);
        Driver::Instance().EnableBlend(false);
        Driver::Instance().EnableDepthTest(true);
        Driver::Instance().EnableCullFace(true);
        Driver::Instance().UpdateFrustum();

        model.identity();
        model.translate(0.0f, 0.0f, 0.0f);

  
        renderShader.Bind();




        renderShader.SetMatrix4("model", &model.m[0]);
        renderShader.SetMatrix4("view", view.m);
        renderShader.SetMatrix4("projection", projection.m);
  

        rtt.Begin();

        Driver::Instance().EnableColorMask(false);
        floor->Render(GL_TRIANGLES);
        Driver::Instance().EnableColorMask(true);

        rtt.End();

        Driver::Instance().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 

        

 
        Driver::Instance().EnableBlend(true);
        Driver::Instance().EnableDepthTest(false);
        Driver::Instance().EnableCullFace(false);
        Driver::Instance().SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
        


        Shader *screen = Assets::Instance().GetShader("screen");
        ScreenQuad quad = Driver::Instance().GetScreenQuad();
        screen->Bind();
        screen->SetInt("Depth", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rtt.GetDepthTexture());
        quad.Render(0.0f, 0.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        device.Swap();
    }

    rtt.Release();
    renderShader.Release();
  
    device.Close();

    return 0;
}
