#include "Mesh.hpp"


MeshBuffer::MeshBuffer() : vbo(0), ebo(0), vao(0), vertexCount(0), indexCount(0)
{
}

void MeshBuffer::Init()
{
    glGenVertexArrays(1, &vao);
}

void MeshBuffer::Release()
{
    glDeleteVertexArrays(1, &vao);
    if (vbo != 0)
        glDeleteBuffers(1, &vbo);
    if (ebo != 0)
        glDeleteBuffers(1, &ebo);
}

void MeshBuffer::Bind()
{
    if (vao == 0)
        Init();
    glBindVertexArray(vao);
}

void MeshBuffer::UnBind()
{
    glBindVertexArray(0);
}

void MeshBuffer::SetVertexData(const float *vertices, u32 count, const std::vector<GLint> &attribSizes, bool dynamic)
{
    vertexCount = count;

    if (vbo == 0)
        glGenBuffers(1, &vbo);
    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    size_t stride = 0;
    size_t offset = 0;
    for (GLint size : attribSizes)
    {
        stride += size * sizeof(float);
    }
    for (size_t i = 0; i < attribSizes.size(); i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, stride, (void *)offset);
        offset += attribSizes[i] * sizeof(float);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshBuffer::UpdateVertexData(const float *vertices, u32 count)
{
    vertexCount = count;
    if (vbo == 0)
        glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(float), vertices);
}

void MeshBuffer::UpdateIndexData(const u32 *indices, u32 count)
{
    indexCount = count;
    if (ebo == 0)
        glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(u32), indices);
}

void MeshBuffer::SetIndexData(const u32 *indices, u32 count, bool dynamic)
{
    indexCount = count;
    if (ebo == 0)
        glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void MeshBuffer::Render(int mode)
{
    Bind();
    if (indexCount == 0)
        Driver::Instance().DrawArrays(mode, 0, vertexCount);
    else
        Driver::Instance().DrawElements(mode, indexCount, GL_UNSIGNED_INT, 0);
    UnBind();
}

Mesh::Mesh(bool dynamic, bool facesDynamic) : isDynamic(dynamic), isFacesDynamic(facesDynamic), isInitialized(false)
{
    EBO = 0;
    VAO = 0;
    VBO[0] = 0;
    VBO[1] = 0;
    VBO[2] = 0;
    VBO[3] = 0;
    VBO[4] = 0;
    flags = 1 | 2 | 4 | 8 | 16 | 32;
    material = 0;
}

Mesh::~Mesh()
{
    EBO = 0;
    VAO = 0;
    VBO[0] = 0; // vertices
    VBO[1] = 0; // texcoords
    VBO[2] = 0; // normals
    VBO[3] = 0; // tangents
    VBO[4] = 0; // bitangents

    VBO[5] = 0; // weights
    VBO[6] = 0; // joints
}

void Mesh::Release()
{

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO[0] != 0)
        glDeleteBuffers(1, &VBO[0]);
    if (VBO[1] != 0)
        glDeleteBuffers(1, &VBO[1]);
    if (VBO[2] != 0)
        glDeleteBuffers(1, &VBO[2]);
    if (VBO[3] != 0)
        glDeleteBuffers(1, &VBO[3]);
    if (VBO[4] != 0)
        glDeleteBuffers(1, &VBO[4]);
    if (VBO[5] != 0)
        glDeleteBuffers(1, &VBO[5]);
    if (VBO[6] != 0)
        glDeleteBuffers(1, &VBO[6]);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
}

void Mesh::Init()
{
    if (isInitialized)
        return;
    isInitialized = true;

    if (VAO == 0)
        glGenVertexArrays(1, &VAO);

    if (VBO[0] == 0)
        glGenBuffers(1, &VBO[0]);

    if (texcoords.size() > 0 && VBO[1] == 0)
        glGenBuffers(1, &VBO[1]);

    if (normals.size() > 0 && VBO[2] == 0)
        glGenBuffers(1, &VBO[2]);

    if (tangents.size() > 0 && VBO[3] == 0)
        glGenBuffers(1, &VBO[3]);

    if (bitangents.size() > 0 && VBO[4] == 0)
        glGenBuffers(1, &VBO[4]);

    if (weights.size() > 0 && VBO[5] == 0)
        glGenBuffers(1, &VBO[5]);

    if (joints.size() > 0 && VBO[6] == 0)
        glGenBuffers(1, &VBO[6]);

    if (indices.size() > 0 && EBO == 0)
        glGenBuffers(1, &EBO);

    GLenum usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    GLenum indexUsage = isFacesDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3), vertices.data(), usage);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (texcoords.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(Vec2), texcoords.data(), usage);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }

    if (normals.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3), normals.data(), usage);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
    }

    if (tangents.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(Vec3), tangents.data(), usage);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(3);
    }

    if (bitangents.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(Vec3), bitangents.data(), usage);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(4);
    }

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), indexUsage);
    }

    if (weights.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(Vec4), weights.data(), usage);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(5);
    }

    if (joints.size() > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
        glBufferData(GL_ARRAY_BUFFER, joints.size() * sizeof(Vec4), joints.data(), usage);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(6);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Update()
{

    if (!isInitialized)
        Init();
    if (!NeedsUpdate())
        return;

    if (isDynamic)
    {
        if (vertices.size() > 0 && (flags & 1))
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vec3), vertices.data());
            flags &= ~1;
        }

        if (texcoords.size() > 0 && (flags & 2))
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, texcoords.size() * sizeof(Vec2), texcoords.data());
            flags &= ~2;
        }

        if (normals.size() > 0 && (flags & 4))
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(Vec3), normals.data());
            flags &= ~4;
        }

        if (tangents.size() > 0 && (flags & 8))
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, tangents.size() * sizeof(Vec3), tangents.data());

            flags &= ~8;
        }

        if (bitangents.size() > 0 && (flags & 16))
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, bitangents.size() * sizeof(Vec3), bitangents.data());

            flags &= ~16;
        }
    }

    if (weights.size() > 0 && (flags & 64))
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, weights.size() * sizeof(Vec4), weights.data());

        flags &= ~64;
    }

    if (joints.size() > 0 && (flags & 128))
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, joints.size() * sizeof(Vec4), joints.data());

        flags &= ~128;
    }

    if (isFacesDynamic)
    {
        if (indices.size() > 0 && (flags & 32))
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(u32), indices.data());

            flags &= ~32;
        }
    }
}

