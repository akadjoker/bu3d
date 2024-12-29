#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <iostream>

// void PrintGLMMatrix(const glm::mat4 &matrix)
// {
//     std::cout << "glm::mat4:" << std::endl;
//     for (int row = 0; row < 4; ++row)
//     {
//         for (int col = 0; col < 4; ++col)
//         {
//             std::cout << matrix[col][row] << " "; // glm usa colunas
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
// }
// void PrintCustomMatrix(const Mat4 &matrix)
// {
//     std::cout << "Custom Mat4:" << std::endl;
//     for (int row = 0; row < 4; ++row)
//     {
//         for (int col = 0; col < 4; ++col)
//         {
//             std::cout << matrix.at(row, col) << " ";
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
// }
int main()
{

    Device device;
    device.Init("OpenGL Device", 800, 600, true);

    Shader quadShader;

    const char *vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTex;
        layout(location = 2) in vec3 aNormal;
        
        
        out vec2 TexCoord;
        out vec3 Normal;
        out vec3 FragPos;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            uniform float time; // Novo uniform para tempo
            uniform vec2 windDirection = vec2(1.0, 0.3); // Direção do vento
            uniform float windStrength = 0.3; // Força do vento

       
        void main() 
        {
            vec3 pos = aPos;
            
            // Aplica o vento apenas se o vértice não estiver na base (y > 0)
            if(pos.y > 0.0) 
            {
                float windWave = sin(time * 2.0 + pos.x * 0.5 + pos.z * 0.5) * 0.5 + 0.5;
                
                // O efeito aumenta com a altura (mais movimento no topo)
                float heightFactor = pos.y / 1.5; // 1.5 é o GRASS_HEIGHT
                
                // Aplica o movimento do vento
                pos.x += windDirection.x * windWave * windStrength * heightFactor;
                pos.z += windDirection.y * windWave * windStrength * heightFactor;
                
                // Adiciona um leve movimento vertical
                pos.y += sin(time * 3.0 + pos.x * 0.5) * 0.05 * heightFactor;
            }
            
            FragPos = vec3(model * vec4(pos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTex;

    gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    const char *fragmentSrc = R"(
        #version 330 core
        in vec2 TexCoord;
        
        uniform vec3 lightPos;
        uniform sampler2D ourTexture;
        in vec3 Normal;
        in vec3 FragPos;
        out vec4 FragColor;

        void main()
         {
            vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
            
            float ambientStrength = 0.8f;

            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            vec3 ambient = ambientStrength * lightColor;    

            vec3 result = (ambient + diffuse);

          

            FragColor = texture(ourTexture, TexCoord)  * vec4(result, 1.0f);
            if (FragColor.a < 0.01f)
            {
                discard;
            }
            
        }
    )";

    quadShader.Create(vertexSrc, fragmentSrc);
    quadShader.LoadDefaults();

    RenderBatch batch;
    batch.Init(1, 1024);
    Assets::Instance().SetFlipTexture(false);
    Shader *shader = Assets::Instance().GetShader("default");
    Texture2D *texture = Assets::Instance().LoadTexture("assets/grass_1024.jpg"); 
    Texture2D *grasstexture =  Assets::Instance().LoadTexture("assets/grassWalpha.tga");
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


  //  Mesh *cube = MeshManager::Instance().CreateSphere(1.0f, 20,20);
    //Mesh *cube = MeshManager::Instance().CreateBox(1.0f, 1.0f, 1.0f);
 //   Mesh *cube = MeshManager::Instance().CreateCone(1.0f, 1.5f, 32);

    //Mesh *cube = MeshManager::Instance().CreateCylinder(1.0f, 1.0f, 32);

    Mesh *cube = MeshManager::Instance().CreatePlane(8,8, 8, 8);
    MeshManager::Instance().ScaleMesh(cube, 20.0f, 1.0f, 20.0f);
   

    //Mesh *cube = MeshManager::Instance().CreateTorus(32, 32, 0.5f, 1.0f);
 //   Mesh *cube = MeshManager::Instance().CreateCapsule(0.5f, 20, 20, 1.0f);

    Mesh *grass = MeshManager::Instance().CreateMesh();
    Mesh *GrassField = MeshManager::Instance().CreateMesh();

    const int GRASS_WIDTH = 1.5;
    const int GRASS_HEIGHT = 1.5;

    grass->vertices.resize(12);
    grass->normals.resize(12);
    grass->texcoords.resize(12);

    for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			// planes intersect along the Y axis with 60 degrees between them
			float x = Cos((i * 60)) * GRASS_WIDTH / 2;
			float z = Sin((i * 60)) * GRASS_WIDTH / 2;

			for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
			{
				Vec3& position = grass->vertices[i * 4 + j];
                Vec2& texcoord = grass->texcoords[i * 4 + j];
                Vec3& normal = grass->normals[i * 4 + j];

				position.x = j < 2 ? -x : x;
				position.y = j % 2 ? 0 : GRASS_HEIGHT;
				position.z = j < 2 ? -z : z;

				normal.x = 0;
				normal.y = 1;
				normal.z = 0;

				texcoord.x = j < 2 ? 0 : 1;
				texcoord.y = j % 2;
			}
		}

    for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			unsigned int off = i * 4; 
            grass->indices.push_back(off + 0);
            grass->indices.push_back(off + 3);
            grass->indices.push_back(off + 1);

            grass->indices.push_back(off + 0);
            grass->indices.push_back(off + 2);
            grass->indices.push_back(off + 3);

            grass->indices.push_back(off + 1);
            grass->indices.push_back(off + 3);
            grass->indices.push_back(off + 0);

            grass->indices.push_back(off + 3);
            grass->indices.push_back(off + 2);
            grass->indices.push_back(off + 0);
        }

    	for (int x = -60; x < 60; x += GRASS_WIDTH)
		{
			for (int z = -60; z < 60; z += GRASS_WIDTH)
			{
                Quat q = Quat(Vec3(0.0f, 1.0f, 0.0f), RangeRandom(0.0f, 360.0f));
                Mat4 rotation = q.toMat4();
                Mat4 scale =Mat4::Scale(Vec3(1.0f, RangeRandom(0.45f, 1.15f), 1.0f));
                Mat4 position = Mat4::Translate(Vec3(x + RangeRandom(-2,2), 0, z+ RangeRandom(-2,2)));
                Mat4 transform = scale * rotation * position;
                GrassField->AddMesh(grass, transform);
            }
        }
 
    // MeshManager::Instance().RotateMesh(cube, Vec3(0.0f, 0.0f, 1.0f), 90.0f);

    Vec3 lightPosition = Vec3(0.0f, 5.0f, 5.0f);


    while (device.Running())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glViewport(0, 0, device.GetWidth(), device.GetHeight());

        float cameraSpeed = 5.5f * device.GetFrameTime();

        lightPosition.x = cos(device.GetTime() * 2.0f) * 5.0f;
        lightPosition.y = 2.0f;
        lightPosition.z = sin(device.GetTime() * 2.0f) * 5.0f;

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
            front.x = cos(Radians(yaw)) * cos(Radians(pitch));
            front.y = sin(Radians(pitch));
            front.z = sin(Radians(yaw)) * cos(Radians(pitch));
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

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        Mat4 model;
        Mat4 view = Mat4::LookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        Mat4 projection = Mat4::Perspective(45.0f, (float)device.GetWidth() / (float)device.GetHeight(), 0.1f, 1000.0f);

        //GeoTerrain terrain(Vec3(1.0f, 1.0f, 1.0f), 5, TERRAIN_PATCH_SIZE::TPS_17, 0.5f);

        model.identity();
        model.translate(0.0f, 0.0f, 0.0f);

  
        quadShader.Bind();




        quadShader.SetMatrix4("model", &model.m[0]);
        quadShader.SetMatrix4("view", view.m);
        quadShader.SetMatrix4("projection", projection.m);
        quadShader.SetInt("ourTexture", 0);
        quadShader.SetFloat("lightPos",lightPosition.x, lightPosition.y, lightPosition.z);
        quadShader.SetFloat("time", 0);
        texture->Bind(0);

        quadShader.SetFloat("time", device.GetTime());
        cube->Render(GL_TRIANGLES);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        grasstexture->Bind(0);
        GrassField->Render(GL_TRIANGLES);

       
        shader->Bind();
        model.identity();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);
        
        batch.Grid(20, 0.1f, true);
        batch.Cube(lightPosition, 0.5f, 0.5f, 0.5f, false);
        batch.Render();

        model = Mat4::Identity();
        view  = Mat4::Identity();
        projection = Mat4::Orthographic(0.0f, (float)device.GetWidth(), (float)device.GetHeight(), 0.0f, -1.0f, 1.0f);

        shader->Bind();
        shader->SetMatrix4("model", model.m);
        shader->SetMatrix4("view", view.m);
        shader->SetMatrix4("projection", projection.m);
        

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        batch.SetColor(1, 1, 1);
        font.SetSize(16);

        font.Print(10, 20, " %d  %f",device.GetFPS(),device.GetFrameTime());

        batch.Render();

        device.Swap();
    }

    
    quadShader.Release();
    batch.Release();
    font.Release();
    
    device.Close();

    return 0;
}
