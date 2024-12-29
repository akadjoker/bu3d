#include "Terrain.hpp"

const int ETPS_9 = 9;
const int ETPS_17 = 17;
const int ETPS_33 = 33;
const int ETPS_65 = 65;
const int ETPS_129 = 129;

Vec3 Terrain::getPosition(int index)
{
    int stride = 3 + 2 + 2;
    const int baseIndex = index * stride;
    return Vec3(vertices[baseIndex], vertices[baseIndex + 1], vertices[baseIndex + 2]);
}

void Terrain::calculateDistanceThresholds()
{
    if (!OverrideDistanceThreshold)
    {
        LODDistanceThreshold.clear();
        const float size = PatchSize * PatchSize * Scale.x * Scale.z;
        for (s32 i = 0; i < MaxLOD; ++i)
        {
            float value = size * ((i + 1 + i / 2) * (i + 1 + i / 2));
            Utils::LogInfo("LOD %d: %f", i, value);
            LODDistanceThreshold.push_back(value);
        }
    }
}

void Terrain::createPatches()
{
    PatchCount = (Size - 1) / (CalcPatchSize);

    if (Patches)
        delete[] Patches;

    Patches = new SPatch[PatchCount * PatchCount];
}

void Terrain::calculatePatchNeighbors()
{

    Box.reset(getPosition(0));

    for (s32 x = 0; x < PatchCount; ++x)
    {
        for (s32 z = 0; z < PatchCount; ++z)
        {
            const s32 index = x * PatchCount + z;
            SPatch &patch = Patches[index];
            patch.CurrentLOD = 0;

            const s32 xstart = x * CalcPatchSize;
            const s32 xend = xstart + CalcPatchSize;
            const s32 zstart = z * CalcPatchSize;
            const s32 zend = zstart + CalcPatchSize;

            patch.Box.reset(getPosition(xstart * Size + zstart));

            for (s32 xx = xstart; xx <= xend; ++xx)
                for (s32 zz = zstart; zz <= zend; ++zz)
                    patch.Box.expand(getPosition(xx * Size + zz));

            Box.expand(patch.Box);

            patch.Center = patch.Box.center();

            // Assign Neighbours
            // Top
            if (x > 0)
                patch.Top = &Patches[(x - 1) * PatchCount + z];
            else
                patch.Top = 0;

            // Bottom
            if (x < PatchCount - 1)
                patch.Bottom = &Patches[(x + 1) * PatchCount + z];
            else
                patch.Bottom = 0;

            // Left
            if (z > 0)
                patch.Left = &Patches[x * PatchCount + z - 1];
            else
                patch.Left = 0;

            // Right
            if (z < PatchCount - 1)
                patch.Right = &Patches[x * PatchCount + z + 1];
            else
                patch.Right = 0;
        }
    }

    Center = Box.center();
}

bool Terrain::preRenderLODCalculations()
{
    const Vec3 cameraRotation = Driver::Instance().GetCameraRotation();
    const Vec3 cameraPosition       = Driver::Instance().GetCameraPosition();

    if( (  Abs( cameraRotation.x - OldCameraRotation.x ) < CameraRotationDelta ) && 
            ( Abs( cameraRotation.y - OldCameraRotation.y ) < CameraRotationDelta ) &&
			( Abs( cameraRotation.z - OldCameraRotation.z ) < CameraRotationDelta ) )
		{
			if( ( Abs( cameraPosition.x - OldCameraPosition.x ) < CameraMovementDelta ) && 
				( Abs( cameraPosition.y - OldCameraPosition.y ) < CameraMovementDelta ) &&
				( Abs( cameraPosition.z - OldCameraPosition.z ) < CameraMovementDelta ) )
			{

				return false;
			}
		}

        OldCameraPosition = cameraPosition;
        OldCameraRotation = cameraRotation;

    const s32 count = PatchCount * PatchCount;
    for (s32 j = 0; j < count; ++j)
    {

       // if (Driver::Instance().IsInFrustum(Patches[j].Box))
        {
            const Vec3 Center = Patches[j].Center;
            float distance =(cameraPosition.x - Center.x) * (cameraPosition.x - Center.x) + 
				            (cameraPosition.y - Center.y) * (cameraPosition.y - Center.y) + 
						    (cameraPosition.z - Center.z) * (cameraPosition.z - Center.z);

            //const f32 distance = Vec3::DistanceSquared(position, Patches[j].Center);

            Patches[j].CurrentLOD = 0;
            for (s32 i = MaxLOD - 1; i > 0; --i)
            {
                if (distance >= LODDistanceThreshold[i])
                {
                    Patches[j].CurrentLOD = i;
                    break;
                }
            }
        }
    //     else
    //     {
    //     //	Patches[j].CurrentLOD = -1;
    //     }
     }

    return true;
}