void Mesh::Render(u32 mode, u32 start, u32 count)
{
    if (!isInitialized)
        Init();

    if (NeedsUpdate())
    {
        Update();
    }
    glBindVertexArray(VAO);

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        Driver::Instance().DrawElements(mode, count, GL_UNSIGNED_INT, (void *)(start * sizeof(u32)));
    }
    else
    {
        Driver::Instance().DrawArrays(mode, start, count);
    }

    glBindVertexArray(0);
}

void Mesh::Render(u32 mode, u32 count)
{
    Render(mode, 0, count);
}

void Mesh::Render(u32 mode)
{
    u32 count = (indices.size() > 0) ? indices.size() : vertices.size();
    Render(mode, 0, count);
}

u32 Mesh::AddVertex(const Vec3 &pos)
{
    vertices.push_back(pos);
    SetFlag(1);
    return vertices.size() - 1;
}

u32 Mesh::AddVertex(float x, float y, float z)
{
    return AddVertex(Vec3(x, y, z));
}

u32 Mesh::Addface(u32 a, u32 b, u32 c)
{
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
    SetFlag(32);
    return indices.size() - 3;
}

void Mesh::AddMesh(Mesh *mesh)
{
    if (!mesh)
        return;

    u32 offset = vertices.size();

    u8 flags = 0;

    if (!mesh->vertices.empty())
    {
        for (u32 i = 0; i < mesh->vertices.size(); i++)
        {
            Vec3 vertex = mesh->vertices[i];
            vertices.push_back(vertex);
            if (!mesh->texcoords.empty())
            {
                Vec2 texcoord = mesh->texcoords[i];
                texcoords.push_back(texcoord);
            }
            if (!mesh->normals.empty())
            {
                Vec3 normal = mesh->normals[i];
                normals.push_back(normal);
            }
            if (!mesh->tangents.empty())
            {
                Vec3 tangent = mesh->tangents[i];
                tangents.push_back(tangent);
            }
            if (!mesh->bitangents.empty())
            {
                Vec3 bitangent = mesh->bitangents[i];
                bitangents.push_back(bitangent);
            }
           
        }
    
    }

    flags |= POSITION;
    if (!mesh->texcoords.empty())
        flags |= TEXCOORD;
    if (!mesh->normals.empty())
        flags |= NORMAL;
    if (!mesh->tangents.empty())
        flags |= TANGENT;
    if (!mesh->bitangents.empty())
        flags |= BITANGENT;

    if (!mesh->indices.empty())
    {
        for (u32 index : mesh->indices)
        {
            indices.push_back(index + offset);
        }
        flags |= INDICES;
    }

    SetFlag(flags);
} 

void Mesh::AddMesh(Mesh *mesh, const Mat4 &transform)
{

    if (!mesh)
        return;

    u32 offset = vertices.size();

    u8 flags = 0;

    if (!mesh->vertices.empty())
    {
        for (u32 i = 0; i < mesh->vertices.size(); i++)
        {
            Vec3 vertex = mesh->vertices[i];
            transform.transformPoint(vertex);
            vertices.push_back(vertex);
            if (!mesh->texcoords.empty())
            {
                Vec2 texcoord = mesh->texcoords[i];
                texcoords.push_back(texcoord);
            }
            if (!mesh->normals.empty())
            {
                Vec3 normal = mesh->normals[i];
                transform.transformNormal(normal);
                normals.push_back(normal);
            }
            if (!mesh->tangents.empty())
            {
                Vec3 tangent = mesh->tangents[i];
                transform.transformNormal(tangent);
                tangents.push_back(tangent);
            }
            if (!mesh->bitangents.empty())
            {
                Vec3 bitangent = mesh->bitangents[i];
                transform.transformNormal(bitangent);
                bitangents.push_back(bitangent);
            }
           
        }
    
    }

    flags |= POSITION;
    if (!mesh->texcoords.empty())
        flags |= TEXCOORD;
    if (!mesh->normals.empty())
        flags |= NORMAL;
    if (!mesh->tangents.empty())
        flags |= TANGENT;
    if (!mesh->bitangents.empty())
        flags |= BITANGENT;

    if (!mesh->indices.empty())
    {
        for (u32 index : mesh->indices)
        {
            indices.push_back(index + offset);
        }
        flags |= INDICES;
    }

    SetFlag(flags);
}

