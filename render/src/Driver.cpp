
#include "Driver.hpp"
#include "Device.hpp"
#include "Utils.hpp"
#include "OS.hpp"
#include "stb_image.h"
#include "MeshBuilder.hpp"





Driver &Driver::Instance()
{
    static Driver driver;
    return driver;
}

Driver::Driver()
{
   
}

Driver::~Driver()
{
   
}

void Driver::Init()
{
    LogInfo("[DRIVER] Initialized.");
     currentShader = 0;
    for (int i = 0; i < 8; i++)
    {
        currentTexture[i] = 0;
        currentCubeTexture[i] = 0;
        m_currentTexture[i] = nullptr;
    }
    depthTest = false;
    depthWrite = false;
    cullFace = false;
    blend = false;
    scissorTest = false;
    stencilTest = false;
    for (int i = 0; i < 4; i++)
    {
        colorMask[i] = true;
    }
    depthClamp = false;
    totalTextures=0;
    totalCubeTextures=0;
    totalShaders=0;
    totalTraingles=0;
    totalDrawCalls=0;
    totalVertex=0;
    depthMask = true;
    
    stencilMask = false;
    justDepth = false;
    stateMode=false;
    blendSrc = 0;
    blendDst = 0;
    cullFaceMode = 0;
    frontFace = 0;
    depthFunc = 0;
    stencilFunc = 0;
    stencilRef = 0;
    stencilMaskRef = 0xFF;

    m_texturePath = "assets/textures/";

    currentMode = BlendMode::COUNT;



    clearColor.Set(0, 0, 0, 1);
    viewport.Set(0, 0, 0, 0);
    scissor.Set(0, 0, 0, 0);

    m_currentShader = nullptr;
    MeshBuilder::Instance().Init();

    
}

void Driver::Resize(int width, int height)
{
    m_width = width;
    m_height = height;
    viewport.width = width;
    viewport.height = height;
    scissor.width = width;
    scissor.height = height;
}

void Driver::Release()
{
     LogInfo("[DRIVER] Destroyed");

     for (u32 i = 0; i < m_textures.size(); i++)
     {
         m_textures[i]->Release();
         delete m_textures[i];
     }
     m_textures.clear();

     MeshBuilder::Instance().Release();
}

Texture2D *Driver::LoadTexture(const char *file_name)
{
    String path = m_texturePath;
    path  +=  file_name;
    if (System::Instance().FileExists(path.c_str()))
    {
        Texture2D *texture = new Texture2D();
        texture->Load(path.c_str());
        m_textures.push_back(texture);
        return texture;
    }
    LogError("Failed to load texture: %s", path.c_str());
    return Texture2D::GetDefaultTexture();
}

Texture2D *Driver::LoadTexture(const Pixmap &pixmap)
{
    Texture2D *texture = new Texture2D();
    texture->Load(pixmap);
    m_textures.push_back(texture);
    return texture;
}

void Driver::SetTexturePath(const String &path)
{
    m_texturePath = path;
}

const String &Driver::GetTexturePath() const
{
    return m_texturePath;
}

u32 Driver::GetTotalDrawCalls()
{
    return totalDrawCalls;
}

u32 Driver::GetTotalVertex()
{
    return totalVertex;
}

u32 Driver::GetTotalTextures()
{
    return totalTextures;
}


u32 Driver::GetTotalPrograms()
{
    return totalShaders;
}

u32 Driver::GetTotalTriangles()
{
    return totalTraingles;
}



void Driver::ResetStats()
{
totalTextures=0;
totalCubeTextures=0;
totalShaders=0;
totalTraingles=0;
totalDrawCalls=0;
totalVertex=0;
}

void Driver::FlipImageOnLoad(bool flip)
{
    int isFlip = (flip == true) ? 1 : 0;
    stbi_set_flip_vertically_on_load(isFlip);
}

void Driver::SetShader(u32 shader)
{
    if (!stateMode)
    {
        glUseProgram(shader);
        totalShaders++;
        currentShader = shader;
        return;
    }
    if (currentShader != shader )
    {
        glUseProgram(shader);
        totalShaders++;
        currentShader = shader;
    }
}

void Driver::SetTextureId(u32 unit, u32 texture)
{
     if (!stateMode)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        totalTextures++;
        currentTexture[unit] = texture;
        return;
    
    }
if (currentTexture[unit] != texture )
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    totalTextures++;
    currentTexture[unit] = texture;
}
}

