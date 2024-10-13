#pragma once
#include "Config.hpp"
#include "Math.hpp"
#include "OpenGL.h"
#include "Texture.hpp"
#include "Shader.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include "Trivial.hpp"

#define MAX_BONE_INFLUENCE 4
class MeshBuilder;

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
};

struct SkinVertex
{
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
    int bones[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

class Material
{
public:
    Material() { m_name = "Material"; }
    virtual ~Material() { Release(); }

    Material(const String &name) { m_name = name; }

    Material(const Material &) = delete;
    Material &operator=(const Material &) = delete;
    Material(Material &&) = delete;
    Material &operator=(Material &&) = delete;

    void SetName(const String &name) { m_name = name; }
    const String &GetName() const { return m_name; }

    bool operator==(const Material &m) const { return (m_name == m.m_name); }
    bool operator!=(const Material &m) const { return (m_name != m.m_name); }

    virtual void Release() {};
    virtual void Bind(Shader *shader);

private:
    friend class Mesh;
    friend class MeshBuilder;
    friend class Surface;

    String m_name;
};

class TextureMaterial : public Material
{
public:
    TextureMaterial(Texture2D *texture);
    void SetTexture(Texture2D *texture);

    TextureMaterial(const TextureMaterial &) = delete;
    TextureMaterial &operator=(const TextureMaterial &) = delete;
    TextureMaterial(TextureMaterial &&) = delete;
    TextureMaterial &operator=(TextureMaterial &&) = delete;

private:
    u32 layer;
    Texture2D *m_texture{nullptr};
    void Bind(Shader *shader);
};

class VertexFormat
{
public:
    enum Usage
    {
        POSITION = 1,
        NORMAL = 2,
        COLOR = 3,
        TANGENT = 4,
        BINORMAL = 5,
        BLENDWEIGHTS = 6,
        BLENDINDICES = 7,
        TEXCOORD0 = 8,
        TEXCOORD1 = 9,
        TEXCOORD2 = 10,
        TEXCOORD3 = 11,
        TEXCOORD4 = 12,
        TEXCOORD5 = 13,
        TEXCOORD6 = 14,
        TEXCOORD7 = 15
    };

    class Element
    {
    public:
        Usage usage;
        unsigned int size;
        Element();
        Element(Usage usage, unsigned int size);
        bool operator==(const Element &e) const;
        bool operator!=(const Element &e) const;
    };

    VertexFormat() { _vertexSize = 0; };

    VertexFormat(const Element *elements, unsigned int elementCount);
    ~VertexFormat();

    const Element &getElement(unsigned int index) const;
    unsigned int getElementCount() const;
    unsigned int getVertexSize() const;
    bool operator==(const VertexFormat &f) const;
    bool operator!=(const VertexFormat &f) const;

    VertexFormat(const VertexFormat &other)
        : _elements(other._elements), _vertexSize(other._vertexSize) {}

    VertexFormat &operator=(const VertexFormat &other)
    {
        if (this != &other)
        {
            _elements = other._elements;
            _vertexSize = other._vertexSize;
        }
        return *this;
    }

    VertexFormat(VertexFormat &&other) noexcept
        : _elements(std::move(other._elements)), _vertexSize(other._vertexSize)
    {
        other._vertexSize = 0;
    }

    VertexFormat &operator=(VertexFormat &&other) noexcept
    {
        if (this != &other)
        {
            _elements = std::move(other._elements);
            _vertexSize = other._vertexSize;
            other._vertexSize = 0;
        }
        return *this;
    }

private:
    Vector<Element> _elements;
    unsigned int _vertexSize;
};

enum PrimitiveType
{
    TRIANGLES = GL_TRIANGLES,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    LINES = GL_LINES,
    LINE_STRIP = GL_LINE_STRIP
    // POINTS = GL_POINTS
};

class MeshBuffer
{
public:
    MeshBuffer(bool dynamic = false);
    ~MeshBuffer();

    void CreateVertexBuffer(const VertexFormat &vertexFormat, unsigned int vertexCount);
    void CreateIndexBuffer(unsigned int indexCount);

    void SetVertexData(const void *vertexData);
    void SetIndexData(const void *indexData);

    void Render(int mode, int count);
    void Release();

    void *Lock();
    bool Unlock();

private:
    friend class Mesh;
    friend class MeshBuilder;

    MeshBuffer(const MeshBuffer &) = delete;
    MeshBuffer &operator=(const MeshBuffer &) = delete;
    MeshBuffer(MeshBuffer &&) = delete;
    MeshBuffer &operator=(MeshBuffer &&) = delete;

    int m_numVertices = 0;
    int m_numIndices = 0;

    bool m_useIndices = false;
    bool m_dynamic = false;
    unsigned int m_ibo = 0;
    unsigned int m_vbo = 0;
    unsigned int m_vao = 0;
    VertexFormat m_vertexFormat;
};

const int VBO_POSITION = 0x00000001;
const int VBO_NORMAL = 0x00000002;
const int VBO_COLOR = 0x00000004;
const int VBO_TANGENT = 0x00000008;
const int VBO_BINORMAL = 0x00000010;
const int VBO_BLENDWEIGHTS = 0x00000020;
const int VBO_BLENDINDICES = 0x00000040;
const int VBO_TEXCOORD0 = 0x00000080;
const int VBO_TEXCOORD1 = 0x00000100;
const int VBO_INDICES = 0x00000200;

class Surface
{
public:
    Surface(const VertexFormat &vertexFormat, u32 material = 0, bool dynamic = false);
    virtual ~Surface();

    u32 GetMaterial() const { return m_material; }
    void SetMaterial(u32 material) { m_material = material; }

    void Render(u32 mode, u32 count);
    void Render(u32 mode);
    void Render();

    int AddVertex(const Vec3 &position);
    int AddVertex(float x, float y, float z);

    const void *GetVertices() const;
    void *GetVertices();

    void *GetIndices();
    const void *GetIndices() const;

    void TexturePlanarMapping(float resolution = 0.001f);
    void TexturePlanarMapping(float resolutionS, float resolutionT, u8 axis, const Vec3 &offset);
    // Changes backfacing triangles to frontfacing
    void FlipFaces();
    void FlipNormals();

    int AddVertex(const Vec3 &position, const Vec2 &texCoord);
    int AddVertex(float x, float y, float z, float u, float v);

    int AddVertex(const Vec3 &position, const Vec2 &texCoord, const Vec3 &normal);
    int AddVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz);

    void VertexPosition(u32 index, const Vec3 &position);
    void VertexPosition(u32 index, float x, float y, float z);

    void VertexNormal(u32 index, const Vec3 &normal);
    void VertexNormal(u32 index, float x, float y, float z);

    void VertexTexCoord(u32 index, const Vec2 &texCoord);
    void VertexTexCoord(u32 index, float u, float v);

    int AddFace(u32 v0, u32 v1, u32 v2);

    void SetName(const String &name) { m_name = name; }
    const String &GetName() const { return m_name; }

    void Transform(const Mat4 &transform);

    void Init();
    void Upload();
    void Release();

    void CalculateNormals();

    void CalculateSmothNormals(bool angleWeighted = false);

    void CalculateBoundingBox();

    BoundingBox GetBoundingBox() const { return m_boundingBox; }

    void VertexUpload(MeshBuffer *buffer, const VertexFormat &vertexFormat);

    void Clear();

    u32 GetVertexCount() const { return (u32)positions.size(); }
    u32 GetIndexCount() const { return (u32)indices.size(); }

private:
    struct VertexBuffer
    {
        String name;
        s32 size;
        s32 usage;
        unsigned int id;
        VertexBuffer()
        {
            size = 0;
            usage = 0;
            id = 0;
            name = "";
        }
    };
    void AddBuffer(VertexBuffer *buffer)
    {
        this->buffers.push_back(buffer);
    }

private:
    friend class Mesh;
    friend class MeshBuffer;
    friend class MeshBuilder;
    u32 m_material;
    u32 m_stride;
    bool m_dynamic;
    u32 flags;
    bool isDirty;
    String m_name;
    BoundingBox m_boundingBox;

    VertexFormat m_vertexFormat;
    Vector<Vec3> positions;
    Vector<Vec3> normals;
    Vector<Vec2> texCoords;
    Vector<Vec4> colors;
    Trivial<unsigned int> indices;
    Vector<VertexBuffer *> buffers;

    u32 IBO;
    u32 VAO;

    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
    Surface(Surface &&) = delete;
    Surface &operator=(Surface &&) = delete;
};

class Mesh 
{
public:
    Mesh(const VertexFormat &vertexFormat, bool dynamic = false);

    Surface *AddSurface(u32 material = 0);
    Surface *GetSurface(u32 index);

    const Surface *GetSurface(u32 index) const;

    int AddMaterial(Material *material);
    Material *GetMaterial(u32 index);

    u32 GetSurfaceCount() const { return (u32)m_surfaces.size(); }
    u32 GetMaterialCount() const { return (u32)m_materials.size(); }

    void Release();

    void Transform(const Mat4 &transform);

    void Render(Shader *shader, bool ignoreMaterial = false);
    void Render(Shader *shader, u32 mode, bool ignoreMaterial = false);
    void Render(Shader *shader, u32 mode, u32 count, bool ignoreMaterial = false);

    BoundingBox GetBoundingBox() const { return m_boundingBox; }
    void CalculateBoundingBox();

    void OrderByMaterial();

private:
    friend class MeshBuilder;
    friend class Surface;

    ~Mesh();

    Vector<Surface *> m_surfaces;
    Vector<Material *> m_materials;
    VertexFormat m_vertexFormat;
    bool m_dynamic = false;
    BoundingBox m_boundingBox;

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) = delete;
    Mesh &operator=(Mesh &&) = delete;
};