MeshManager *MeshManager::instance = nullptr;

MeshManager *MeshManager::InstancePtr()
{
    if (instance == nullptr)
        instance = new MeshManager();
    return instance;
}

MeshManager &MeshManager::Instance()
{
    if (instance == nullptr)
        instance = new MeshManager();
    return *instance;
}

Mesh *MeshManager::CreateMesh()
{
    Mesh *mesh = new Mesh();
    meshes.push_back(mesh);
    return mesh;
}

void MeshManager::Release()
{
    for (Mesh *mesh : meshes)
    {
        mesh->Release();
        delete mesh;
    }

    meshes.clear();
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

void MeshManager::Init()
{
    if (!instance)
        instance = new MeshManager();
}

void MeshManager::CalculateNormals(Mesh *mesh)
{
    mesh->normals.resize(mesh->vertices.size(), Vec3(0.0f, 0.0f, 0.0f));

    for (size_t i = 0; i < mesh->indices.size(); i += 3)
    {
        u32 index0 = mesh->indices[i];
        u32 index1 = mesh->indices[i + 1];
        u32 index2 = mesh->indices[i + 2];

        const Vec3 &v0 = mesh->vertices[index0];
        const Vec3 &v1 = mesh->vertices[index1];
        const Vec3 &v2 = mesh->vertices[index2];

        // Calcula os vetores das arestas do triângulo
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;

        Vec3 normal = edge1.cross(edge2);
        normal.normalize(); // Normaliza a normal

        mesh->normals[index0] += normal;
        mesh->normals[index1] += normal;
        mesh->normals[index2] += normal;
    }

    for (Vec3 &normal : mesh->normals)
    {
        normal.normalize();
    }
    mesh->SetFlag(NORMAL);
}

static float DistanceFromSq(const Vec3 &v1, const Vec3 &v2)
{
Vec3 v = v1 - v2;
return v.x * v.x + v.y * v.y + v.z * v.z;
}

static Vec3 GetAngleWeights(const Vec3 &v, const Vec3 &v1, const Vec3 &v2)
{
        float a = DistanceFromSq(v1, v2);
		float asqrt = sqrtf(a);
		
		float b = DistanceFromSq(v, v1);
		float bsqrt = sqrtf(b);
		
		float c = DistanceFromSq(v, v2);
		float csqrt = sqrtf(c);

		return Vec3(
		acosf((b + c - a) / (2.f * bsqrt * csqrt)),
		acosf((-b + c + a) / (2.f * asqrt * csqrt)),
		acosf((b - c + a) / (2.f * bsqrt * asqrt)));

	
    
}

void MeshManager::CalculateSmothNormals(Mesh *mesh, bool angleWeighted)
{
    if (!mesh) return;
    if (mesh->vertices.empty()) return;
    if (mesh->indices.empty()) return;

    mesh->normals.resize(mesh->vertices.size(), Vec3(0.0f, 0.0f, 0.0f));

    for (size_t i = 0; i < mesh->indices.size(); i += 3)
    {
       Vec3 v0 = mesh->vertices[mesh->indices[i]];
       Vec3 v1 = mesh->vertices[mesh->indices[i + 1]];
       Vec3 v2 = mesh->vertices[mesh->indices[i + 2]];

       Vec3 normal = Vec3::Normalize(Vec3::Cross(v1 - v0, v2 - v0));

        Vec3 weight(1.f, 1.f, 1.f);
        if (angleWeighted)
        {
            weight = GetAngleWeights(v0, v1, v2);
        }

        mesh->normals[mesh->indices[i]] += normal * weight.x;   
        mesh->normals[mesh->indices[i + 1]] += normal * weight.y;
        mesh->normals[mesh->indices[i + 2]] += normal * weight.z;
    }

    for (Vec3 &normal : mesh->normals)
    {
        normal = Vec3::Normalize(normal);
    }

    mesh->SetFlag(NORMAL);


}

void MeshManager::GetBoundingBox(Mesh *mesh, Vec3 &min, Vec3 &max)
{
    min.x = 1000000000;
    min.y = 1000000000;
    min.z = 1000000000;
    max.x = -1000000000;
    max.y = -1000000000;
    max.z = -1000000000;
    for (Vec3 &vertex : mesh->vertices)
    {
        min.x = Min(min.x, vertex.x);
        min.y = Min(min.y, vertex.y);
        min.z = Min(min.z, vertex.z);
        max.x = Max(max.x, vertex.x);
        max.y = Max(max.y, vertex.y);
        max.z = Max(max.z, vertex.z);
    }
}

void MeshManager::TranslateMesh(Mesh *mesh, float x, float y, float z)
{
    for (Vec3 &vertex : mesh->vertices)
    {
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
    }

    mesh->SetFlag(POSITION);

}

void MeshManager::ScaleMesh(Mesh *mesh, float x, float y, float z)
{
    for (Vec3 &vertex : mesh->vertices)
    {
        vertex.x *= x;
        vertex.y *= y;
        vertex.z *= z;
    }

    mesh->SetFlag(POSITION);    
}

void MeshManager::RotateMesh(Mesh *mesh, const Vec3 &axis,float angle)
{

    float angleInRadians =  angle * Pi / 180.0f;

    Quat q(axis, angleInRadians);
    Mat4 rotationMatrix = q.toMat4();

    u8 flags = 0;

    for (u32 i = 0; i < mesh->vertices.size(); i++)
    {
        Vec3 vertex = mesh->vertices[i];
        flags |= POSITION;
        if (!mesh->normals.empty())
        {
            Vec3 normal = mesh->normals[i];
            rotationMatrix.transformNormal(normal);
            mesh->normals[i] = normal;
            flags |= NORMAL;
        }
        if (!mesh->tangents.empty())
        {
            Vec3 tangent = mesh->tangents[i];
            rotationMatrix.transformNormal(tangent);
            mesh->tangents[i] = tangent;
            flags |= TANGENT;
        }
        if (!mesh->bitangents.empty())
        {
            Vec3 bitangent = mesh->bitangents[i];
            rotationMatrix.transformNormal(bitangent);
            mesh->bitangents[i] = bitangent;
            flags |= BITANGENT;
        }
        rotationMatrix.transformPoint(vertex);
        mesh->vertices[i] = vertex;
    }

    mesh->SetFlag(flags);
}

void MeshManager::TexturePlanarMapping(Mesh *mesh, float resolution)
{
    if (!mesh) return;
    if (mesh->vertices.empty()) return;
    if (mesh->indices.empty()) return;

    const u32 idxcnt = mesh->indices.size();

    Vec2 *texCoords =  mesh->texcoords.data();
    const Vec3 *positions = mesh->vertices.data();
    const u32  *indices    =  mesh->indices.data();

    for (u32 i = 0; i < idxcnt; i += 3)
    {
        Plane plane(mesh->vertices[mesh->indices[i]], mesh->vertices[mesh->indices[i + 1]], mesh->vertices[mesh->indices[i + 2]]);
        Vec3 normal = plane.normal;
        normal.x = fabs(normal.x);
        normal.y = fabs(normal.y);
        normal.z = fabs(normal.z);


        if (normal.x > normal.y && normal.x > normal.z) 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].y * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].z * resolution;
            }
        } else 
        if (normal.y > normal.x && normal.y > normal.z) 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].x * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].z * resolution;
            }
        } else 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].x * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].y * resolution;
            }
        }         
          

    }

    mesh->SetFlag(TEXCOORD);

}

