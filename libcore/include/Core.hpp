#pragma once

// Detect platform
#if defined(WIN32) || defined(_WINDOWS) || defined(_WIN32)
#if !defined(PLATFORM_WIN)
#define PLATFORM_WIN
#endif
#define CORE_WINDOWS
#define CORE_WINDOWS_API
#define PLATFORM_DESKTOP
#elif defined(__ANDROID__)
#if !defined(PLATFORM_ANDROID)
#define PLATFORM_ANDROID
#endif
#define CORE_LINUX
#define CORE_LINUX_API
#define PLATFORM_MOBILE
#elif defined(__EMSCRIPTEN__)
#if !defined(PLATFORM_EMSCRIPTEN)
#define PLATFORM_EMSCRIPTEN
#endif
#define CORE_LINUX
#define CORE_LINUX_API
#define PLATFORM_WEB
#else
#if !defined(PLATFORM_LINUX)
#define PLATFORM_LINUX
#endif
#define CORE_LINUX
#define CORE_LINUX_API
#define PLATFORM_DESKTOP
#endif

typedef unsigned char u8;
typedef signed char s8;
typedef char c8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef long long s64;
typedef float f32;
typedef double f64;

#define GLSL(src) "#version 460 core\n" #src

#if defined(WIN32) || defined(_WINDOWS) || defined(_WIN32)

#pragma warning(disable : 4514) // unreferenced inline removed
#pragma warning(disable : 4820) // padding added
#pragma warning(disable : 4828) // bytes padding added

#endif

#if defined(_DEBUG)
#include <assert.h>

