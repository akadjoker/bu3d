
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
     shader.SetInt("texture0", 0);


    scene.Init();

    VertexFormat::Element VertexElements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
        VertexFormat::Element(VertexFormat::NORMAL, 3),
    }; 



    Camera3D *camera = scene.GetMainCamera();
    camera->setPerspective(60, (float)screenWidth / (float)screenHeight, 0.5f, 1000.0f);
    camera->setLocalPosition(Vec3(0, 1.5f, 5.0f));

    Mesh *cubeMesh = MeshBuilder::Instance().CreateCube(VertexFormat(VertexElements, 3),1,1,1);
    
    //System::Instance().ChangeDirectory("../");

    #if defined(_WIN32)

        Driver::Instance().SetTexturePath("E:\\projectos\\bu3d\\bin\\assets\\textures\\");
    #else
        Driver::Instance().SetTexturePath("assets/textures/");
    #endif

    cubeMesh->AddMaterial(new Material(Driver::Instance().LoadTexture("tiles.jpg")));

     Mesh *planeMesh = MeshBuilder::Instance().CreatePlane(VertexFormat(VertexElements, 3), 10, 10, 8, 8);
     planeMesh->AddMaterial(new Material(Driver::Instance().LoadTexture("wood.png")));



    Model *plane = scene.AddModel(planeMesh);
    plane->setLocalPosition(Vec3(0, 0.5, 0));
    plane->setLocalScale(Vec3(10, 1, 10));
   // plane->SetShadow(true);


    Model *cube = scene.AddModel(cubeMesh);
    cube->setLocalPosition(Vec3(0, 1, 0));
    cube->SetCastShadows(true);

    cube = scene.AddModel(cubeMesh);
    cube->setLocalPosition(Vec3(0, 1.5, 1.5));
    cube->SetCastShadows(true);

     

    Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    float mouseSensitivity = 0.1f;




    float yaw = 0;
    float pitch = 0;

    while (device.Run())
    {

        device.Update();
        scene.Update(device.GetFrameTime());

        Driver::Instance().Clear();

      //  Driver::Instance().SetBlend(false);
     //   Driver::Instance().SetDepthTest(true);


        if (Keyboard::Down(KEY_W))
        {
            camera->Move(0,0,-0.1f);
        }
        if (Keyboard::Down(KEY_S))
        {
            camera->Move(0,0,0.1f);
        }
        if (Keyboard::Down(KEY_A))
        {
            camera->Move(-0.1f,0,0);
        }
        if (Keyboard::Down(KEY_D))
        {
            camera->Move(0.1f,0,0);
        }

        if (Keyboard::Down(KEY_Q))
        {
            camera->Turn(0,0.1f,0,false);
        }
        if (Keyboard::Down(KEY_E))
        {
            camera->Turn(0,-0.1f,0,false);
        }

        


        if (Mouse::Down(0))
        {
            yaw += Mouse::dX() * mouseSensitivity; 
            pitch -= Mouse::dY() * mouseSensitivity; 
            if (pitch > 89.0f   ) pitch = 89.0f;
            if (pitch < -89.0f  ) pitch = -89.0f;
            camera->Rotate(pitch, yaw, 0.0f, false);
        }

    Mat4 view = camera->getView(); 
    Mat4 projection =camera->getProjection();
    Mat4 identity = Mat4::Identity();

    scene.Render();


    shader.Use();
    shader.SetMatrix4("view", &view.c[0][0]);
    shader.SetMatrix4("projection", &projection.c[0][0]);
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

    font.Print(10, 20, " %d  %f",device.GetFPS(),device.GetFrameTime());

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