void MeshManager::TexturePlanarMapping(Mesh *mesh, float resolutionS, float resolutionT, u8 axis, const Vec3 &offset)
{
      if (!mesh) return;
    if (mesh->vertices.empty()) return;
    if (mesh->indices.empty()) return;

    const u32 idxcnt = mesh->indices.size();

    Vec2 *texCoords =  mesh->texcoords.data();
    const Vec3 *positions = mesh->vertices.data();
    const u32  *indices    =  mesh->indices.data();

    for (u32 i = 0; i < idxcnt; i += 3)
    {
         if (axis==0)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].z + offset.z) * resolutionS;
                    texCoords[indices[i + j]].y =0.5f -(positions[indices[i + j]].y + offset.y) * resolutionT;
                

                }
        } else if (axis==1)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].x + offset.x) * resolutionS;
                    texCoords[indices[i + j]].y =1.0f -(positions[indices[i + j]].z + offset.z) * resolutionT;
                }
        } else if (axis==2)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].x + offset.x) * resolutionS;
                    texCoords[indices[i + j]].y =0.5f -(positions[indices[i + j]].y + offset.y) * resolutionT;
                }
        }

     
    }

    mesh->SetFlag(TEXCOORD);
}

void MeshManager::FlipFaces(Mesh *mesh)
{
    if (!mesh) return;
    if (mesh->indices.empty()) return;
    const u32 idxcnt = mesh->indices.size();
    u32 *indices = mesh->indices.data();
    for (u32 i = 0; i < idxcnt; i += 3)
    {
        u32 tmp = indices[i + 1];
        indices[i + 1] = indices[i + 2];
        indices[i + 2] = tmp;
    }
    mesh->SetFlag(INDICES);
}

void MeshManager::FlipNormals(Mesh *mesh)
{
    if (!mesh) return;
    
    if (mesh->normals.empty()) return;
    u32 idxcnt = mesh->normals.size();
    Vec3 *normals = mesh->normals.data();
    for (u32 i = 0; i < idxcnt; i++)
    {
        normals[i] = -normals[i];
    }
    mesh->SetFlag(NORMAL);
}

