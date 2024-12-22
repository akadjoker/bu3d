
#include "pch.h"
#include "Camera.hpp"


 int screenWidth = 1024 ;
 int screenHeight = 768 ;



const int TEXS=1;
const int BRUS=2;
const int NODE=3;
const int ANIM=4;
const int MESH=5;
const int VRTS=6;
const int TRIS=7;
const int BONE=8;
const int KEYS=9;

/*
quaternion start w,x,y,z a trucate -w
*/

#define MAX_BONE_INFLUENCE 4

struct Weight
{
    int     bone;
    float   weight;
    int     vertex;

    Weight()
    {
        bone=-1;
        weight=0;
        vertex=-1;
  
    }
};

int multCount=0;

struct Bone
{
Array<Weight> mWeights;
 String m_name;
 Bone(const String &n):m_name(n)
 {

 }
 ~Bone()
 {
    mWeights.clear();
 }
};

struct Vertex
{
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
    Color  color;
    int   bones[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];

    int numBones;
    Vertex()
    {
       numBones=0;
      color.Set(255,255,255,255);
    
    }

};
struct Anim
{
String name;
 int start;
 int end;
};

struct Brush
{
    String texture;
    float r,g,b,a;
    Texture2D tex;

    Brush()
    {
        r=g=b=a=1.0f;
        
        
    }
    ~Brush()
    {
        tex.Release();
    }
};


struct PosKeyFrame
{
    Vec3 pos;
    float frame;
};

struct RotKeyFrame
{
    Quaternion rot;
    float frame;
};




struct Node
{
    Vec3 position;
    Vec3 scale;
 
    Quaternion orientation;
    Mat4 AbsoluteTransformation;
    Mat4 LocalWorld;
   
    Node *parent;
    Array<Node*> childs;
   String m_name;
   String m_parentName;
    int index;

   virtual ~Node()
    {
            // std::cout<<" Free node"<<m_name<<std::endl;
            for (auto c : childs) 
            {
                delete c;
            }
            childs.clear();
    }

    Node(Node *parent=nullptr)
    {
        this->parent = parent;
        if (parent)
        {
            m_parentName=parent->m_name;
            parent->childs.push_back(this);
        }
        index =0;
        position.set(0,0,0);
        scale.set(1,1,1);
        orientation.identity();
        AbsoluteTransformation.identity();
        
     

    }
    Mat4 getRelativeTransformation()
    {


            LocalWorld = Mat4::Scale( scale.x, scale.y, scale.z );
            LocalWorld.rotate(orientation);
            LocalWorld.translate( position.x, position.y, position.z );

            

        if (parent)
        {
            Mat4 m_absTrans;
            Mat4::fastMult43(m_absTrans, parent->getRelativeTransformation(), LocalWorld );
            return m_absTrans;
           //return Mat4Multiply(LocalWorld,parent->AbsoluteTransformation);
        } else
        {
            return LocalWorld;
        }
    } 

    void UpdateAbsolutePosition()
    {
        AbsoluteTransformation = getRelativeTransformation(); 
    } 


    virtual void update()
    {
       // std::cout<<"Update :"<<name<<std::endl;
        UpdateAbsolutePosition();
        for ( Node * n : childs)
        {
            n->update();
        }
    }
    virtual void render( RenderBatch &batch)
    {
        // batch.BeginTransform(AbsoluteTransformation);
        // batch.Cube(Vec3(0,0,0),0.5,0.5,0.5,false);
        // batch.EndTransform();

         for ( Node * n : childs)
        {
            n->render(batch);
        }
    }

};

Vec3 VectorInterpolate(const Vec3& A, const Vec3& B, const float S) 
{
    Vec3 result;
    result.x = (A.x * S) + (B.x * (1.0f - S));
    result.y = (A.y * S) + (B.y * (1.0f - S));
    result.z = (A.z * S) + (B.z * (1.0f - S));
    return result;
}


struct Joint : public Node
{
    Array<PosKeyFrame> PosKeys;
    Array<RotKeyFrame> RotKeys;
    
    Mat4     offMatrix;

    Vec3    binPosition;
    Quaternion binOrientation;

