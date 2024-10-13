#pragma once
#include "Config.hpp"
#include "Node.hpp"
#include "Node3D.hpp"
#include "Mesh.hpp"
#include "Math.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include "Map.hpp"

class  Model : public Node3D 
{

    public:
        Model();
        virtual ~Model();


      


        Mesh *AddMesh(Mesh *mesh);
        Mesh *AddMesh(const VertexFormat& vertexFormat);

        bool Load(const String &fileName,const VertexFormat& vertexFormat);


        Mesh *GetMesh(u32 index) { return m_meshs[index]; }

        void Render(Shader *shader) override;

        void RenderNoMaterial(Shader *shader);



        void Transform(const Mat4 &transform);


        void CalculateBox();

        void Release() override;

        

        BoundingBox GetBoundingBox() { return m_boundingBox; }

        Vec3 GetSize();

    private:
         friend class Scene;
        Vector<Mesh*>       m_meshs;
        Vector<Material*>   m_materials;
        BoundingBox              m_boundingBox;
};