Mesh *MeshManager::CreateBox(float width, float height, float length)
{
    Mesh *mesh = new Mesh();

    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float hl = length / 2.0f;

    Vec3 positions[] =
        {
            // Face frontal
            {-hw, -hh, -hl},
            {hw, -hh, -hl},
            {hw, hh, -hl},
            {-hw, hh, -hl},
            // Face traseira
            {hw, -hh, hl},
            {-hw, -hh, hl},
            {-hw, hh, hl},
            {hw, hh, hl},
            // Face superior
            {-hw, hh, -hl},
            {hw, hh, -hl},
            {hw, hh, hl},
            {-hw, hh, hl},
            // Face inferior
            {-hw, -hh, hl},
            {hw, -hh, hl},
            {hw, -hh, -hl},
            {-hw, -hh, -hl},
            // Face direita
            {hw, -hh, -hl},
            {hw, -hh, hl},
            {hw, hh, hl},
            {hw, hh, -hl},
            // Face esquerda
            {-hw, -hh, hl},
            {-hw, -hh, -hl},
            {-hw, hh, -hl},
            {-hw, hh, hl},
        };

    Vec3 normals[] =
        {

            {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, // Frontal
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1}, // Traseira
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0}, // Superior
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0}, // Inferior
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0}, // Direita
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0}, // Esquerda
        };

    Vec2 texCoords[] =
        {

            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
        };

    u32 boxIndices[] =
        {

            0, 1, 2, 2, 3, 0,       // Frontal
            4, 5, 6, 6, 7, 4,       // Traseira
            8, 9, 10, 10, 11, 8,    // Superior
            12, 13, 14, 14, 15, 12, // Inferior
            16, 17, 18, 18, 19, 16, // Direita
            20, 21, 22, 22, 23, 20  // Esquerda
        };

    for (int i = 0; i < 24; ++i)
    {
        Vec2 uv = texCoords[i];
        Vec3 position = positions[i];

        float u = uv.x;
        float v = uv.y;

        uv.x = 1.0f - u;
        uv.y = 1.0f - v;

        mesh->texcoords.push_back(uv);
        mesh->normals.push_back(normals[i]);
        mesh->vertices.push_back(position);
    }

    for (int i = 0; i < (36 / 3); ++i)
    {
        u32 a = boxIndices[i * 3 + 0];
        u32 b = boxIndices[i * 3 + 1];
        u32 c = boxIndices[i * 3 + 2];

        mesh->indices.push_back(c);
        mesh->indices.push_back(b);
        mesh->indices.push_back(a);
    }

    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);

    meshes.push_back(mesh);

    return mesh;
}
Mesh *MeshManager::CreateSphere(float diameter, int rows, int columns)
{

    Mesh *mesh = new Mesh();

    if (rows < 2)
        rows = 2;
    if (columns < 3)
        columns = 3;

    float radius = diameter / 2.0f;

    float fSegY = Pi / rows;       // Segmento de latitude
    float fSegX = TwoPi / columns; // Segmento de longitude

    for (int j = 0; j <= rows; ++j)
    {
        float fSY = SinRad(fSegY * j);
        float fCY = CosRad(fSegY * j);

        for (int i = 0; i <= columns; ++i)
        {
            float fSX = SinRad(-fSegX * i);
            float fCX = CosRad(-fSegX * i);

            Vec3 vertex(
                fSX * fSY * radius, // x
                fCY * radius,       // y
                fCX * fSY * radius  // z
            );
            mesh->vertices.push_back(vertex);

            Vec3 normal = vertex / radius;
            normal.normalize();
            mesh->normals.push_back(normal);

            float u = 1.0f - (float)i / columns;
            float v = (float)j / rows;

            mesh->texcoords.push_back(Vec2(u, v));
        }
    }

    for (int i = 0; i < columns; ++i)
    {
        int next = (i + 1) % (columns + 1);
        mesh->indices.push_back(i);
        mesh->indices.push_back(columns + 1 + next);
        mesh->indices.push_back(columns + 1 + i);
    }

    // Meio da esfera
    for (int j = 1; j < rows - 1; ++j)
    {
        for (int i = 0; i < columns; ++i)
        {
            int next = (i + 1) % (columns + 1);

            // Primeiro triângulo
            mesh->indices.push_back((columns + 1) * j + i);
            mesh->indices.push_back((columns + 1) * j + next);
            mesh->indices.push_back((columns + 1) * (j + 1) + i);

            // Segundo triângulo
            mesh->indices.push_back((columns + 1) * j + next);
            mesh->indices.push_back((columns + 1) * (j + 1) + next);
            mesh->indices.push_back((columns + 1) * (j + 1) + i);
        }
    }

    // Base da esfera
    for (int i = 0; i < columns; ++i)
    {
        int next = (i + 1) % (columns + 1);

        mesh->indices.push_back((columns + 1) * (rows - 1) + i);
        mesh->indices.push_back((columns + 1) * (rows - 1) + next);
        mesh->indices.push_back((columns + 1) * rows + i);
    }

    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);

    meshes.push_back(mesh);

    return mesh;
}