   // Array<Weight> weights;
 
    Joint(const String &Name):Node(nullptr)
    {
            m_name = Name;
            offMatrix.identity();
    }

    void InitPose()
    {
     
        UpdateAbsolutePosition();
        offMatrix = AbsoluteTransformation.inverted();
        binPosition =   position;
        binOrientation =orientation;
    }

    int FindPosition(float time)
     {
        for (u32 i = 0; i < (int)PosKeys.size()-1; i++) 
        {
            if (time < PosKeys[i+1].frame) 
            {
                return i;
            }
        }
    return 0;
    }

int FindRotation(float time) 
{
    for (u32 i = 0; i < RotKeys.size()-1 ; i++)
     {
        if (time < RotKeys[i+1].frame) 
        {
            return i;
        }
    }
    return 0;
}




Quaternion AnimateRotation(float movetime) 
{
    int currentIndex = FindRotation(movetime);
    int nextIndex = currentIndex + 1;
    if (nextIndex > RotKeys.size()) 
    {
        return  binOrientation;
    }
    float deltaTime = RotKeys[nextIndex].frame - RotKeys[currentIndex].frame;
    float factor = (movetime - RotKeys[currentIndex].frame) / deltaTime;
    Quaternion orientation = Quaternion::Slerp(RotKeys[currentIndex].rot, RotKeys[nextIndex].rot, factor);
    return orientation;
}
Vec3 AnimatePosition(float movetime) 
{
    int currentIndex = FindPosition(movetime);
    int nextIndex = currentIndex + 1;
    if (nextIndex > PosKeys.size() ) 
    {
        return  binPosition;
    }
   // std::cout<<"Bone "<<name<<"  "<<nextIndex <<std::endl;
    float deltaTime = PosKeys[nextIndex].frame - PosKeys[currentIndex].frame;
    float factor = (movetime - PosKeyFrame(PosKeys[currentIndex]).frame) / deltaTime;
    return VectorInterpolate(PosKeys[currentIndex].pos, PosKeys[nextIndex].pos, factor);
}
void Animate(float movetime) 
{

      // std::cout<<"Bone "<<name<<"  "<<PosKeys.size() <<std::endl;
    if (PosKeys.size()==0 || RotKeys.size()==0) 
            return;

    if (PosKeys.size() != RotKeys.size()) 
    {
      Logger::Instance().Error("Joint::Animate: PosKeys.size() != RotKeys.size()");
        return;
    }




    position = AnimatePosition(movetime);
    orientation = AnimateRotation(movetime);
}


void render( RenderBatch &batch) override
{
    //    std::cout<<"Render :"<<name<<std::endl;
    
    Node::render(batch);
    Vec3 pos;
    Vec3 parentPos;

    pos = Mat4::Transform(AbsoluteTransformation,pos);
    if (parent)
    {
        parentPos = Mat4::Transform(parent->AbsoluteTransformation,parentPos);
    }

    batch.SetColor(1, 0, 0);
    batch.Line3D(pos,parentPos);



}


};

struct MeshBuffer
{
    Array<int>    tris;
    Array<Vertex>  vertex;
    Vec3 *position;

 
   String name;
    Array<Bone*> bones;
    int totalBones;


    int brush;
    ~MeshBuffer()
    {
            for (auto b : bones) 
            {
                delete b;
            }
            bones.clear();
            free(position);

    }


    void addBone(Bone *b)
    {
        bones.push_back(b);
    }

    Bone* getBoneAt( u32 index)
    {
        

        if (index<=0)
            index=0;
        if (index>=bones.size())
            index=bones.size();
        return bones[index];
    }

};

struct Entity : public Node
{
    Array<MeshBuffer*> surfaces;
    Array<Joint*> joints;
    Array<Anim*> animations;
    Array<Brush*> brushes;
    
   Mat4 transformVertex;


int AnimCur ;
int MaxTime ;
int CurTime ;
int Frames  ;

float framesPerSecond ;
float  duration ;
int frame;
int NumFrames;

    Entity(Node *parent=nullptr):Node(parent)
    {
          frame=0;
          transformVertex.identity();
    }
    ~Entity()
    {
 
    }

