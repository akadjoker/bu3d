#pragma once
#include "Config.hpp"
#include "Math.hpp"
#include "OpenGL.h"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "String.hpp"






class    MeshBuilder
{
    public:


    static MeshBuilder& Instance();

    void Release();




    void Init();

    void SetLoadPath(const String &path);
    String GetLoadPath() const;

 
    Mesh *CreateMesh(const VertexFormat& vertexFormat, bool dynamic = false);


    Mesh*  CreateCube(const VertexFormat& vertexFormat,float width, float height, float length);
    Mesh * CreatePlane(const VertexFormat& vertexFormat,int stacks, int slices, int tilesX, int tilesY);
    Mesh *CreateSphere(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateCylinder(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateCone(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateTorus(const VertexFormat& vertexFormat,int stacks, int slices, float innerRadius, float outerRadius);
    Mesh* CreateHillsPlane(const VertexFormat& vertexFormat,
    const FloatSize& tileSize,
    const IntSize& tileCount,
    float hillHeight,
    const FloatSize& countHills,
    const FloatSize& textureRepeatCount);

         Mesh* CreateTerrain(const VertexFormat& vertexFormat, Pixmap& heightmap, const Pixmap& colors, const FloatSize& stretchSize, f32 maxHeight, const IntSize& maxVtxBlockSize);

    //     Mesh * LoadMesh(const VertexFormat& vertexFormat,const String &fileName);

private:
  


    String m_loadPath;

    Vector<Mesh*> m_meshes;



};