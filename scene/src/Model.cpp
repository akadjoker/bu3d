#include "Model.hpp"
#include "MeshBuilder.hpp"
#include "Driver.hpp"
#include "OS.hpp"
#include "File.hpp"
 Model::Model():Node3D()
 {
    
 }

Model::~Model()
{

    
}

Mesh *Model::AddMesh(const VertexFormat& vertexFormat)
{
    Mesh *mesh = MeshBuilder::Instance().CreateMesh(vertexFormat);
    return AddMesh(mesh);
}

Mesh *Model::AddMesh(Mesh *mesh)
{
    m_meshs.push_back(mesh);

    return mesh;
}

bool Model::Load(const String &fileName,const VertexFormat& vertexFormat)
{


    FileStream stream(fileName, "rb");
    if (!stream.IsOpen())
    {
        LogError("Cannot load %s", fileName.c_str());
        return false;
    }

    stream.ReadChar();
    stream.ReadChar();
    stream.ReadChar();


    u32 version = stream.ReadInt();

    if (version != 2024)
    {
        LogError("Unsupported version %d", version);
        return false;
    }

    u32 countMaterial = stream.ReadInt();
    Mesh *mesh = AddMesh(vertexFormat);

    for (u32 i = 0; i < countMaterial; i++)
    {
        String diffuse = stream.ReadUTFString();
        String name    = stream.ReadUTFString();

        Texture2D *texture = Driver::Instance().LoadTexture(diffuse.c_str());
        TextureMaterial *mat =  new TextureMaterial(texture);
        mesh->AddMaterial(mat);

        Logger::Instance().Info("Material %s texture %s",name.c_str(),diffuse.c_str());

        
    }


    u32 countSurfaces = stream.ReadInt();
    for (u32 i = 0; i < countSurfaces; i++)
    {

        
       String name       = stream.ReadUTFString();
        int material   = stream.ReadInt();
        u32 countVertices = stream.ReadInt();
        u32 countFaces = stream.ReadInt();

        Surface *surf = mesh->AddSurface(material);

        LogInfo("Surface %s %d %d %d", name.c_str(), material, countVertices, countFaces);

        for (u32 j = 0; j < countVertices; j++)
        {
            Vec3 pos;
            Vec3 normal;
            Vec2 uv;

            pos.x = stream.ReadFloat();
            pos.y = stream.ReadFloat();
            pos.z = stream.ReadFloat();

            normal.x = stream.ReadFloat();
            normal.y = stream.ReadFloat();
            normal.z = stream.ReadFloat();

            uv.x = stream.ReadFloat();
            uv.y = stream.ReadFloat();
            surf->AddVertex(pos, uv,normal);

            

        }
 
        for (u32 j = 0; j < countFaces; j++)
        {
            int a = stream.ReadInt();
            int b = stream.ReadInt();
            int c = stream.ReadInt();
            surf->AddFace(a, b, c);
        }
        //surf->CalculateTangents();
        surf->Upload(); 
        surf->CalculateBoundingBox();    
        m_boundingBox.merge(surf->GetBoundingBox());
    }
    mesh->CalculateBoundingBox();

    return true;
}
void Model::Release()
{
    m_meshs.clear();
    for (u32 i = 0; i < m_materials.size(); i++)
    {
        m_materials[i]->Release();
        delete m_materials[i];
    }
    m_materials.clear();
}

Vec3 Model::GetSize()
{
    float x = m_boundingBox.max.x - m_boundingBox.min.x;
    float y = m_boundingBox.max.y - m_boundingBox.min.y;
    float z = m_boundingBox.max.z - m_boundingBox.min.z;

    return Vec3(x, y, z);
}

void Model::Render(Shader *shader) 
{

    for (u32 i = 0; i < m_meshs.size(); i++)
    {
        m_meshs[i]->Render(shader);
    }

}

void Model::RenderNoMaterial(Shader *shader)
{

    for (u32 i = 0; i < m_meshs.size(); i++)
    {
        m_meshs[i]->Render(shader, true);
    }
}


void Model::Transform(const Mat4 &transform)
{

    for (u32 i = 0; i < m_meshs.size(); i++)
    {
        m_meshs[i]->Transform(transform);
    }
}



void Model::CalculateBox()
{
    m_boundingBox.clear();
    for (u32 i = 0; i < m_meshs.size(); i++)
    {
        m_boundingBox.merge(m_meshs[i]->GetBoundingBox());
    }
}