    void release()
    {
         for (auto surf : surfaces) 
            {
                delete surf;
            }
            surfaces.clear();
            for (auto b : joints) 
            {
                delete b;
            }
            joints.clear();
            for (auto b : animations) 
            {
                delete b;
            }
            animations.clear();
              for (auto b : brushes) 
            {
                delete b;
            }
            brushes.clear();
            // for (auto b : bones) 
            // {
            //     delete b;
            // }
            // bones.clear();
    }
    void update() override
    {
        Node::update();
        for (auto b : joints) 
        {
            b->update();
            b->Animate(frame);
        }
    }
    void render( RenderBatch &batch) override
    {


        for (unsigned int x=0;x<surfaces.size();x++)
        {
            MeshBuffer *surf = surfaces[x];
            if (surf->bones.size()<=0)
                continue;
                for(unsigned int j=0;j<surf->vertex.size();j++)
                {
                     surf->vertex[j].pos.x=0;
                     surf->vertex[j].pos.y=0;
                     surf->vertex[j].pos.z=0;
                }
               for (unsigned int j =0;j<surf->bones.size();j++)
                {
                    Bone * bone = surf->bones[j];
                    assert(bone!=nullptr);  
                    Joint * joint =getJoint(bone->m_name);
                    assert(joint!=nullptr);  
          
                    Mat4::fastMult43(transformVertex,joint->AbsoluteTransformation,joint->offMatrix);
                     multCount++;

                    for (int b=0;b<(int)bone->mWeights.size();b++)
                    {
                    	int vID   = bone->mWeights[b].vertex;
                        assert(vID <= surf->vertex.size());
		            	float w = bone->mWeights[b].weight;
                        
                        Vec3 pos =surf->position[vID];

                        pos = Mat4::Transform(transformVertex, pos);
                                       
                        surf->vertex[vID].pos.x+=pos.x *w;
                        surf->vertex[vID].pos.y+=pos.y *w;
                        surf->vertex[vID].pos.z+=pos.z *w;
                    }

               
                }
         }


    // for (unsigned int x=0;x<surfaces.size();x++)
    //     {
    //         MeshBuffer *surf = surfaces[x];
    //         if (surf->bones.size()<=0) continue;

    //             for(unsigned int j=0;j<surf->vertex.size();j++)
    //             {
    //                 // surf->vertex[j].pos.x=0;
    //                 // surf->vertex[j].pos.y=0;
    //                 // surf->vertex[j].pos.z=0;
    //             }

    //         for(unsigned int j=0;j<surf->vertex.size();j++)
    //         {
    //           //  memset(surf->position, 0, surf->vertex.size()  * sizeof(Vec3));
    //                 Vertex vertex = surf->vertex[j];
         
                
    //                 float x =0;
    //                 float y =0;
    //                 float z =0;
    //                 Vec3 pos;
                  

    //                if (vertex.numBones==1)
    //                {

    //                     Joint * joint = getJointAt(vertex.bones[0]);
    //                     assert(joint!=nullptr); 
    //                     float w = vertex.weights[0];
    //                     if (w==0.0f) continue;


    //                      Vec3 pos =surf->position[j];
    //                      Mat4::fastMult43(transformVertex, joint->AbsoluteTransformation, joint->offMatrix);   
                   
             
                       
    //                     multCount++;

    //                     pos = Mat4::Transform(transformVertex, pos);

    //                     surf->vertex[j].pos.x+=pos.x *w;
    //                     surf->vertex[j].pos.y+=pos.y *w;
    //                     surf->vertex[j].pos.z+=pos.z *w;



    //                     // x = ( transformVertex.x[0] * pos.x + transformVertex.x[4] * pos.y + transformVertex.x[8] * pos.z  + transformVertex.x[12] ) * w;
    //                     // y = ( transformVertex.x[1] * pos.x + transformVertex.x[5] * pos.y + transformVertex.x[9] * pos.z  + transformVertex.x[13] ) * w;
    //                     // z = ( transformVertex.x[2] * pos.x + transformVertex.x[6] * pos.y + transformVertex.x[10] * pos.z + transformVertex.x[14] ) * w;



    //                } else
    //                {
    //              //     Logger::Instance().Info("Vertex %d",vertex.numBones);
    //                     if (vertex.bones[0]!=-1)
    //                     {
    //                         Joint * joint = getJointAt(vertex.bones[0]);
    //                         assert(joint!=nullptr); 
    //                         float w = vertex.weights[0];
                           

    //                             Vec3 pos =surf->position[j];
    //                             Mat4::fastMult43(transformVertex, joint->AbsoluteTransformation, joint->offMatrix);

    //                             pos = Mat4::Transform(transformVertex, pos);

    //                             surf->vertex[j].pos.x+=pos.x *w;
    //                             surf->vertex[j].pos.y+=pos.y *w;
    //                             surf->vertex[j].pos.z+=pos.z *w;
                                                
                   

    //                     if (vertex.bones[1]!=-1)
    //                     {
    //                         Joint * joint = getJointAt(vertex.bones[1]);
    //                         assert(joint!=nullptr); 
    //                         float w = vertex.weights[1];

    //                             Vec3 pos =surf->position[j];
    //                             Mat4::fastMult43(transformVertex, joint->AbsoluteTransformation, joint->offMatrix);
    //                             pos = Mat4::Transform(transformVertex, pos);
    //                             surf->vertex[j].pos.x+=pos.x *w;
    //                             surf->vertex[j].pos.y+=pos.y *w;
    //                             surf->vertex[j].pos.z+=pos.z *w;
                     

    //                                  if (vertex.bones[2]!=-1)
    //                                 {
    //                                     Joint * joint = getJointAt(vertex.bones[2]);
    //                                     assert(joint!=nullptr); 
    //                                     float w = vertex.weights[2];
    //                                     Vec3 pos =surf->position[j];
    //                                     Mat4::fastMult43(transformVertex, joint->AbsoluteTransformation, joint->offMatrix);
    //                                     pos = Mat4::Transform(transformVertex, pos);
    //                                     surf->vertex[j].pos.x+=pos.x *w;
    //                                     surf->vertex[j].pos.y+=pos.y *w;
    //                                     surf->vertex[j].pos.z+=pos.z *w;
                     


    //                                             if (vertex.bones[3]!=-1)
    //                                             {
    //                                                 Joint * joint = getJointAt(vertex.bones[3]);
    //                                                 assert(joint!=nullptr); 
    //                                                 float w = vertex.weights[3];

    //                                                 Vec3 pos =surf->position[j];
    //                                                 Mat4::fastMult43(transformVertex, joint->AbsoluteTransformation, joint->offMatrix);
    //                                                 pos = Mat4::Transform(transformVertex, pos);
    //                                                 surf->vertex[j].pos.x+=pos.x *w;
    //                                                 surf->vertex[j].pos.y+=pos.y *w;
    //                                                 surf->vertex[j].pos.z+=pos.z *w;
                                        
                                        
    //                                             }
    //                             }
    //                  }
    //                 }
    //                }

    //                 // surf->vertex[j].pos.x=x;
    //                 // surf->vertex[j].pos.y=y;
    //                 // surf->vertex[j].pos.z=z;
    //         }
              
    //      }

         for ( int i=0;i<(int)surfaces.size();i++)
        {

            MeshBuffer *surf = surfaces[i];
            int brush =surf->brush;
    
      
             for (int i =0;i<  (int)surf->tris.size()/3;i++)
             {
                int f0 = surf->tris[i * 3 + 0];
                int f1 = surf->tris[i * 3 + 1];
                int f2 = surf->tris[i * 3 + 2];
                
                Vertex v0 = surf->vertex[f0];
                Vertex v1 = surf->vertex[f1];
                Vertex v2 = surf->vertex[f2];

                // Vec3 p0 = surf->position[f0];
                // Vec3 p1 = surf->position[f1];
                // Vec3 p2 = surf->position[f2];

                batch.TriangleLines(v0.pos,v1.pos,v2.pos);
   

             }
    

          
        }
     
     Node::render(batch);
     for (auto b : joints) 
            b->render(batch);
    }
    MeshBuffer *CreateMesh(const String &name)
    {
        MeshBuffer *m = new MeshBuffer();
        m->name=name;
        surfaces.push_back(m);
        return m;    
    }

Joint *getJoint( const String &str)
{
    for (auto n : joints)
    {
        if (strcmp(n->m_name.c_str(),str.c_str())==0)
                return n;
    }
    return nullptr;
}
Joint* getJointAt( int index)
{
    if (index<=0)
        index=0;
    if (index>=joints.size())
        index=(int)joints.size();
    return joints[index];
}
int getJointId( const String &str)
{
    for (int i=0;i<(int)joints.size();i++)
    {
        if (strcmp(joints[i]->m_name.c_str(),str.c_str())==0)
            return i;
    }
   
    return -1;
}
void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.bones[i] < 0)
        {
            vertex.weights[i] = weight;
            vertex.bones[i] = boneID;
            vertex.numBones++;
            break;
        }
    }
}
void SetVertexBoneDataToDefault(Vertex& vertex)
{
    vertex.numBones=0;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.bones[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

//************************
 FILE *in;

Array<int> chunk_stack;
bool hasNormals=false;
MeshBuffer *v_surf    =nullptr;



 int swap_endian( int n )
{
	return ((n&0xff)<<24)|((n&0xff00)<<8)|((n&0xff0000)>>8)|((n&0xff000000)>>24);
}

 void clear()
{
   
    hasNormals=false;
	chunk_stack.clear();
    
}

 int readChunk()
{
	int header[2];
	if( fread( header,8,1,in )<1 ) return 0;
	chunk_stack.push_back( ftell( in )+header[1] );
	return swap_endian( header[0] );
}



void exitChunk()
{
	fseek( in,chunk_stack.back(),SEEK_SET );
	chunk_stack.pop_back();
}

 int chunkSize()
{
	return chunk_stack.back()-ftell( in );
}

 void read( void *buf,int n )
{
	fread( buf,n,1,in );
}

 void skip( int n )
{
	fseek( in,n,SEEK_CUR );
}

 int readInt()
{
	int n;
	read( &n,4 );
	return n;
}

String readChunkName()
{
    String t;
  	for(int i=0;i<4;i++)
    {
		char c;
		read( &c,1 );
		t+=c;
	}
    int size= readInt();  
    chunk_stack.push_back( ftell( in )+ size);
    return t;


}
 void readIntArray( int t[],int n )
{
	read( t,n*4 );
}

 float readFloat()
{
	float n;
	read( &n,4 );
	return n;
}

 void readFloatArray( float t[],int n )
{
	read( t,n*4 );
}

 Vec3 readVector3( )
{
    Vec3 v;
	v.x=readFloat();
    v.y=readFloat();
    v.z=readFloat();
	return v;
}

 Quaternion readQuat( )
{
    Quaternion v;
	v.w=-readFloat(); 
    v.x=readFloat();
    v.y=readFloat();
    v.z=readFloat();
	return v;
}
 Vec2 readVector2( )
{
    Vec2 v;
	v.x=readFloat();
    v.y=readFloat();
	return v;
}

 Color readColor(  )
{
	float r=readFloat();if(r<0) r=0;else if(r>1) r=1;
	float g=readFloat();if(g<0) g=0;else if(g>1) g=1;
	float b=readFloat();if(b<0) b=0;else if(b>1) b=1;
	float a=readFloat();if(a<0) a=0;else if(a>1) a=1;
    Color c;
    c.Set((unsigned char)r*255,(unsigned char)g*255,(unsigned char)b*255,(unsigned char)a*255);
    return c;
	//*t=(int(a*255)<<24)|(int(r*255)<<16)|(int(g*255)<<8)|int(b*255);
}

String readString()
{
	String t;
	for(;;)
    {
		char c;
		read( &c,1 );
		if( !c ) return t;
		t+=c;
	}
}

 String readTag()
{
	String t;
    int pos = ftell( in );
	for(int i=0;i<4;i++)
    {
		char c;
		read( &c,1 );
		t+=c;
	}
    fseek( in,pos,SEEK_SET );
    return t;
}
 void readTextures(const String &path)
{
	while( chunkSize() )
    {
		String name=readString();
		int flags=readInt();
		int blend=readInt();
		Vec2 pos,scl;
        pos = readVector2();
        scl = readVector2();
		float rot=readFloat();
        
        Logger::Instance().Info("Texture %s  uvs %d  blend %d ",name.c_str() , flags,blend);

     

     

	}
}
 void readBrushes(const String &path)
{
	int n_texs=readInt();
	int tex_id[8]={-1,-1,-1,-1,-1,-1,-1,-1};
	while( chunkSize() )
    {
		String name=readString();
		Color col;
		col = readColor();
		float shi=readFloat();
		int blend=readInt();
		int fx=readInt();
		readIntArray( tex_id,n_texs );

       Logger::Instance().Info("Brush %s  blend %d ",name.c_str() , blend);

        Brush * brush = new Brush();

           	
            
        brushes.push_back(brush);
	}
}
 int readVertices()
{

    assert(v_surf!=nullptr);
    const int max_tex_coords = 3;
	int  tex_coord_sets, tex_coord_set_size;
    
	int flags=readInt();
	int tc_sets=readInt();
	int tc_size=readInt();

	float tc[4]={0};
  


    int size = 12 +  tc_sets * tc_size * 4;

	if (flags & 1)
	{
	
		size += 12;
	}
	if (flags & 2)
	{
		size += 16;
	
	}

	int numVertex = chunkSize() / size;
    v_surf->position = (Vec3*) calloc(numVertex , sizeof(Vec3));
    int index=0;

	
	   
	//Surface::Vertex t;
	while( chunkSize() )
    {
        Vertex v;
        SetVertexBoneDataToDefault(v);
        
        float tex_coords[2]={0};
        Color color;
        

		v.pos = readVector3();
		if( flags&1 )
        {
			v.normal = readVector3();
            hasNormals=true;
		}
		if( flags&2 )
        {
			color = readColor();
		}
		for( int k=0;k<tc_sets;++k )
        {
			readFloatArray( tc, tc_size );
		}
        v.uv.x = tc[0];
        v.uv.y =1.0f*-tc[1];
        v_surf->vertex.push_back(v);
        v_surf->position[index++]=v.pos;
	}

    Logger::Instance().Info("Vertices %d",v_surf->vertex.size());

	return flags;
}

 int TagID(const String &tag)
{

    
	if(tag=="TEXS") return TEXS;
	if(tag=="BRUS") return BRUS;
	if(tag=="NODE") return NODE;
	if(tag=="ANIM") return ANIM;
	if(tag=="MESH") return MESH;
	if(tag=="VRTS") return VRTS;
	if(tag=="TRIS") return TRIS;
	if(tag=="BONE") return BONE;
	if(tag=="KEYS") return KEYS;
	return 0;

}

 void readTriangles()
{
    
    bool showwarning=false;
	int brush_id=readInt();
    int TriangleCount = (int)(chunkSize() / 12);

    const int max_tex_coords = 3;
	int flags, tex_coord_sets, tex_coord_set_size;
    
  
    
	while( chunkSize() )
    {
		int vertex_id[3];
		readIntArray( vertex_id,3 );
        v_surf->tris.push_back(vertex_id[0]);
        v_surf->tris.push_back(vertex_id[1]);
        v_surf->tris.push_back(vertex_id[2]);
	}

    Logger::Instance().Info("Triangles %d",v_surf->tris.size()/3);



    	

}

 void readMesh( )
{

    String name =String("surface_")+String(surfaces.size());
    v_surf = CreateMesh(name);


	int flags=0;
    v_surf->brush=readInt();

	while( chunkSize() )
    {
          String tag =readChunkName();
		  if (tag=="VRTS")
			    readVertices();
			else
            if(tag=="TRIS")
           {
               readTriangles();
            }
			
		
		exitChunk();
	}
	
}

MeshBuffer *getSurface(int index)
{
    if (index<=0)
        index=0;
    if (index >=surfaces.size())
        index = surfaces.size();

    return surfaces[index];
}

 void readBone(Joint *joint)
{
    assert(v_surf!=nullptr);
    assert(joint!=nullptr);

    int boneID = joint->index;
    int wights=0;    
    Bone *b= new Bone(joint->m_name);
	while( chunkSize() )
    {
		int vertexId=readInt();
		float vertexWight=readFloat();
        
           
                 Weight weight;
                 weight.bone = joint->index;
                 weight.vertex=vertexId;
                 weight.weight=vertexWight;
        
                 Vertex v= v_surf->vertex[vertexId];
                 SetVertexBoneData(v,boneID,vertexId);
                 v_surf->vertex[vertexId]=v;
                

                b->mWeights.push_back(weight);

	
	}
    v_surf->addBone(b);
  
//	TraceLog(LOG_INFO, " LOAD BONES %s surface (%d)   ",joint->m_name.c_str(), buffer_id);

}

void  readKeys(Joint *joint)
{
    assert(v_surf!=nullptr);
	int flags=readInt();
 

    

	while( chunkSize() )
    {
		int frame=readInt();

        
		if( flags&1 )
        {
            PosKeyFrame Key;
            Key.frame=frame;
			Key.pos=readVector3();
            joint->PosKeys.push_back(Key);
		}
		if( flags&2 )
        {
			Vec3 scl=readVector3();

		}
		if( flags&4 )
        {
             RotKeyFrame Key;
             Key.frame=frame;
             Key.rot =readQuat();
             joint->RotKeys.push_back(Key);
		}
         
   
	}



}

 void readNODE(Joint *parent)
{
	String name=readString();

    
    
    Joint *joint= new Joint(name);
	
    joint->position    = readVector3();
    joint->scale       = readVector3();
    joint->orientation = readQuat();
    
    if (parent)
    {
        joint->m_parentName = parent->m_name;
    } 
    joint->index=(int)joints.size();
    joints.push_back(joint);


	
    while( chunkSize() )
    {

        String tag =readChunkName();
		if (tag=="MESH")
        {
            // std::string surName = "mesh_"+std::to_string((int)surfaces.size());
            // TraceLog(LOG_INFO,"Create Mesh  %s",surName.c_str());
            // surf = CreateMesh(surName);
           	readMesh();
        }
		else
        if (tag=="BONE")
        {
            readBone(joint);
        }else
		if (tag=="KEYS")
        {
			readKeys( joint);

            
        } else 
        if (tag=="ANIM")
        {
			int flags    = readInt();
			NumFrames   = readInt();
			framesPerSecond =readFloat();
            if (framesPerSecond <= 0) 
            {
            framesPerSecond = 25.0;
            }
    	    duration =  (NumFrames / framesPerSecond);
           Logger::Instance().Info("Duration %f FramesPerSecond %f NumFrames %d",duration, framesPerSecond, NumFrames);

        }else
		if (tag=="NODE")
        {
			readNODE(joint);
        }
       	
        exitChunk();
	}


}

bool Load(const String &f)
{
 
    in=fopen( f.c_str(),"rb" );
	if( !in )
    {
       Logger::Instance().Error("Can't open file %s",f.c_str());
        return false;
    }


	String tag =readChunkName();
	if( tag!="BB3D" )
    {
		fclose( in );
		return 0;
	}

	int version=readInt();
	if( version>1 )
    {
		fclose( in );
		return 0;
	}

   String surName = String("mesh_")+String(surfaces.size());
   Logger::Instance().Info("Create Mesh  %s",surName.c_str());
 
 
   MeshBuffer *surf = CreateMesh(surName);
   
    
  
	while( chunkSize() )
    {
		String tag =readChunkName();
	    if (tag=="TEXS")
			readTextures("");
		else
		if (tag=="BRUS")
			readBrushes("");
		else
		if (tag=="NODE")
		    readNODE(nullptr );
             
		exitChunk();
	}

	fclose( in );
	clear();
    

for (unsigned int i=0;i<joints.size();i++)
{
   Joint *joint=joints[i];
   String name = joint->m_name;

   Logger::Instance().Info("Joint %s",name.c_str());

//   std::cout << " Joint Name:" << name <<             " Parent Name :" << joint->m_name  << endl;

    for (auto j : joints)
    {
        if (j->m_name==joint->m_parentName)
        {
            joint->parent=j;
            joint->InitPose();
            break;
        }
    }
    if (!joint->parent)
        joint->parent=this;
}


   
    return true;
}

    


};

int main()
{

    Device device;
    Driver driver;
    TextureManager textureManager;
    RenderBatch batch;
    Logger logger;
    Shader shader;
    System system;
    Font        font;

    
    device = Device::Instance();

    device.Create(screenWidth, screenHeight, "Engine", false);

    batch.Init(2, 1024*4); 


    font.LoadDefaultFont();
    font.SetBatch(&batch);

    float camera_distance = 850.0f;
    float camera_rotation_x = 50.0f;
    float camera_rotation_y = 30.0f;

    float lastWell =camera_distance;
    float well = 0.0f;


      Entity  model;
      model.Load("assets/vegeta.b3d");
      model.scale.set(0.2,0.2,0.2);

      float currentTime = 0.0f;
      float frameTime;;

     u32  startTime= SDL_GetTicks64();



 
    while (device.Run())
    {
        u32 timeMs = SDL_GetTicks64();
        frameTime = (timeMs - startTime) / 1000.0f;
        startTime = timeMs;
      
        currentTime += frameTime * model.framesPerSecond;
 

        if (currentTime > model.NumFrames)
        {
            currentTime = 0.0f;
        }


        frameTime = device.GetFrameTime();

     

        model.frame = currentTime;



        Driver::Instance().SetBlend(true);
        Driver::Instance().SetBlendMode(BlendMode::BLEND);
        Driver::Instance().SetDepthTest(true);
        Driver::Instance().SetDepthClamp(true);
        
        Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        Driver::Instance().Clear();


       if (Mouse::Down(0))
        {
         const float sensitivity = 0.01f * (frameTime *1000);

        double xpos = Mouse::dX();
        double ypos = Mouse::dY();

        camera_rotation_x += xpos * sensitivity;
        camera_rotation_y += ypos * sensitivity;
       
          
        }

        if (Mouse::Down(1))
        {
           
            camera_distance -=  0.5f * Mouse::Wheel() * (frameTime*1000);
          
        } 

        //  if (camera_distance < 1.0f) camera_distance = 1.0f;
        // if (camera_distance > 10.0f) camera_distance = 10.0f;

        if(camera_rotation_y > 89.0f) camera_rotation_y = 89.0f;
        if(camera_rotation_y < -89.0f) camera_rotation_y = -89.0f;
        
      

        float camx =  sin(ToRadians(camera_rotation_x)) * cos(ToRadians(camera_rotation_y)) * camera_distance;
        float camy =  sin(ToRadians(camera_rotation_y)) * camera_distance;
        float camz =  cos(ToRadians(camera_rotation_x)) * cos(ToRadians(camera_rotation_y)) * camera_distance;


   
        Mat4 proj = Mat4::Perspective(ToRadians(60.0f), (float)screenWidth / (float)screenHeight, 0.1f, 2000.0f);
        Mat4 view = Mat4::LookAt(Vec3(camx, camy, camz), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
        Mat4 mvp = proj * view;



        model.render(batch);
        model.update();

        batch.Grid(20, 1.0f);
        batch.SetMatrix(mvp);
        batch.Render();
    


      
   
//2d
        Driver::Instance().SetDepthTest(false);
        Mat4 model = Mat4::Ortho(0.0f, screenWidth, screenHeight,0.0f, -5.0f, 5.0f);
        batch.SetMatrix(model);


        batch.SetColor(255, 255, 255, 255);
        font.Print(10, 10, System::Instance().TextFormat("FPS: %d", Device::Instance().GetFPS()));
        font.Print(10, 30, System::Instance().TextFormat("%f %f", camera_rotation_x, camera_rotation_y));
        batch.Render();



        device.Flip();
    }
    model.release();
    batch.Release();
    device.Close();
    return 0;
}