#pragma once


#include "Config.hpp"
#include "OpenGL.h"
#include "Chars.hpp"
#include "Map.hpp"



class  Shader 
{
    public:
           Shader();
            virtual ~Shader();
        

            bool Create(const char* vShaderCode, const char* fShaderCode);
            bool Create(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode);
            bool Load(const char* vShaderCode, const char* fShaderCode);
            bool Load(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode);
            bool operator ==(const Shader&      other) const { return m_program == other.m_program; }
            bool operator !=(const Shader&      other) const { return m_program != other.m_program; }


   

        u32 GetID()    {        return m_program;    }

        void Use() const;



   

        void SetInt(const Chars &name, int value) ;
 
        void SetMatrix4(const Chars &name, const float *value) ;
        void SetMatrix3(const Chars &name, const float *value) ;

        void SetFloat(const Chars& name, float v);
        void SetFloat(const Chars& name, float x, float y);
        void SetFloat(const Chars& name, float x, float y, float z);
        void SetFloat(const Chars& name, float x, float y, float z,float w);



        void Release();

        


    void print();

    bool findUniform(const Chars &name)const;

    int  getUniform(const Chars &name);
    int  getAttribute(const Chars &name);


    int  getUniformLocation(const Chars &uniformName) const;
    int  getAttribLocation( const Chars &attribName) const;
    
    bool addUniform(const Chars &name);
    bool addAttribute(const Chars &name);

    bool ContainsUniform(const Chars &name) const;

    void LoadDefaults();

private:
        u32 m_program;
        int m_numAttributes;
        int m_numUniforms;


    private:
         void checkCompileErrors(unsigned int shader, const Chars &type);
 
           Shader& operator =(const Shader& other) = delete;
           Shader(const Shader& other) = delete;
          
};