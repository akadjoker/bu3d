#pragma once

#include "Core.hpp"
#include "Math.hpp"


class MeshBuffer
{
private:
    GLuint vbo;
    GLuint ebo;  
    GLuint vao; 
    size_t vertexCount;
    size_t indexCount;

public:
    MeshBuffer();
    ~MeshBuffer() = default;

    void Init();
    void Release();

    void Bind();
    void UnBind();

    void SetVertexData(const float *vertices, u32 count, const std::vector<GLint> &attribSizes,bool dynamic = false);
    void UpdateVertexData(const float *vertices, u32 count);
    void UpdateIndexData(const u32 *indices, u32 count);
    void SetIndexData(const u32 *indices, u32 count,bool dynamic = false);
    void Render(int mode = GL_TRIANGLES);
};


const u8 POSITION    = 1;
const u8 TEXCOORD    = 2;
const u8 NORMAL      = 4;
const u8 TANGENT     = 8;
const u8 BITANGENT   = 16;
const u8 INDICES     = 32;
const u8 WEIGHTS     = 64;
const u8 JOINTS      = 128;

class Mesh 
{
    private:
        u32 EBO;
        u32 VAO;
        u32 VBO[7];
        u8 flags;
        bool isDynamic;
        bool isFacesDynamic;
        bool isInitialized;
    public:
        std::vector<Vec3> vertices;   // 0 attrib
        std::vector<Vec2> texcoords;  // 1
        std::vector<Vec3> normals;    // 2
        std::vector<Vec3> tangents;   // 3
        std::vector<Vec3> bitangents; // 4
        std::vector<u32>  indices;
        std::vector<Vec4> weights;
        std::vector<Vec4> joints;
        u32 material;

        Mesh(bool dynamic = false, bool facesDynamic = false);
        ~Mesh();

        void Release();
        void Init();
        void Update();
        
        void Render(u32 mode, u32 start, u32 count);
        void Render(u32 mode, u32 count);
        void Render(u32 mode);

        u32 AddVertex(const Vec3 &pos);
        u32 AddVertex(float x, float y, float z);
        u32 Addface(u32 a, u32 b, u32 c);

        void AddMesh(Mesh *mesh);
        void AddMesh(Mesh *mesh, const Mat4 &transform);

        bool NeedsUpdate() const { return flags != 0; }
        void SetFlag(u8 flag) { flags |= flag; }
        void ClearFlag(u8 flag) { flags &= ~flag; }

};



class MeshManager
{
    private:
        static MeshManager *instance;
        std::vector<Mesh *> meshes;
    public:
        static MeshManager *InstancePtr();
        static MeshManager &Instance();

        Mesh *CreateMesh();

        Mesh *CreateBox(float width, float height, float length);
        Mesh *CreateSphere(float diameter, int rows, int columns);
        Mesh *CreateCone(float height, float diameter, int segments);
        Mesh *CreateCylinder( float height, float diameter, int segments );
        Mesh *CreatePlane(int stacks, int slices, int tilesX, int tilesY);
        Mesh *CreateTorus( int stacks, int slices, float innerRadius, float outerRadius);
        Mesh *CreateCapsule(float diameter, int rows, int columns, float height);

        void CalculateNormals(Mesh *mesh);
        void CalculateSmothNormals(Mesh *mesh,bool angleWeighted);
        void GetBoundingBox(Mesh *mesh,Vec3 &min,Vec3 &max);
        void TranslateMesh(Mesh *mesh,float x, float y, float z);
        void ScaleMesh(Mesh *mesh,float x, float y, float z);
        void RotateMesh(Mesh *mesh,const Vec3 &axis,float angle);

        void TexturePlanarMapping(Mesh *mesh,float resolution=0.001f);
        void TexturePlanarMapping(Mesh *mesh,float resolutionS, float resolutionT, u8 axis, const Vec3& offset);
        void FlipFaces(Mesh *mesh);
        void FlipNormals(Mesh *mesh);

        void Release();
        void Init();




};