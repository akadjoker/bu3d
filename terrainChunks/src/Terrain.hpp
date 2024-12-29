#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"

struct Vertex
{
    float x, y, z;      // Posição
    float u1, v1;       // Coordenadas para textura principal
    float u2, v2;       // Coordenadas para textura de detalhe

};

class TerrainChunk
{
private:
    int resolution;
    float size;
    std::vector<float> vertices;
    std::vector<u32> indices;
    BoundingBox boundingBox;
    int chunkX, chunkZ;  
    MeshBuffer buffer;

public:
    TerrainChunk(int res, float chunkSize,   int chunkX, int chunkZ);


    void Release();
    void GenerateMesh( Heightmap &heightmap,float detailScale, float paintScale);
    void GenerateMesh(Heightmap &heightmap, const Vec3 &position, const Vec3 &scale, float detailScale, float paintScale);
    void Render();
    void Debug(RenderBatch *batch);

};


class Terrain 
{
private:
    std::vector<TerrainChunk*> chunks;
    int chunkResolution;
    float chunkSize;
    float textureDetailScale;
    float texturePaintScale;
    Vec3 m_position;
    Vec3 m_scale;
    

public:
    Terrain(int terrainSize, int chunkResolution, float chunkSize);
    Terrain(int terrainSize, int chunkResolution, float chunkSize, const Vec3 &position, const Vec3 &scale);
    ~Terrain();

    void SetPaintScale(float scale);
    void SetDetailScale(float scale);
    void GenerateMesh(Heightmap &heightmap);
    void GenerateMeshWorld(Heightmap &heightmap);
    void Release();
    void Render();
    void Debug(RenderBatch *batch);
};
