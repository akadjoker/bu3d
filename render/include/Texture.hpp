#pragma once


#include "Config.hpp"
#include "OpenGL.h"
#include "Texture.hpp"
#include "Utils.hpp"
#include "OS.hpp"
#include "Math.hpp"
#include "Color.hpp"
#include "String.hpp"


enum WrapMode
{
	ClampToEdge       = 0x812F,
	ClampToBorder     = 0x812D,
	MirroredRepeat    = 0x8370,
	Repeat            = 0x2901 // Default
	
};



enum FilterMode
{
	Nearest           = 0x2600,
	Linear            = 0x2601,
	NearestMipNearest = 0x2700,
	LinearMipNearest  = 0x2701,
	NearestMipLinear  = 0x2702, // This is the default setting
	LinearMipLinear   = 0x2703
};

class  Pixmap
{
public:
    Pixmap();
    ~Pixmap();
    Pixmap(int w, int h, int components);
    Pixmap(int w, int h, int components, unsigned char *data);

    Pixmap(const Pixmap &image,const IntRect &crop);
    Pixmap(const Pixmap &other) = delete;
    Pixmap &operator=(const Pixmap &other) = delete;
    Pixmap(Pixmap&&) = delete;

    void SetPixel(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a);
    void SetPixel(u32 x, u32 y, u32 rgba);

    u32 GetPixel(u32 x, u32 y) const;
    Color GetPixelColor(u32 x, u32 y) const;

    void Fill(u8 r, u8 g, u8 b, u8 a);
    void Fill(u32 rgba);
    
    bool Save(const char* file_name);
    
    void Clear();
    

    bool Load(const char* file_name);
    bool LoadFromMemory(const unsigned char *buffer,   unsigned int bytesRead);


    void FlipVertical();
    void FlipHorizontal();




    unsigned char *pixels;
    int components;
    int width;
    int height;
};




class  Texture : public Ref
{
    public:
        Texture();
        virtual ~Texture();


           
        u32 GetID() {return id;}

	FilterMode GetMinFilter() const { return MinificationFilter; }
	FilterMode GetMagFilter() const { return MagnificationFilter; }
	WrapMode GetWrapS() const { return HorizontalWrap; }
	WrapMode GetWrapT() const { return VerticalWrap; }

    int GetWidth() {return width;}
    int GetHeight() {return height;}
	
	void SetMinFilter(FilterMode filter);
	void SetMagFilter(FilterMode filter);
	void SetWrapS(WrapMode mode);
	void SetWrapT(WrapMode mode);
	void SetAnisotropicFiltering(float level = -1.0f);

    void Use(u32 unit=0);
    void Update(const Pixmap &pixmap);
    void Update(const unsigned char *buffer, u16 components, int width, int height);

    virtual    void Release();


        
    protected:
        u32 id;   
        WrapMode       HorizontalWrap;
        WrapMode       VerticalWrap;
	    FilterMode      MinificationFilter;
	    FilterMode      MagnificationFilter;
	    float          MaxAnisotropic;
        int width;          
        int height;    

        void createTexture();

        Texture& operator=(const Texture& other) = delete;
        Texture(const Texture& other) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(Texture&&) = delete;

   



     
};


class   Texture2D  : public Texture
{
    public:
    Texture2D();

    
    Texture2D(int w, int h);
    Texture2D(const Pixmap &pixmap);
    Texture2D(const char* file_name);

    bool Load(const Pixmap &pixmap);
    bool Load(const char* file_name);
    bool LoadFromMemory(const unsigned char *buffer,u16 components, int width, int height);
    u32 GetID() {return id;}

    static Texture2D * GetDefaultTexture();


    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&&) = delete;
    Texture2D& operator=(Texture2D&&) = delete;


    private:
        friend class Texture;
        s32 components{0};     
        static Texture2D * defaultTexture;
     
};


class  CubemapTexture : public Texture
{
public:
    CubemapTexture() = default;
    CubemapTexture(const CubemapTexture&) = delete;
    CubemapTexture& operator=(const CubemapTexture&) = delete;
    CubemapTexture(CubemapTexture&&) = delete;
    CubemapTexture& operator=(CubemapTexture&&) = delete;


    CubemapTexture(const String& PosXFilename,
                   const String& NegXFilename,
                   const String& PosYFilename,
                   const String& NegYFilename,
                   const String& PosZFilename,
                   const String& NegZFilename);


    bool Load();




private:

    String m_fileNames[6];

};