Mesh *MeshManager::CreateCone(float height, float diameter, int segments)
{

    if (segments < 3)
        segments = 3;
    if (diameter < 0)
        diameter = -diameter;

    float radius = diameter / 2.0f;

    Mesh *mesh = new Mesh();

    int faces = segments * 2;
    int vertexCount = faces * 3;
    int indexCount = segments * 6;

    mesh->vertices.reserve(vertexCount);
    mesh->normals.reserve(vertexCount);
    mesh->texcoords.reserve(vertexCount);
    mesh->indices.reserve(indexCount);

    float segmentAngle = TwoPi / segments;

    for (int i = 0; i < segments; i++)
    {
        int next = (i + 1) % segments;

        // Vértice no topo do cone
        mesh->vertices.push_back(Vec3(0.0f, height / 2.0f, 0.0f));
        mesh->normals.push_back(Vec3(0.0f, 1.0f, 0.0f));
        mesh->texcoords.push_back(Vec2(0.5f, 0.5f));

        mesh->vertices.push_back(Vec3(
            SinRad(-segmentAngle * i) * radius,
            -height / 2.0f,
            CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(
                                    SinRad(-segmentAngle * i),
                                    radius / height,
                                    CosRad(-segmentAngle * i))
                                    .normalize());
        float u = SinRad(-segmentAngle * i) * 0.5f + 0.5f;
        float v = CosRad(-segmentAngle * i) * 0.5f + 0.5f;

        mesh->texcoords.push_back(Vec2(u, v));

        mesh->vertices.push_back(Vec3(
            SinRad(-segmentAngle * next) * radius,
            -height / 2.0f,
            CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(
                                    SinRad(-segmentAngle * next),
                                    radius / height,
                                    CosRad(-segmentAngle * next))
                                    .normalize());

        u = SinRad(-segmentAngle * next) * 0.5f + 0.5f;
        v = CosRad(-segmentAngle * next) * 0.5f + 0.5f;

        mesh->texcoords.push_back(Vec2(u, v));

        int baseIndex = i * 3;
        mesh->indices.push_back(baseIndex);
        mesh->indices.push_back(baseIndex + 2);
        mesh->indices.push_back(baseIndex + 1);
    }

    for (int i = 0; i < segments; i++)
    {
        int next = (i + 1) % segments;

        mesh->vertices.push_back(Vec3(
            SinRad(-segmentAngle * i) * radius,
            -height / 2.0f,
            CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(0.0f, -1.0f, 0.0f));

        float u = SinRad(-segmentAngle * i) * 0.5f + 0.5f;
        float v = CosRad(-segmentAngle * i) * 0.5f + 0.5f;

        mesh->texcoords.push_back(Vec2(v, u));

        mesh->vertices.push_back(Vec3(0.0f, -height / 2.0f, 0.0f));
        mesh->normals.push_back(Vec3(0.0f, -1.0f, 0.0f));
        mesh->texcoords.push_back(Vec2(0.5f, 0.5f));

        mesh->vertices.push_back(Vec3(
            SinRad(-segmentAngle * next) * radius,
            -height / 2.0f,
            CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(0.0f, -1.0f, 0.0f));

        u = SinRad(-segmentAngle * next) * 0.5f + 0.5f;
        v = CosRad(-segmentAngle * next) * 0.5f + 0.5f;

        mesh->texcoords.push_back(Vec2(v, u));

        // Adiciona índices para o triângulo da base
        int baseIndex = segments * 3 + i * 3;
        mesh->indices.push_back(baseIndex);
        mesh->indices.push_back(baseIndex + 2);
        mesh->indices.push_back(baseIndex + 1);
    }

    // Normaliza os vetores normais
    for (auto &normal : mesh->normals)
    {
        normal.normalize();
    }
    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);

    meshes.push_back(mesh);

    return mesh;
}
Mesh *MeshManager::CreateCylinder(float height, float diameter, int segments)
{
    if (segments < 3)
        segments = 3;
    if (diameter < 0)
        diameter = -diameter;

    float radius = diameter / 2.0f;

    Mesh *mesh = new Mesh();

    float segmentAngle = TwoPi / segments;
    float segmentU = 1.0f / segments;
    float normY = (height > 0) ? 1.0f : -1.0f;

    std::vector<u32> indices;

    int vertexIndex = 0;

    // Top face
    for (int i = 0; i < segments; i++)
    {
        int next = (i + 1) % segments;

        // Centro do topo
        mesh->vertices.push_back(Vec3(0, height / 2.0f, 0));
        mesh->normals.push_back(Vec3(0, normY, 0));
        mesh->texcoords.push_back(Vec2(0.5f, 0.5f));

        // Borda do topo
        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * i) * radius, height / 2.0f, CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(0, normY, 0));
        mesh->texcoords.push_back(Vec2(SinRad(-segmentAngle * i) / 2.0f + 0.5f, CosRad(-segmentAngle * i) / 2.0f + 0.5f));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * next) * radius, height / 2.0f, CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(0, normY, 0));
        mesh->texcoords.push_back(Vec2(SinRad(-segmentAngle * next) / 2.0f + 0.5f, CosRad(-segmentAngle * next) / 2.0f + 0.5f));

        u32 a = vertexIndex++;
        u32 b = vertexIndex++;
        u32 c = vertexIndex++;
        mesh->indices.push_back(c);
        mesh->indices.push_back(b);
        mesh->indices.push_back(a);
    }

    // Middle part
    for (int i = 0; i < segments; i++)
    {
        int next = i + 1;
        if (next >= segments)
            next = 0;

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * i) * radius, height / 2.0f, CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * i), 0, CosRad(-segmentAngle * i)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * i, 0));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * i) * radius, -height / 2.0f, CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * i), 0, CosRad(-segmentAngle * i)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * i, 1));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * next) * radius, -height / 2.0f, CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * next), 0, CosRad(-segmentAngle * next)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * (i + 1), 1));

        u32 a = vertexIndex++;
        u32 b = vertexIndex++;
        u32 c = vertexIndex++;
        mesh->indices.push_back(c);
        mesh->indices.push_back(b);
        mesh->indices.push_back(a);

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * i) * radius, height / 2.0f, CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * i), 0, CosRad(-segmentAngle * i)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * i, 0));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * next) * radius, -height / 2.0f, CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * next), 0, CosRad(-segmentAngle * next)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * (i + 1), 1));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * next) * radius, height / 2.0f, CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(SinRad(-segmentAngle * next), 0, CosRad(-segmentAngle * next)).normalize());
        mesh->texcoords.push_back(Vec2(segmentU * (i + 1), 0));

        a = vertexIndex++;
        b = vertexIndex++;
        c = vertexIndex++;
        mesh->indices.push_back(c);
        mesh->indices.push_back(b);
        mesh->indices.push_back(a);
    }

    // Bottom face
    for (int i = 0; i < segments; i++)
    {
        int next = (i + 1) % segments;

        // Borda inferior
        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * i) * radius, -height / 2.0f, CosRad(-segmentAngle * i) * radius));
        mesh->normals.push_back(Vec3(0, -normY, 0));
        mesh->texcoords.push_back(Vec2(SinRad(-segmentAngle * i) / 2.0f + 0.5f, CosRad(-segmentAngle * i) / 2.0f + 0.5f));

        // Centro do fundo
        mesh->vertices.push_back(Vec3(0, -height / 2.0f, 0));
        mesh->normals.push_back(Vec3(0, -normY, 0));
        mesh->texcoords.push_back(Vec2(0.5f, 0.5f));

        mesh->vertices.push_back(Vec3(SinRad(-segmentAngle * next) * radius, -height / 2.0f, CosRad(-segmentAngle * next) * radius));
        mesh->normals.push_back(Vec3(0, -normY, 0));
        mesh->texcoords.push_back(Vec2(SinRad(-segmentAngle * next) / 2.0f + 0.5f, CosRad(-segmentAngle * next) / 2.0f + 0.5f));

        u32 a = vertexIndex++;
        u32 b = vertexIndex++;
        u32 c = vertexIndex++;
        mesh->indices.push_back(c);
        mesh->indices.push_back(b);
        mesh->indices.push_back(a);
    }

    // Corrige normais se altura for negativa
    if (height < 0)
    {
        for (auto &normal : mesh->normals)
        {
            normal = -normal;
        }
    }

    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);

    meshes.push_back(mesh);

    return mesh;
}