#define DEBUG_BREAK_IF(condition)                                                      \
    if (condition)                                                                     \
    {                                                                                  \
        fprintf(stderr, "Debug break: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
        std::exit(EXIT_FAILURE);                                                       \
    }

#define ASSERT(condition, message)                                 \
    do                                                             \
    {                                                              \
        if (!(condition))                                          \
        {                                                          \
            fprintf(stderr, "[%s:%d] Assert failed in %s(): %s\n", \
                    __FILE__, __LINE__, __func__, message);        \
            abort();                                               \
        }                                                          \
    } while (false)

// Indicates that we know execution should never reach this point in the
// program. In debug mode, we assert this fact because it's a bug to get here.
//
// In release mode, we use compiler-specific built in functions to tell the
// compiler the code can't be reached. This avoids "missing return" warnings
// in some cases and also lets it perform some optimizations by assuming the
// code is never reached.
#define UNREACHABLE()                                                        \
    do                                                                       \
    {                                                                        \
        fprintf(stderr, "[%s:%d] This code should not be reached in %s()\n", \
                __FILE__, __LINE__, __func__);                               \
        abort();                                                             \
    } while (false)

#else
#define DEBUG_BREAK_IF(_CONDITION_)

#define ASSERT(condition, message) \
    do                             \
    {                              \
    } while (false)

// Tell the compiler that this part of the code will never be reached.
#if defined(_MSC_VER)

#define UNREACHABLE() __assume(0)
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE()
#endif

#endif

#include <time.h>
#include <climits>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "Math.hpp"

enum WrapMode
{
    ClampToEdge = 0x812F,
    ClampToBorder = 0x812D,
    MirroredRepeat = 0x8370,
    Repeat = 0x2901 // Default

};

enum FilterMode
{
    Nearest = 0x2600,
    Linear = 0x2601,
    NearestMipNearest = 0x2700,
    LinearMipNearest = 0x2701,
    NearestMipLinear = 0x2702, // This is the default setting
    LinearMipLinear = 0x2703
};

enum PixelFormat
{
    GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    GRAY_ALPHA,    // 8*2 bpp (2 channels)
    R8G8B8,        // 24 bpp
    R8G8B8A8,      // 32 bpp
};

enum class VertexType
{
    POSITION = 1,
    NORMAL,
    COLOR,
    TANGENT,
    BINORMAL,
    BLENDWEIGHTS,
    BLENDINDICES,
    TEXCOORD0,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    TEXCOORD4,
    TEXCOORD5,
    TEXCOORD6,
    TEXCOORD7,
    VT_COUNT
};

const int VIEW_MATRIX       = 0;
const int PROJECTION_MATRIX = 1;
const int MODEL_MATRIX      = 2;

const int BLEND_NORMAL      = 0;
const int BLEND_ADDITIVE    = 1;
const int BLEND_MULTIPLY    = 2;
const int BLEND_SUBTRACT    = 3;
const int BLEND_SCREEN      = 4;
const int BLEND_COUNT       = 5;

class Utils
{
public:
    static void Sleep(int ms) { SDL_Delay(ms); }
    static void LogError(const char *msg, ...);
    static void LogInfo(const char *msg, ...);
    static void LogWarning(const char *msg, ...);
    static unsigned char *LoadDataFile(const char *fileName, unsigned int *bytesRead);
    static char *LoadTextFile(const char *fileName);
    static bool FileExists(const char *fileName);
    static bool DirectoryExists(const char *dirPath);
    static bool IsFileExtension(const char *fileName, const char *ext);
    static const char *TextFormat(const char *text, ...);
    static const char *GetFileExtension(const char *fileName);
    static const char *GetFileName(const char *filePath);
    static const char *GetFileNameWithoutExt(const char *filePath);
    static const char *GetDirectoryPath(const char *filePath);

    static float getRed(u32 value) { return (float)((value >> 16) & 0xFF) / 255.0f; }
    static float getGreen(u32 value) { return (float)((value >> 8) & 0xFF) / 255.0f; }
    static float getBlue(u32 value) { return (float)((value >> 0) & 0xFF) / 255.0f; }
    static float getAlpha(u32 value) { return (float)((value >> 24) & 0xFF) / 255.0f; }
};

struct Rectangle
{
    Rectangle()
    {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }
    Rectangle(float _x, float _y, float _width, float _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }
    void set(float _x, float _y, float _width, float _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }
    void Clip(const Rectangle &rect)
    {
        float x1 = x;
        float y1 = y;
        float x2 = x + width;
        float y2 = y + height;
        float rx1 = rect.x;
        float ry1 = rect.y;
        float rx2 = rect.x + rect.width;
        float ry2 = rect.y + rect.height;
        if (x1 < rx1)
            x1 = rx1;
        if (y1 < ry1)
            y1 = ry1;
        if (x2 > rx2)
            x2 = rx2;
        if (y2 > ry2)
            y2 = ry2;
        x = x1;
        y = y1;
        width = x2 - x1;
        height = y2 - y1;
    }
    bool Contains(int px, int py)
    {
        return (px >= x && py >= y && px < x + width && py < y + height);
    }
    bool Contains(float px, float py)
    {
        return (px >= x && py >= y && px < x + width && py < y + height);
    }

    void Set(float _x, float _y, float _width, float _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }

    float x, y, width, height;
};

class ScreenQuad
{
    GLuint vao;
    GLuint vbo;
public:
    ScreenQuad();

    void Init();
    void Release();
    void Render(float x, float y, float width, float height);
};


class Driver
{
    private:
        static Driver *instance;
        u32 width;
        u32 height;
        Mat4 matrix[2];
        Mat4 viewProjection;
        Vec3 cameraPosition;
        Vec3 cameraRotation;
        Frustum frustum;
        ScreenQuad screenQuad;
        unsigned  long triangles;
        unsigned  long vertices;
        unsigned  long drawCalls;

    public:
        static Driver *InstancePtr();
        static Driver& Instance();


        void SetTransform(u8 mode, const Mat4 &matrix);

        ScreenQuad& GetScreenQuad() { return screenQuad; }

        Mat4& GetTransform(u8 mode)   { return matrix[mode]; } 
        const Mat4& GetTransform(u8 mode) const { return matrix[mode]; } 

        Mat4& GetViewProjection() { return viewProjection; }
        const Mat4& GetViewProjection() const { return viewProjection; }

        const Vec3& GetCameraPosition() const { return cameraPosition; }
        const Vec3& GetCameraRotation() const { return cameraRotation; } 
        
        void EnableBlend(bool enable);
        void EnableDepthTest(bool enable);
        void EnableCullFace(bool enable);
        void EnableColorMask(bool enable);
        void SetBlend(int src, int dst);
        void SetBlendMode(u8 mode);
        void SetViewport(float x, float y, float width, float height);
        void SetClearColor(float r, float g, float b);
        void Clear(int mode);
 
        void UpdateFrustum();
        bool IsInFrustum(const Vec3 &point);
        bool IsInFrustum(const Vec3 &min, const Vec3 &max);
        bool IsInFrustum(const BoundingBox &box);

        void DrawArrays(int mode, int first,int vertexCount);
        void DrawElements(int mode, int indexCount, int indexType, const void *indices);

        unsigned long GetTotalTriangles() const { return triangles; }
        unsigned long GetTotalVertices() const { return vertices; }
        unsigned long GetTotalDrawCalls() const { return drawCalls; }

        u32 GetWidth() const { return width; }
        u32 GetHeight() const { return height; }

        void Init();
        void Release();
        void Resize(u32 w, u32 h);
};

class Device
{
private:
    SDL_Window *window;
    SDL_GLContext glContext;
    int width;
    int height;
    std::string title;
    bool isRunning;
    SDL_Event event;
    double m_current;
    double m_previous;
    double m_update;
    double m_draw;
    double m_frame;
    double m_target;

    bool initSDL();
    bool initGL(bool vzync);
    // Prevent copying
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

public:
    Device();
    ~Device();

    bool Init(const char *windowTitle = "OpenGL Device",
              int windowWidth = 800,
              int windowHeight = 600, bool vzync = true);
    void Close();

    // Getters
    SDL_Window *GetSDLWindow() const { return window; }
    SDL_GLContext GetGLContext() const { return glContext; }
    int GetWidth() const;
    int GetHeight() const;

    bool Running();

    void Update();

    // Device operations
    void Swap();
    void SetTitle(const char *newTitle);
    void SetSize(int newWidth, int newHeight);
    void Quit() { isRunning = false; }

    // Timer
    void Wait(float ms);
    void SetTargetFPS(int fps);
    int GetFPS(void);
    float GetFrameTime(void);
    double GetTime(void);
    u32 GetTicks(void);
};

class Input
{
private:
    static std::unordered_map<SDL_Keycode, bool> currentKeys;
    static std::unordered_map<SDL_Keycode, bool> previousKeys;
    static std::unordered_map<uint8_t, bool> currentMouseButtons;
    static std::unordered_map<uint8_t, bool> previousMouseButtons;
    static int mouseX;
    static int mouseY;
    static int mouseDeltaX;
    static int mouseDeltaY;
    static bool quit;
    static int quit_key;

public:
    static void Init();
    static void Update();
    static void ProcessEvent(const SDL_Event &event);

    // Keyboard functions
    static bool IsKeyPressed(SDL_Keycode key);
    static bool IsKeyDown(SDL_Keycode key);
    static bool IsKeyReleased(SDL_Keycode key);
    static bool IsKeyUp(SDL_Keycode key);

    static bool IsMouseButtonPressed(uint8_t button);
    static bool IsMouseButtonDown(uint8_t button);
    static bool IsMouseButtonReleased(uint8_t button);
    static bool IsMouseButtonUp(uint8_t button);

    static int GetMouseX();
    static int GetMouseY();
    static void GetMousePosition(int &x, int &y);
    static int GetMouseDeltaX();
    static int GetMouseDeltaY();
    static void GetMouseDelta(int &deltaX, int &deltaY);

    static bool ShouldQuit();
    static void SetQuit(bool shouldQuit);
    static void SetQuitKey(int key);
};

class Color
{
public:
    Color() : r(255), g(255), b(255), a(255) {}

    Color(u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a) {}
    Color(u32 color) : r((color >> 24) & 0xFF), g((color >> 16) & 0xFF), b((color >> 8) & 0xFF), a(color & 0xFF) {}

    Color Lerp(const Color &rhs, float t) const;

    u32 ToUInt() const;

    void Set(u8 r, u8 g, u8 b, u8 a);


    u8 r, g, b, a;

    static const Color WHITE;

    static const Color GRAY;

    static const Color BLACK;

    static const Color RED;

    static const Color GREEN;

    static const Color BLUE;

    static const Color CYAN;

    static const Color MAGENTA;

    static const Color YELLOW;

    static const Color BLANK;
};

class Pixmap
{
public:
    Pixmap();
    ~Pixmap();
    Pixmap(int w, int h, int components);
    Pixmap(int w, int h, int components, unsigned char *data);

    Pixmap(const Pixmap &image, const Rectangle &crop);
    Pixmap(const Pixmap &other) = delete;
    Pixmap &operator=(const Pixmap &other) = delete;
    Pixmap(Pixmap &&) = delete;

    void SetPixel(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a);
    void SetPixel(u32 x, u32 y, u32 rgba);

    u32 GetPixel(u32 x, u32 y) const;
    Color GetPixelColor(u32 x, u32 y) const;

    void Fill(u8 r, u8 g, u8 b, u8 a);
    void Fill(u32 rgba);

    bool Save(const char *file_name);

    void Clear();

    bool Load(const char *file_name);
    bool LoadFromMemory(const unsigned char *buffer, unsigned int bytesRead);

    void FlipVertical();
    void FlipHorizontal();

    unsigned char *pixels;
    int components;
    int width;
    int height;
};

class Shader
{
public:
    Shader();
    virtual ~Shader();

    bool Create(const char *vShaderCode, const char *fShaderCode);
    bool Create(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode);
    bool Load(const char *vShaderCode, const char *fShaderCode);
    bool Load(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode);
    bool operator==(const Shader &other) const { return m_program == other.m_program; }
    bool operator!=(const Shader &other) const { return m_program != other.m_program; }

    u32 GetID() { return m_program; }

    void Bind() const;

    void SetInt(const std::string &name, int value);

    void SetMatrix4(const std::string &name, const float *value);
    void SetMatrix3(const std::string &name, const float *value);

    void SetFloat(const std::string &name, float v);
    void SetFloat(const std::string &name, float x, float y);
    void SetFloat(const std::string &name, float x, float y, float z);
    void SetFloat(const std::string &name, float x, float y, float z, float w);

    void Release();

    void print();

    bool findUniform(const std::string &name) const;

    int getUniform(const std::string &name);
    int getAttribute(const std::string &name);

    int getUniformLocation(const std::string &uniformName) const;
    int getAttribLocation(const std::string &attribName) const;

    bool addUniform(const std::string &name);
    bool addAttribute(const std::string &name);

    bool ContainsUniform(const std::string &name) const;

    void LoadDefaults();

private:
    u32 m_program;
    int m_numAttributes;
    int m_numUniforms;
    int success;

private:
    void checkCompileErrors(unsigned int shader, const std::string &type);

    Shader &operator=(const Shader &other) = delete;
    Shader(const Shader &other) = delete;
};

class Texture
{
public:
    Texture();
    virtual ~Texture();

    u32 GetID() { return id; }

    FilterMode GetMinFilter() const { return MinificationFilter; }
    FilterMode GetMagFilter() const { return MagnificationFilter; }
    WrapMode GetWrapS() const { return HorizontalWrap; }
    WrapMode GetWrapT() const { return VerticalWrap; }

    int GetWidth() { return width; }
    int GetHeight() { return height; }

    void SetMinFilter(FilterMode filter);
    void SetMagFilter(FilterMode filter);
    void SetWrapS(WrapMode mode);
    void SetWrapT(WrapMode mode);
    void SetAnisotropicFiltering(float level = -1.0f);

    void Bind(u32 unit = 0);
    void Update(const Pixmap &pixmap);
    void Update(const unsigned char *buffer, u16 components, int width, int height);

    virtual void Release();

protected:
    u32 id;
    WrapMode HorizontalWrap;
    WrapMode VerticalWrap;
    FilterMode MinificationFilter;
    FilterMode MagnificationFilter;
    float MaxAnisotropic;
    int width;
    int height;

    void createTexture();

    Texture &operator=(const Texture &other) = delete;
    Texture(const Texture &other) = delete;
    Texture(Texture &&) = delete;
    Texture &operator=(Texture &&) = delete;
};

class Texture2D : public Texture
{
public:
    Texture2D();

    Texture2D(int w, int h);
    Texture2D(const Pixmap &pixmap);
    Texture2D(const char *file_name);

    bool Load(const Pixmap &pixmap);
    bool Load(const char *file_name);
    bool LoadFromMemory(const unsigned char *buffer, u16 components, int width, int height);
    u32 GetID() { return id; }

    

    Texture2D(const Texture2D &) = delete;
    Texture2D &operator=(const Texture2D &) = delete;
    Texture2D(Texture2D &&) = delete;
    Texture2D &operator=(Texture2D &&) = delete;

private:
    friend class Texture;
    s32 components{0};
    static Texture2D *defaultTexture;
};

class RenderTexture 
{
private:
    u32 framebuffer;
    u32 depthBuffer;               
    u32 depthTexture;              
    std::vector<u32> colorBuffers;  
    u32 width;
    u32 height;
public:
    RenderTexture();
    
    bool Init(int width, int height);

    void addColorBuffer();
    void addDepthBuffer();
    void addDepthTexture();

    bool Process();

    void Begin();
    void End();




    void Release() ;

    u32 GetFramebuffer() { return framebuffer; }
    u32 GetDepthBuffer() { return depthBuffer; }
    u32 GetDepthTexture() { return depthTexture; }
    u32 GetColorBuffer(int index);
    u32 GetWidth() { return width; }
    u32 GetHeight() { return height; }
};

class Assets
{
private:
    static Assets *instance;
    std::unordered_map<std::string, Texture2D *> textures;
    std::unordered_map<std::string, Shader *> shaders;
    Texture2D *defaultTexture= nullptr;
    
public:
        static Assets *InstancePtr();
        static Assets& Instance();

        Texture2D *GetDefaultTexture();

        void SetFlipTexture(bool flip);

        bool ContainsTexture(const std::string &name);
        bool ContainsShader(const std::string &name);
        
        Shader* LoadShader(const std::string &name, const std::string &vertexShader, const std::string &fragmentShader);
        Shader* CreateShader(const std::string &name, const std::string &vertexShader, const std::string &fragmentShader);
        Shader* GetShader(const std::string &name);

        Texture2D* LoadTexture(const std::string &file_name);
        Texture2D* GetTexture(const std::string &name); 

        void Init();
        void Release();
};