void Terrain::preRenderIndicesCalculations()
{

    indices.clear();

    s32 index = 0;
    for (s32 i = 0; i < PatchCount; ++i)
    {
        for (s32 j = 0; j < PatchCount; ++j)
        {
            if (Patches[index].CurrentLOD >= 0)
            {
                s32 x = 0;
                s32 z = 0;

                // calculate the step we take this patch, based on the patches current LOD
                const s32 step = 1 << Patches[index].CurrentLOD;

                // Loop through patch and generate indices
                while (z < CalcPatchSize)
                {
                    const s32 index11 = getIndex(j, i, index, x, z);
                    const s32 index21 = getIndex(j, i, index, x + step, z);
                    const s32 index12 = getIndex(j, i, index, x, z + step);
                    const s32 index22 = getIndex(j, i, index, x + step, z + step);

                    indices.push_back(index12);
                    indices.push_back(index11);
                    indices.push_back(index22);
                    indices.push_back(index22);
                    indices.push_back(index11);
                    indices.push_back(index21);



          
                    x += step;

                    // we've hit an edge
                    if (x >= CalcPatchSize)
                    {
                        x = 0;
                        z += step;
                    }
                }
            }
            ++index;
        }
    }
    mesh.UpdateIndexData(indices.data(), indices.size());  
}

u32 Terrain::getIndex(const s32 PatchX, const s32 PatchZ, const s32 PatchIndex, u32 vX, u32 vZ)
{
    // top border
    if (vZ == 0)
    {
        if (Patches[PatchIndex].Top &&
            Patches[PatchIndex].CurrentLOD < Patches[PatchIndex].Top->CurrentLOD &&
            (vX % (1 << Patches[PatchIndex].Top->CurrentLOD)) != 0)
        {
            vX -= vX % (1 << Patches[PatchIndex].Top->CurrentLOD);
        }
    }
    else if (vZ == (u32)CalcPatchSize) // bottom border
    {
        if (Patches[PatchIndex].Bottom &&
            Patches[PatchIndex].CurrentLOD < Patches[PatchIndex].Bottom->CurrentLOD &&
            (vX % (1 << Patches[PatchIndex].Bottom->CurrentLOD)) != 0)
        {
            vX -= vX % (1 << Patches[PatchIndex].Bottom->CurrentLOD);
        }
    }

    // left border
    if (vX == 0)
    {
        if (Patches[PatchIndex].Left &&
            Patches[PatchIndex].CurrentLOD < Patches[PatchIndex].Left->CurrentLOD &&
            (vZ % (1 << Patches[PatchIndex].Left->CurrentLOD)) != 0)
        {
            vZ -= vZ % (1 << Patches[PatchIndex].Left->CurrentLOD);
        }
    }
    else if (vX == (u32)CalcPatchSize) // right border
    {
        if (Patches[PatchIndex].Right &&
            Patches[PatchIndex].CurrentLOD < Patches[PatchIndex].Right->CurrentLOD &&
            (vZ % (1 << Patches[PatchIndex].Right->CurrentLOD)) != 0)
        {
            vZ -= vZ % (1 << Patches[PatchIndex].Right->CurrentLOD);
        }
    }

    if (vZ >= (u32)PatchSize)
        vZ = CalcPatchSize;

    if (vX >= (u32)PatchSize)
        vX = CalcPatchSize;

    return (vZ + ((CalcPatchSize) * PatchZ)) * Size + (vX + ((CalcPatchSize) * PatchX));
}

Terrain::Terrain(const Vec3 &position, const Vec3 &scale, s32 patchSize, s32 maxLOD)
{
    Position = position;
    Scale = scale;
    PatchSize = patchSize;
    MaxLOD = maxLOD;
    CalcPatchSize = patchSize - 1;
    OverrideDistanceThreshold = false;
    Patches = nullptr;
    	
    OldCameraPosition = Vec3(-99999.9, -99999.9, -99999.9 );
    OldCameraRotation = Vec3(0.0, 0.0, 0.0);
    CameraMovementDelta = 1.0f;
    CameraRotationDelta = 0.5f * ToRad;
}

