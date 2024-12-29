#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Batch.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"




struct SPatch
{
    SPatch()
        : Top(0), Bottom(0), Right(0), Left(0), CurrentLOD(-1), PreviousLOD(-1)
    {
    }

    SPatch *Top;
    SPatch *Bottom;
    SPatch *Right;
    SPatch *Left;
    s32 CurrentLOD;
    s32 PreviousLOD;
    BoundingBox Box;
    Vec3 Center;
};



class Terrain
{
    private:
        
        MeshBuffer mesh;
        
        std::vector<u32>   indices;
        std::vector<float> vertices;
        std::vector<float> LODDistanceThreshold;
        
        bool OverrideDistanceThreshold;
        float textureDetailScale;
        float texturePaintScale;

        bool lodChanged;
        Vec3 OldCameraRotation;
        Vec3 OldCameraPosition;
        float CameraMovementDelta;
        float CameraRotationDelta;

        SPatch *Patches;
        s32 Size;
        Vec3 Position;
        Vec3 Scale;
        Vec3 Center;
        s32 PatchSize;
        s32 CalcPatchSize;
        s32 PatchCount;
        s32 MaxLOD;
        BoundingBox Box;
     


    private:
        Vec3 getPosition(int index);
        void calculateDistanceThresholds();
        void createPatches();
        void calculatePatchNeighbors();
        bool preRenderLODCalculations();
        void preRenderIndicesCalculations();
        u32 getIndex(const s32 PatchX, const s32 PatchZ, const s32 PatchIndex, u32 vX, u32 vZ);

    public:
        Terrain(const Vec3 &position,  const Vec3 &scale, s32 patchSize, s32 maxLOD);
        ~Terrain();

        bool LoadTerrain(const char *filename,float heightScale);
        bool LoadHeightmap(Heightmap &heightmap);

        void Release();
        void Render();
        void Debug(RenderBatch *batch);

        void SetPaintScale(float scale);
        void SetDetailScale(float scale);
    

};