
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


#include "Core.hpp"
#include "Render.hpp"
#include "Scene.hpp"

  
 int screenWidth = 1024 ;
 int screenHeight = 768 ;




int main()  
{

    Device      device;

    

    device.Create(screenWidth, screenHeight, "Hello World");

    Driver::Instance().Init();
    Driver::Instance().Resize(screenWidth, screenHeight);





    RenderBatch batch;
    Shader      shader;
    Font      font;
    Scene      scene;


    batch.Init(1,1024);

     font.LoadDefaultFont();
     font.SetBatch(&batch);
     font.SetSize(12);

    scene.Init();

     const char *vShader = GLSL(
         layout(location = 0) in vec3 position;
         layout(location = 1) in vec2 texCoord;
         layout(location = 2) in vec4 color;

         uniform mat4 model;
         uniform mat4 view;
         uniform mat4 projection;
         

         out vec2 TexCoord;
         out vec4 vertexColor;
         void main() {
             gl_Position = projection * view * model * vec4(position, 1.0);
             TexCoord = texCoord;
             vertexColor = color;
         });

     const char *fShader = GLSL(
         in vec2 TexCoord;
         out vec4 color;
         in vec4 vertexColor;
         uniform sampler2D texture0;
         void main() {
             color = texture(texture0, TexCoord) * vertexColor;
         });

     shader.Create(vShader, fShader);
     shader.LoadDefaults();

     VertexFormat::Element elements[] =
         {
             VertexFormat::Element(VertexFormat::POSITION, 3),
             VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
             VertexFormat::Element(VertexFormat::COLOR, 4)

         };

     Mesh *cube = MeshBuilder::Instance().CreateCube(VertexFormat(elements, 3), 1, 1, 1);
     cube->AddMaterial(new TextureMaterial(Texture2D::GetDefaultTexture()));

     Node3D node;
     Camera3D camera;

     camera.setLocalPosition(Vec3(0, 1.5f, -25.0f));
     

     Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    float mouseSensitivity = 0.1f;


  //   SDL_SetRelativeMouseMode(SDL_TRUE);
    int lastMouseX, lastMouseY;
    SDL_GetMouseState(&lastMouseX, &lastMouseY);  


    float yaw = 0;
    float pitch = 0;

    while (device.Run())
    {

        device.Update();

        Driver::Instance().Clear();

        Driver::Instance().SetBlend(false);
        Driver::Instance().SetDepthTest(true);


        if (Keyboard::Down(KEY_W))
        {
            camera.Move(0,0,-0.1f);
        }
        if (Keyboard::Down(KEY_S))
        {
            camera.Move(0,0,0.1f);
        }
        if (Keyboard::Down(KEY_A))
        {
            camera.Move(-0.1f,0,0);
        }
        if (Keyboard::Down(KEY_D))
        {
            camera.Move(0.1f,0,0);
        }

        if (Keyboard::Down(KEY_Q))
        {
            camera.Turn(0,0.1f,0,false);
        }
        if (Keyboard::Down(KEY_E))
        {
            camera.Turn(0,-0.1f,0,false);
        }

        int mouseX = 0, mouseY = 0;
        Uint32 button= SDL_GetMouseState(&mouseX, &mouseY);
        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);
        lastMouseX = mouseX;
        lastMouseY = mouseY;



        if (Mouse::Down(0))
        {
            yaw += Mouse::dX() * mouseSensitivity; 
            pitch -= Mouse::dY() * mouseSensitivity; 
            if (pitch > 89.0f   ) pitch = 89.0f;
            if (pitch < -89.0f  ) pitch = -89.0f;
            camera.Rotate(pitch, yaw, 0.0f, false);
        }
    node.Point(&camera, 0);

    Mat4 view = camera.getView(); // Mat4::LookAt(Vec3(0.0f, 10.8f, -25.0f), Vec3(0.0f, -0.5f, 10.0f), Vec3(0.0f, 1.0f, 0.0f));
    Mat4 projection = Mat4::Perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

    shader.Use();
  
    Mat4 transfom = node.getWorldTransform();
    Mat4 identity = Mat4::Identity();

    shader.Use();

    shader.SetMatrix4("view", &view.c[0][0]);
    shader.SetMatrix4("projection", &projection.c[0][0]);

    shader.SetMatrix4("model", &transfom.c[0][0]);
    cube->Render(&shader);

    shader.SetMatrix4("model", &identity.c[0][0]);
    batch.Grid(10, 10);
    batch.Render();

    Mat4 ortho = Mat4::Ortho(0.0f, 1024.0f, 720.0f, 0.0f, -1.0f, 1.0f);

    shader.Use();
    shader.SetMatrix4("model", &identity.c[0][0]);
    shader.SetMatrix4("view", &identity.c[0][0]);
    shader.SetMatrix4("projection", &ortho.c[0][0]);

    batch.SetColor(255, 255, 255, 255);

    Driver::Instance().SetBlend(true);
    Driver::Instance().SetBlendMode(BlendMode::BLEND);
    Driver::Instance().SetDepthTest(false);
    Driver::Instance().SetDepthWrite(false);

    font.Print(10, 20, " %f  %f",deltaX,deltaY);

    batch.Render();

    device.Flip();


    }

    scene.Release();
    font.Release();
    batch.Release();
    shader.Release();

  
    Driver::Instance().Release();
    device.Close();
  
 

    return 0;
}