Mesh *MeshManager::CreatePlane(int stacks, int slices, int tilesX, int tilesY)
{

    Mesh *mesh = new Mesh();
    mesh->SetFlag(1);
    mesh->SetFlag(2);
    mesh->SetFlag(4);
    mesh->SetFlag(32);

    Vec3 center(-MaxInt32, 0, -MaxInt32);

    for (int i = 0; i <= stacks; ++i)
    {
        float y = static_cast<float>(i) / static_cast<float>(stacks) * tilesY;
        for (int j = 0; j <= slices; ++j)
        {
            float x = static_cast<float>(j) / static_cast<float>(slices) * tilesX;

            if (x > center.x)
                center.x = x;
            if (y > center.z)
                center.z = y;
            if (x < center.x)
                center.x = x;
            if (y < center.z)
                center.z = y;

            mesh->vertices.push_back(Vec3(x, 0, y));
            mesh->normals.push_back(Vec3(0, 1, 0));
            mesh->texcoords.push_back(Vec2(x, y));
        }
    }

    for (u32 i = 0; i < mesh->vertices.size(); ++i)
    {
        Vec3 &v = mesh->vertices[i];
        v.x -= center.x * 0.5f;
        v.z -= center.z * 0.5f;
        v.y = 0;
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            u16 index = (slices + 1) * i + j;

            u32 a = index;
            u32 b = index + slices + 1;
            u32 c = index + slices + 2;
            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
            mesh->indices.push_back(c);

            a = index;
            b = index + slices + 2;
            c = index + 1;
            mesh->indices.push_back(c);
            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
        }
    }

    CalculateNormals(mesh);
    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);


    meshes.push_back(mesh);

    return mesh;
}