bool Terrain::LoadHeightmap(Heightmap &heightmap)
{

    u32 startTimer = SDL_GetTicks();
    indices.clear();
    vertices.clear();
    
    Size = heightmap.GetWidth();
    indices.reserve(Size * Size * 6);
    texturePaintScale = (Size - 1);
    textureDetailScale = 16.0f;
    for (int x = 0; x < Size; x++)
    {
        for (int z = 0; z < Size; z++)
        {

            float height = heightmap.GetInterpolatedHeight(x, z);
            Vec3 v = Vec3((float)x, height, (float)z);

            v.x = (v.x * Scale.x) + Position.x;
            v.z = (v.z * Scale.x) + Position.z;
            v.y = (v.y * Scale.x) + Position.y;


            float tx0 = (float)x / texturePaintScale;
            float ty0 = (float)z / texturePaintScale;
            float tx1 = tx0 * textureDetailScale;
            float ty1 = ty0 * textureDetailScale;

            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            vertices.push_back(tx0);
            vertices.push_back(ty0);
            vertices.push_back(tx1);
            vertices.push_back(ty1);
            indices.push_back(0);indices.push_back(0);indices.push_back(0);
            indices.push_back(0);indices.push_back(0);indices.push_back(0);

        }
    }
    lodChanged = true;
    mesh.SetVertexData(vertices.data(), vertices.size(), {3, 2, 2}, false);

    calculateDistanceThresholds();
    createPatches();
    calculatePatchNeighbors();
    
    mesh.SetIndexData(indices.data(), indices.size(), true);

    u32 endTimer = SDL_GetTicks();
    float time = (endTimer - startTimer) / 1000.0f;

    Utils::LogInfo("Generated terrain %d  loaded in %f seconds", Size, time);

    return true;
}

bool Terrain::LoadTerrain(const char *filename, float heightScale)
{
    Pixmap heightMap;

    u32 startTimer = SDL_GetTicks();

    if (!heightMap.Load(filename))
    {
        return false;
    }
    Size = heightMap.width;
    texturePaintScale = (Size - 1);
    textureDetailScale = 16.0f;
    indices.clear();
    vertices.clear();
    indices.reserve(Size * Size * 6);
    for (int x = 0; x < Size; x++)
    {
        for (int z = 0; z < Size; z++)
        {
            u32 color = heightMap.GetPixel(x, z);
            float r = (float)((color >> 16) & 0xFF) / 255.0f;
            float g = (float)((color >> 8) & 0xFF) / 255.0f;
            float b = (float)((color >> 0) & 0xFF) / 255.0f;
            float height = (r + g + b) / 3.0f * heightScale;
            Vec3 v = Vec3((float)x, height, (float)z);

            v.x = (v.x * Scale.x) + Position.x;
            v.z = (v.z * Scale.x) + Position.z;
            v.y = (v.y * Scale.x) + Position.y;

            float tx0 = (float)x / texturePaintScale;
            float ty0 = (float)z / texturePaintScale;
            float tx1 = tx0 * textureDetailScale;
            float ty1 = ty0 * textureDetailScale;

            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            vertices.push_back(tx0);
            vertices.push_back(ty0);
            vertices.push_back(tx1);
            vertices.push_back(ty1);
            indices.push_back(0);indices.push_back(0);indices.push_back(0);
            indices.push_back(0);indices.push_back(0);indices.push_back(0);
        }
    }

    mesh.SetVertexData(vertices.data(), vertices.size(), {3, 2, 2}, false);

    calculateDistanceThresholds();
    createPatches();
    calculatePatchNeighbors();

    mesh.SetIndexData(indices.data(), indices.size(), true);


    u32 endTimer = SDL_GetTicks();
    float time = (endTimer - startTimer) / 1000.0f;

    Utils::LogInfo("Generated terrain %d  loaded in %f seconds", Size, time);

    return true;
}

void Terrain::Render()
{

    if (!Driver::Instance().IsInFrustum(Box))
    {
        return;
    }

    lodChanged = false;
    if (preRenderLODCalculations())
    {
        preRenderIndicesCalculations();
            
    }


    




    mesh.Render(GL_TRIANGLES);
}

void Terrain::Debug(RenderBatch *batch)
{

    batch->SetColor(1.0f, 0.0f, 0.0f);
    batch->Box(Box.min, Box.max);
    if ((PatchCount * PatchCount) < 0)
        return;

    batch->SetColor(0.0f, 1.0f, 0.0f);
    for (int x = 0; x < (PatchCount * PatchCount); x++)
    {
        switch (Patches[x].CurrentLOD)
        {
        case 0:
            batch->SetColor(0.0f, 1.0f, 0.0f);
            break;
        case 1:
            batch->SetColor(1.0f, 0.0f, 0.0f);
            break;
        case 2:
            batch->SetColor(0.0f, 0.0f, 1.0f);
            break;
        case 3:
            batch->SetColor(1.0f, 1.0f, 0.0f);
            break;
        case 4:
            batch->SetColor(1.0f, 0.0f, 1.0f);
            break;
        case 5:
            batch->SetColor(0.0f, 1.0f, 1.0f);
            break;
        default:
            batch->SetColor(1.0f, 1.0f, 1.0f);
            break;
        }
        batch->Box(Patches[x].Box.min, Patches[x].Box.max);
        batch->Cube(Patches[x].Center, 1.5, 1.5, 1.5, false);
    }
}

void Terrain::Release()
{
    mesh.Release();
}

Terrain::~Terrain()
{
    if (Patches)
        delete[] Patches;
}
void Terrain::SetPaintScale(float scale)
{
    texturePaintScale = scale;
}

void Terrain::SetDetailScale(float scale)
{
    textureDetailScale = scale;
}