void Driver::SetCubeTexture(u32 unit, u32 texture)
{
     if (!stateMode)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        totalCubeTextures++;
        currentCubeTexture[unit] = texture;
        return;
    
    }

if (currentCubeTexture[unit] != texture )
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    totalCubeTextures++;
    currentCubeTexture[unit] = texture;
}

}

void Driver::SetDepthTest(bool enable)
{

     if (!stateMode)
    {
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        depthTest = enable;
        return;
    }


if (depthTest != enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    depthTest = enable;
}
}

void Driver::SetDepthWrite(bool enable)
{
     if (!stateMode)
    {
        if (depthWrite != enable)
        {
            glDepthMask(enable);
            depthWrite = enable;
        }
        return;
    }
    if (depthWrite != enable)
    {
        glDepthMask(enable);
        depthWrite = enable;
    }
}

void Driver::SetCullFace(bool enable)
{

    if (!stateMode)
    {
        if (enable)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        cullFace = enable;
        return;
    }

    if (cullFace != enable)
    {
        if (enable)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        cullFace = enable;
    }
}

void Driver::SetBlend(bool enable)
{

     if (!stateMode)
    {
        if (enable)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        blend = enable;
        return;
    }

    if (blend != enable )
    {
        if (enable)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        blend = enable;
    }
}

void Driver::SetScissorTest(bool enable)
{
     if (!stateMode)
    {
        if (enable)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
        scissorTest = enable;
        return;
    }
        
    if (scissorTest != enable)
    {
        if (enable)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
        scissorTest = enable;
    }
}

void Driver::SetStencilTest(bool enable)
{
     if (!stateMode)
    {
        if (enable)
        {
            glEnable(GL_STENCIL_TEST);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }
        stencilTest = enable;
        return;
    }
    if (stencilTest != enable )
    {
        if (enable)
        {
            glEnable(GL_STENCIL_TEST);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }
        stencilTest = enable;
    }
}

void Driver::SetColorMask(bool r, bool g, bool b, bool a)
{

    if (!stateMode)
    {
        if (colorMask[0] != r || colorMask[1] != g || colorMask[2] != b || colorMask[3] != a )
        {
            glColorMask(r, g, b, a);
            colorMask[0] = r;
            colorMask[1] = g;
            colorMask[2] = b;
            colorMask[3] = a;
        }
        return;
    }

    if (colorMask[0] != r || colorMask[1] != g || colorMask[2] != b || colorMask[3] != a )
    {
        glColorMask(r, g, b, a);
        colorMask[0] = r;
        colorMask[1] = g;
        colorMask[2] = b;
        colorMask[3] = a;
    }
}

void Driver::SetDepthMask(bool enable)
{
     if (!stateMode)
    {
        if (depthMask != enable)
        {
            glDepthMask(enable);
            depthMask = enable;
        }
        return;
    }
    if (depthMask != enable  )
    {
        glDepthMask(enable);
        depthMask = enable;
    }
}

void Driver::SetDepthClamp(bool enable)
{

     if (!stateMode)
    {
        if (enable)
        {
            glDisable(GL_DEPTH_CLAMP);
        }
        else
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        return;
    }
    if (depthClamp != enable)
    {
        if (enable)
        {
            glDisable(GL_DEPTH_CLAMP);
        }
        else
        {
            glEnable(GL_DEPTH_CLAMP);
        }
        depthClamp = enable;
    }
}

void Driver::SetStencilMask(bool enable)
{
     if (!stateMode)
    {
        if (stencilMask != enable)
        {
            glStencilMask(enable);
            stencilMask = enable;
        }
        return;
    }
    if (stencilMask != enable)
    {
        glStencilMask(enable);
        stencilMask = enable;
    }
}

void Driver::SetBlendFunc(u32 src, u32 dst)
{
     if (!stateMode)
    {
        if (blendSrc != src || blendDst != dst )
        {
            glBlendFunc(src, dst);
            blendSrc = src;
            blendDst = dst;
        }
        return;
    }
    if (blendSrc != src || blendDst != dst )
    {
        glBlendFunc(src, dst);
        blendSrc = src;
        blendDst = dst;
    }
}

void Driver::SetCullFaceMode(u32 mode)
{
     if (!stateMode)
    {
        if (cullFaceMode != mode )
        {
            glCullFace(mode);
            cullFaceMode = mode;
        }
        return;
    }
    if (cullFaceMode != mode )
    {
        glCullFace(mode);
        cullFaceMode = mode;
    }
}

