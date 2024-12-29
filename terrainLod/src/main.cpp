#include "Terrain.hpp"


int main()
{

    Device device;
    device.Init("OpenGL Device", 800, 600, true);

    Shader renderShader;

    const char *vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTex0;
        layout(location = 2) in vec2 aTex1;
        
 
        
        
        out vec2 TexCoord0;
        out vec2 TexCoord1;

            
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;


       
        void main() 
        {
            gl_Position = projection * view * model * vec4( aPos, 1.0);
            TexCoord0 = aTex0;
            TexCoord1 = aTex1;
            
        }
    )";

    const char *fragmentSrc = R"(
        #version 330 core
        
        in vec2 TexCoord0;
        in vec2 TexCoord1;
        
        
        uniform sampler2D Texture0;
        uniform sampler2D Texture1;

        out vec4 FragColor;

        void main()
         {
            
            FragColor = mix(texture(Texture0, TexCoord0), texture(Texture1, TexCoord1), 0.2);
        }
    )";

    renderShader.Create(vertexSrc, fragmentSrc);
    renderShader.LoadDefaults();

    RenderBatch batch;
    batch.Init(1, 1024);
    Assets::Instance().SetFlipTexture(false);
    Shader *shader = Assets::Instance().GetShader("default");
    Texture2D *texture0 = Assets::Instance().LoadTexture("assets/Texture.jpg"); 
    Texture2D *texture1 = Assets::Instance().LoadTexture("assets/detail.jpg"); 

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

    Mesh *floor = MeshManager::Instance().CreatePlane(8,8, 2, 2);
 //   MeshManager::Instance().ScaleMesh(floor, 20.0f, 1.0f, 20.0f);
   

    Heightmap heightmap(40.0f);
    //heightmap.LoadImage("assets/heightmap.png");
    heightmap.Load("assets/Terrain.raw", 8);
   // heightmap.Save("assets/heightmap.raw", 24);

    // TerrainChunk chunk(33, 32.0f, 0, 0, 16.0f);
    // chunk.GenerateMesh(heightmap);

    Terrain terrain(Vec3(0.0f, -300.0f, 0.0f), Vec3(8.0f, 10.0f, 8.0f), 17, 4);
    terrain.LoadTerrain("assets/Terrain.raw.png", 28.0f);
  //  terrain.LoadHeightmap(heightmap);

    //    TerrainChunk(int res, float chunkSize,   int chunkX, int chunkZ, float detailScale = 16.0f);

   // Terrain terrain(256, 64, 32, Vec3(0.0f, -100.0f, 0.0f), Vec3(2.0f, 2.0f, 2.0f));
    //terrain.SetPaintScale(2028.0f);
   // terrain.GenerateMeshWorld(heightmap);
    Driver::Instance().SetClearColor(0.1f, 0.1f, 0.1f);

 
    while (device.Running())
    {
        Driver::Instance().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Driver::Instance().SetViewport(0, 0, device.GetWidth(), device.GetHeight());


        float cameraSpeed = 80.5f * device.GetFrameTime();

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

      
        model.identity();
        model.translate(0.0f, 0.0f, 0.0f);

  
        renderShader.Bind();




        renderShader.SetMatrix4("model", &model.m[0]);
        renderShader.SetMatrix4("view", view.m);
        renderShader.SetMatrix4("projection", projection.m);
        renderShader.SetInt("Texture0", 0);
        renderShader.SetInt("Texture1", 1);
        
        texture0->Bind(0);
        texture1->Bind(1);
        terrain.Render();

        floor->Render(GL_TRIANGLES);




       
        shader->Bind();
        model.identity();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);
        
    //    terrain.Debug(&batch);
        batch.Grid(20, 0.1f, true);
        batch.Render();

        model = Mat4::Identity();
        view  = Mat4::Identity();
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
        font.Print(10, 20, " %d  %ld",device.GetFPS(),drawcalls);
        u64 triangles = Driver::Instance().GetTotalTriangles();
        u64 vertices = Driver::Instance().GetTotalVertices();
        font.Print(10, 40, "Triangles %ld  Vertices %ld", triangles, vertices);


        
       

        batch.Render();

        device.Swap();
    }

    terrain.Release();    
    renderShader.Release();
    batch.Release();
    font.Release();
    
    device.Close();

    return 0;
}
