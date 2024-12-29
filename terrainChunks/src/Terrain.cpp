#include "Terrain.hpp"

TerrainChunk::TerrainChunk(int res, float chunkSize, int chunkX, int chunkZ)
    : resolution(res), size(chunkSize), chunkX(chunkX), chunkZ(chunkZ)
{
}

void TerrainChunk::Release()
{
    buffer.Release();
}

void TerrainChunk::GenerateMesh(Heightmap &heightmap, const Vec3 &position, const Vec3 &scale, float detailScale, float paintScale)
{
    float step = size / (resolution - 1);
    vertices.clear();
    indices.clear();

    for (int z = 0; z < resolution; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            float worldX = chunkX * size + x * step;
            float worldZ = chunkZ * size + z * step;
            float height = heightmap.GetInterpolatedHeight(worldX, worldZ);

            Vertex v;
            v.x = position.x + worldX * scale.x;
            v.y = position.y + height * scale.y;
            v.z = position.z + worldZ * scale.z;

            v.u1 = worldX / paintScale;
            v.v1 = worldZ / paintScale;
            v.u2 = v.u1 * detailScale;
            v.v2 = v.v1 * detailScale;

            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            vertices.push_back(v.u1);
            vertices.push_back(v.v1);
            vertices.push_back(v.u2);
            vertices.push_back(v.v2);

            boundingBox.expand(Vec3(v.x, v.y, v.z));
        }
    }
    for (int z = 0; z < resolution - 1; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            int top = z * resolution + x;
            int bottom = (z + 1) * resolution + x;
            indices.push_back(top);
            indices.push_back(bottom);
        }

        if (z < resolution - 2)
        {
            indices.push_back((z + 1) * resolution + (resolution - 1));
            indices.push_back((z + 1) * resolution);
        }
    }

    buffer.SetVertexData(vertices.data(), vertices.size(), {3, 2, 2}, true);
    buffer.SetIndexData(indices.data(), indices.size(), false);
}

void TerrainChunk::GenerateMesh(Heightmap &heightmap, float detailScale, float paintScale)
{
    float step = size / (resolution - 1);
    vertices.clear();
    indices.clear();

    for (int z = 0; z < resolution; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            float worldX = chunkX * size + x * step;
            float worldZ = chunkZ * size + z * step;

            float height = heightmap.GetInterpolatedHeight(worldX, worldZ);

            Vertex v;
            v.x = worldX;
            v.y = height;
            v.z = worldZ;
            // v.u1 = (float)x / (resolution - 1);
            // v.v1 = (float)z / (resolution - 1);
            v.u1 = (float)worldX / paintScale;
            v.v1 = (float)worldZ / paintScale;
            v.u2 = v.u1 * detailScale;
            v.v2 = v.v1 * detailScale;

            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            vertices.push_back(v.u1);
            vertices.push_back(v.v1);
            vertices.push_back(v.u2);
            vertices.push_back(v.v2);
            boundingBox.expand((Vec3(v.x, v.y, v.z)));
        }
    }


    for (int z = 0; z < resolution - 1; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            int top = z * resolution + x;
            int bottom = (z + 1) * resolution + x;
            indices.push_back(top);
            indices.push_back(bottom);
        }

        if (z < resolution - 2)
        {
            indices.push_back((z + 1) * resolution + (resolution - 1));
            indices.push_back((z + 1) * resolution);
        }
    }

    buffer.SetVertexData(vertices.data(), vertices.size(), {3, 2, 2}, true);
    buffer.SetIndexData(indices.data(), indices.size(), false);
}

void TerrainChunk::Render()
{
    if (!Driver::Instance().IsInFrustum(boundingBox))
    {
        return;
    }
    buffer.Render(GL_TRIANGLE_STRIP);
}

void TerrainChunk::Debug(RenderBatch *batch)
{
    batch->SetColor(1.0f, 0.0f, 0.0f);
    batch->Box(boundingBox.min, boundingBox.max);
}

Terrain::Terrain(int terrainSize, int chunkResolution, float chunkSize)
    : chunkResolution(chunkResolution), chunkSize(chunkSize), textureDetailScale(chunkSize)
{
    int numChunksPerSide = terrainSize / chunkSize;
    texturePaintScale = terrainSize;
    m_position = Vec3(0.0f, 0.0f, 0.0f);
    m_scale = Vec3(1.0f, 1.0f, 1.0f);

    for (int z = 0; z < numChunksPerSide; ++z)
    {
        for (int x = 0; x < numChunksPerSide; ++x)
        {
            chunks.push_back(new TerrainChunk(chunkResolution, chunkSize, x, z));
        }
    }
}

Terrain::Terrain(int terrainSize, int chunkResolution, float chunkSize, const Vec3& position, const Vec3& scale)
    : chunkResolution(chunkResolution), chunkSize(chunkSize), textureDetailScale(chunkSize)
{
    m_position = position;
    m_scale = scale;
    int numChunksPerSide = terrainSize / chunkSize;
    texturePaintScale = terrainSize;

    for (int z = 0; z < numChunksPerSide; ++z)
    {
        for (int x = 0; x < numChunksPerSide; ++x)
        {
          //  Vec3 chunkPosition = position + Vec3(x * chunkSize * scale.x, 0.0f, z * chunkSize * scale.z);
            chunks.push_back(new TerrainChunk(chunkResolution, chunkSize, x, z));
        }
    }
}

Terrain::~Terrain()
{
    for (TerrainChunk *chunk : chunks)
    {
        delete chunk;
    }
}

void Terrain::SetPaintScale(float scale)
{
    texturePaintScale = scale;
}

void Terrain::SetDetailScale(float scale)
{
    textureDetailScale = scale;
}

void Terrain::Release()
{
    for (TerrainChunk *chunk : chunks)
    {
        chunk->Release();
        delete chunk;
    }
    chunks.clear();
}

void Terrain::Render()
{
    for (const auto &chunk : chunks)
    {
        
        chunk->Render();
    }
}

void Terrain::Debug(RenderBatch *batch)
{
    for (const auto &chunk : chunks)
    {
        chunk->Debug(batch);
    }
}

void Terrain::GenerateMesh(Heightmap &heightmap)
{
    for (TerrainChunk *chunk : chunks)
    {
        chunk->GenerateMesh(heightmap, textureDetailScale, texturePaintScale);
    }
}

void Terrain::GenerateMeshWorld(Heightmap &heightmap)
{
    for (TerrainChunk *chunk : chunks)
    {
        chunk->GenerateMesh(heightmap, m_position, m_scale, textureDetailScale, texturePaintScale);
    }
}