void Driver::SetFrontFace(u32 mode)
{
     if (!stateMode)
    {
        if (frontFace != mode)
        {
            glFrontFace(mode);
            frontFace = mode;
        }
        return;
    }
    if (frontFace != mode)
    {
        glFrontFace(mode);
        frontFace = mode;
    }
}

void Driver::SetDepthFunc(u32 func)
{
     if (!stateMode)
    {
        if (depthFunc != func)
        {
            glDepthFunc(func);
            depthFunc = func;
        }
        return;
    }

    if (depthFunc != func)
    {
        glDepthFunc(func);
        depthFunc = func;
    }
}

void Driver::SetStencilFunc(u32 func, u32 ref, u32 mask)
{
     if (!stateMode)
    {
        if (stencilFunc != func || stencilRef != ref || stencilMaskRef != mask)
        {
            glStencilFunc(func, ref, mask);
            stencilFunc = func;
            stencilRef = ref;
            stencilMaskRef = mask;
        }
        return;
    }
    if (stencilFunc != func || stencilRef != ref || stencilMaskRef != mask)
    {
        glStencilFunc(func, ref, mask);
        stencilFunc = func;
        stencilRef = ref;
        stencilMaskRef = mask;
    }
}

void Driver::SetStencilOp(u32 sfail, u32 dpfail, u32 dppass)
{
glStencilOp(sfail, dpfail, dppass);
}

void Driver::SetScissor(u32 x, u32 y, u32 width, u32 height)
{

int yy = m_height - y - height;

glScissor(x, yy, width, height);
scissor.Set(x, y, width, height);
}

IntRect Driver::GetScissor()
{
return scissor;
}

IntRect Driver::GetViewport()
{
return viewport;

}

void Driver::SetViewport(u32 x, u32 y, u32 width, u32 height)
{
glViewport(x, y, width, height);
viewport.Set(x, y, width, height);
}


void Driver::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
    clearColor.r = (u8)(r * 255.0f);
    clearColor.g = (u8)(g * 255.0f);
    clearColor.b = (u8)(b * 255.0f);
    clearColor.a = (u8)(a * 255.0f);
    glClearColor(r, g, b, a);

}

void Driver::SetClearColor(u8 r, u8 g, u8 b, u8 a)
{
    clearColor.Set(r, g, b, a);
    float floatRed = static_cast<float>(r) / 255.0f;
    float floatGreen = static_cast<float>(g) / 255.0f;
    float floatBlue = static_cast<float>(b) / 255.0f;
    float floatAlpha = static_cast<float>(a) / 255.0f;

    glClearColor(floatRed, floatGreen, floatBlue, floatAlpha);
}

Color Driver::GetClearColor()
{
    return clearColor;
}



void Driver::Clear()
{
    u32 mask = GL_COLOR_BUFFER_BIT;
    if (depthMask)
    {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencilMask)
    {
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(mask);
}

 void  Driver::DrawElements(GLenum mode, GLsizei count, GLenum type,const void *indices)
{
    glDrawElements(mode, count, type, indices);
    if (mode == GL_TRIANGLES)
    {
		totalTraingles += count / 3;
	}

    totalDrawCalls++;
    totalVertex += count;
}

void Driver::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    if (mode == GL_TRIANGLES)
    {
        totalTraingles += count / 3;
    }
    totalDrawCalls++;
    totalVertex += count;
}

void Driver::SetBlendMode(BlendMode blendMode)
{
    if (!blend) return ;
    if (currentMode == blendMode) return;

    switch (blendMode)
    {
    case BlendMode::BLEND:
        SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendMode::ADD:
        SetBlendFunc(GL_ONE, GL_ONE);
        break;
    case BlendMode::MULTIPLY:
        SetBlendFunc(GL_DST_COLOR, GL_ZERO);
        break;
    case BlendMode::SCREEN:
        SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        break;
    case BlendMode::SUBTRACT:
        SetBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        break;
    case BlendMode::REPLACE:
        SetBlendFunc(GL_ONE, GL_ZERO);
        break;
    case BlendMode::LIGHTEN:
        SetBlendFunc(GL_ONE, GL_ONE);
        break;
    case BlendMode::DARKEN:
        SetBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendMode::LIGHT:
        SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendMode::COUNT:
        break;
    }
    currentMode = blendMode;
}