Mesh *MeshManager::CreateTorus(int stacks, int slices, float innerRadius, float outerRadius)
{

    Mesh *mesh = new Mesh();
    mesh->SetFlag(1);
    mesh->SetFlag(2);
    mesh->SetFlag(4);
    mesh->SetFlag(32);

    const float pi = 3.14159265359f;
    const float stackAngle = 2.0f * pi / static_cast<float>(stacks);
    const float sliceAngle = 2.0f * pi / static_cast<float>(slices);

    for (int i = 0; i <= stacks; ++i)
    {
        float u = static_cast<float>(i) * stackAngle;

        for (int j = 0; j <= slices; ++j)
        {
            float v = static_cast<float>(j) * sliceAngle;

            float x = (outerRadius + innerRadius * cos(v)) * cos(u);
            float y = (outerRadius + innerRadius * cos(v)) * sin(u);
            float z = innerRadius * sin(v);

            float textureU = static_cast<float>(i) / stacks;
            float textureV = static_cast<float>(j) / slices;

            mesh->vertices.push_back(Vec3(x, y, z));
            mesh->normals.push_back(Vec3(x, y, z));
            mesh->texcoords.push_back(Vec2(textureU, textureV));
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int index = (slices + 1) * i + j;

            u32 a = index;
            u32 b = index + slices + 1;
            u32 c = index + slices + 2;

            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
            mesh->indices.push_back(c);

            a = index;
            b = index + slices + 2;
            c = index + 1;

            mesh->indices.push_back(c);
            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
        }
    }

    CalculateNormals(mesh);
    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);

    meshes.push_back(mesh);

    return mesh;
}

Mesh *MeshManager::CreateCapsule(float diameter, int rows, int columns, float height)
{

    Mesh *mesh = new Mesh();
    mesh->SetFlag(1);
    mesh->SetFlag(2);
    mesh->SetFlag(4);
    mesh->SetFlag(32);

    if ( rows < 2 ) rows = 2;
	if ( columns < 3 ) columns = 3;

	float radius = diameter / 2.0f;

    float fSegY = Pi / rows;
    float fSegX = 2 * Pi / columns;

    float fSegU = 1.0f / columns;
    float fSegV = 1.0f / rows;

    height = (height - diameter) / 2.0f;

    int faces = (rows-1) * columns * 2;
	u32 numIndices = faces*3;

    mesh->indices.resize(numIndices);


    u32 count = 0;
    for (int j = 0; j <= rows; j++)
    {
        float fSY = SinRad(fSegY * j);
        float fCY = CosRad(fSegY * j);

        for (int i = 0; i <= columns; i++)
        {
            
            Vec3 point;
            Vec2 uv;

            point.x = SinRad(-fSegX * i) * fSY * radius;
            if (fCY * radius > 0)
                point.y = fCY * radius + height;
            else
                point.y = fCY * radius - height;
            point.z = CosRad(-fSegX * i) * fSY * radius;

            if (j == 0 || j == rows)
                uv.x = fSegU * i + fSegU / 2.0f;
            else
                uv.x = fSegU * i;
            uv.y = fSegV * j;

            mesh->vertices.push_back(point);
            mesh->texcoords.push_back(uv);
            Vec3 normal = Vec3(point.x, point.y, point.z) / radius;
        //    normal.normalize();
            mesh->normals.push_back(normal);
            count++;
        }
    }



	u32 countI = 0;
	// top row
	for ( int i = 0; i < columns; i++ )
	{
		int next = i+1;

		mesh->indices[ countI*3 + 2 ] = i;
		mesh->indices[ countI*3 + 1 ] = columns+1 + i;
		mesh->indices[ countI*3 + 0 ] = columns+1 + next;
		countI++;
	}

	// middle rows
	for ( int j = 1; j < rows-1; j++ )
	{
		for ( int i = 0; i < columns; i++ )
		{
			int next = i+1;

			mesh->indices[ countI*3 + 2 ] = (columns+1)*j + i;
			mesh->indices[ countI*3 + 1 ] = (columns+1)*(j+1) + i;
			mesh->indices[ countI*3 + 0 ] = (columns+1)*j + next;
			countI++;

			mesh->indices[ countI*3 + 2 ] = (columns+1)*j + next;
			mesh->indices[ countI*3 + 1 ] = (columns+1)*(j+1) + i;
			mesh->indices[ countI*3 + 0 ] = (columns+1)*(j+1) + next;
			countI++;
		}
	}

	// bottom row
	for ( int i = 0; i < columns; i++ )
	{
		int next = i+1;

		mesh->indices[ countI*3 + 2 ] = (columns+1)*(rows-1) + i;
		mesh->indices[ countI*3 + 1 ] = (columns+1)*rows + i;
		mesh->indices[ countI*3 + 0 ] = (columns+1)*(rows-1) + next;
		countI++;
	}
    CalculateNormals(mesh);
    mesh->SetFlag(POSITION | NORMAL | TEXCOORD | INDICES);
    meshes.push_back(mesh);
    return mesh;
}

