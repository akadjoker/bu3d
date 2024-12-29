#include "Core.hpp"
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

#if defined(PLATFORM_DESKTOP) && defined(_WIN32) && (defined(_MSC_VER) || defined(__TINYC__))

#include "wdirent.h" // Required for: DIR, opendir(), closedir()
#else
#include <dirent.h> // Required for: DIR, opendir(), closedir()
#endif

#if defined(_WIN32)
#include <direct.h>    // Required for: _getch(), _chdir()
#define GETCWD _getcwd // NOTE: MSDN recommends not to use getcwd(), chdir()
#define CHDIR _chdir
#include <io.h> // Required for: _access() [Used in FileExists()]
#else
#include <unistd.h> // Required for: getch(), chdir() (POSIX), access()
#define GETCWD getcwd
#define CHDIR chdir
#endif

#ifdef PLATFORM_WIN

#define CONSOLE_COLOR_RESET ""
#define CONSOLE_COLOR_GREEN ""
#define CONSOLE_COLOR_RED ""
#define CONSOLE_COLOR_PURPLE ""

#else

#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"

#endif

double GetTime()
{
    return static_cast<double>(SDL_GetTicks()) / 1000;
}


static void LogMessage(int level, const char *msg, va_list args)
{
    time_t rawTime;
    struct tm *timeInfo;
    char timeBuffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), msg, args);

    switch (level)
    {
    case 0:
        SDL_LogInfo(0, "%s%s: %s%s", CONSOLE_COLOR_GREEN, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 1:
        SDL_LogError(0, "%s%s: %s%s", CONSOLE_COLOR_RED, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 2:
        SDL_LogWarn(0, "%s%s: %s%s", CONSOLE_COLOR_PURPLE, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    }
}

void Utils::LogError(const char *msg, ...)
{

    va_list args;
    va_start(args, msg);
    LogMessage(1, msg, args);
    va_end(args);
}

void Utils::LogWarning(const char *msg, ...)
{

    va_list args;
    va_start(args, msg);
    LogMessage(2, msg, args);
    va_end(args);
}

void Utils::LogInfo(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    LogMessage(0, msg, args);
    va_end(args);
}

char *Utils::LoadTextFile(const char *fileName)
{
    char *text = NULL;

    SDL_RWops *textFile = SDL_RWFromFile(fileName, "rt");
    if (textFile != NULL)
    {
        unsigned int size = (int)SDL_RWsize(textFile);
        if (size > 0)
        {
            text = (char *)malloc((size + 1) * sizeof(char));
            unsigned int count = (unsigned int)SDL_RWread(textFile, text, sizeof(char), size);
            if (count < size)
                text = (char *)realloc(text, count + 1);
            text[count] = '\0';

            SDL_LogInfo(0, "FILEIO: [%s] Text file loaded successfully", fileName);
        }
        else
            SDL_LogError(0, "FILEIO: [%s] Failed to read text file", fileName);

        SDL_RWclose(textFile);
    }
    else
        SDL_LogError(0, "FILEIO: [%s] Failed to open text file", fileName);

    return text;
}

unsigned char *Utils::LoadDataFile(const char *fileName, unsigned int *bytesRead)
{
    unsigned char *data = NULL;
    *bytesRead = 0;

    SDL_RWops *file = SDL_RWFromFile(fileName, "rb");

    if (file != NULL)
    {
        unsigned int size = (int)SDL_RWsize(file);

        if (size > 0)
        {
            data = (unsigned char *)malloc(size * sizeof(unsigned char));

            unsigned int count = (unsigned int)SDL_RWread(file, data, sizeof(unsigned char), size);
            *bytesRead = count;

            SDL_LogInfo(0, "FILEIO: [%s] File loaded successfully", fileName);
        }
        else
            SDL_LogError(0, "FILEIO: [%s] Failed to read file", fileName);
        SDL_RWclose(file);
    }
    else
        SDL_LogError(0, "FILEIO: [%s] Failed to open file", fileName);

    return data;
}

bool Utils::FileExists(const char *fileName)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1)
        result = true;
#else
    if (access(fileName, F_OK) != -1)
        result = true;
#endif
    return result;
}

bool Utils::IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;

    const char *fileExt = strrchr(fileName, '.');

    if (fileExt != NULL)
    {
        if (strcmp(fileExt, ext) == 0)
            result = true;
    }

    return result;
}

bool Utils::DirectoryExists(const char *dirPath)
{
    bool result = false;

    DIR *dir = opendir(dirPath);
    if (dir != NULL)
    {
        result = true;
        closedir(dir);
    }

    return result;
}

const char *Utils::GetFileExtension(const char *fileName)
{
    const char *fileExt = strrchr(fileName, '.');

    if (fileExt != NULL)
        return fileExt;

    return NULL;
}

const char *Utils::GetFileName(const char *filePath)
{
    const char *fileName = strrchr(filePath, '/');

    if (fileName != NULL)
        return fileName + 1;

    return filePath;
}

const char *Utils::GetFileNameWithoutExt(const char *filePath)
{
    static char baseName[256];
    strcpy(baseName, GetFileName(filePath));

    char *dot = strrchr(baseName, '.');
    if (dot)
        *dot = '\0';

    return baseName;
}

const char *Utils::GetDirectoryPath(const char *filePath)
{
    static char dirPath[256];
    strcpy(dirPath, filePath);

    char *lastSlash = strrchr(dirPath, '/');

    if (lastSlash != NULL)
        lastSlash[1] = '\0';
    else
        dirPath[0] = '\0';

    return dirPath;
}

const Color Color::WHITE;
const Color Color::GRAY(128, 128, 128);
const Color Color::BLACK(0, 0, 0);
const Color Color::RED(255, 0, 0);
const Color Color::GREEN(0, 255, 0);
const Color Color::BLUE(0, 0, 255);
const Color Color::CYAN(0, 255, 255);
const Color Color::MAGENTA(255, 0, 255);
const Color Color::YELLOW(255, 255, 0);
const Color Color::BLANK(0, 0, 0);

Color Color::Lerp(const Color &rhs, float t) const
{
    return Color((r + rhs.r) * t, (g + rhs.g) * t, (b + rhs.b) * t, (a + rhs.a) * t);
  
}
u32 Color::ToUInt() const
{
    unsigned r = (unsigned)Clamp(((int)(this->r * 255.0f)), 0, 255);
    unsigned g = (unsigned)Clamp(((int)(this->g * 255.0f)), 0, 255);
    unsigned b = (unsigned)Clamp(((int)(this->b * 255.0f)), 0, 255);
    unsigned a = (unsigned)Clamp(((int)(this->a * 255.0f)), 0, 255);
    return (a << 24) | (b << 16) | (g << 8) | r;
}

void Color::Set(u8 r, u8 g, u8 b, u8 a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Device::Device() : window(nullptr),
                   glContext(nullptr),
                   width(800),
                   height(600),
                   title("OpenGL Device"),
                   isRunning(true)
{
    m_current = 0;
    m_previous = 0;
    m_update = 0;
    m_draw = 0;
    m_frame = 0;
    // SetTargetFPS(60);

    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;
    m_frame = m_update + m_draw;
}

Device::~Device()
{
}

void Device::Wait(float ms)
{
    Uint32 time = static_cast<Uint32>(ms * 1000.0f);
    SDL_Delay(time);
}

int Device::GetFPS(void)
{
#define FPS_CAPTURE_FRAMES_COUNT 30   // 30 captures
#define FPS_AVERAGE_TIME_SECONDS 0.5f // 500 millisecondes
#define FPS_STEP (FPS_AVERAGE_TIME_SECONDS / FPS_CAPTURE_FRAMES_COUNT)

    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = {0};
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    if (fpsFrame == 0)
        return 0;

    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1) % FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame / FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }

    return (int)roundf(1.0f / average);
}

void Device::SetTargetFPS(int fps)
{
    if (fps < 1)
        m_target = 0.0;
    else
        m_target = 1.0 / (double)fps;
}

float Device::GetFrameTime(void)
{
    return (float)m_frame;
}

double Device::GetTime(void)
{
    unsigned int ms = SDL_GetTicks(); // Elapsed time in milliseconds since SDL_Init()
    double time = (double)ms / 1000;
    return time;
}

u32 Device::GetTicks(void)
{
    return SDL_GetTicks();
}
bool Device::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    return true;
}

void glDebugOutput(u32 source,
                   u32 type,
                   u32 id,
                   u32 severity,
                   s32 length,
                   const char *message,
                   const void *userParam)
{
    (void)length;
    (void)userParam;
    (void)severity;
    GLuint ignore_ids[1] = {131185}; // "will use video memory..."

    for (int i = 0; i < (int)ARRAY_SIZE_IN_ELEMENTS(ignore_ids); i++)
    {
        if (id == ignore_ids[i])
        {
            return;
        }
    }

    //  SDL_LogWarn(1,"!!! Debug callback !!!\n");
    SDL_LogDebug(2, "Debug message: id %d, %s", id, message);

    //   SDL_LogWarn(1,"Message source: ");
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        SDL_LogWarn(1, "API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        SDL_LogWarn(1, "Device System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        SDL_LogWarn(1, "Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        SDL_LogWarn(1, "Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        SDL_LogWarn(1, "Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        SDL_LogWarn(1, "Other");
        break;
    }

    //  printf("Error type: ");
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        SDL_LogError(2, "Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        SDL_LogError(2, "Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        SDL_LogError(2, "Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        SDL_LogError(2, "Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        SDL_LogError(2, "Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        SDL_LogError(2, "Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        SDL_LogError(2, "Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        SDL_LogError(2, "Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        SDL_LogError(2, "Other");
        break;
    }
}
bool Device::initGL(bool vzync)
{
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        SDL_Log("OpenGL context creation failed: %s", SDL_GetError());
        return false;
    }

    if (vzync)
    {
        SetTargetFPS(60);
        SDL_GL_SetSwapInterval(1); // Enable VSync
    }
    else
    {
        SetTargetFPS(INT_MAX);
        SDL_GL_SetSwapInterval(0);
    }

    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);


    //   glDebugMessageCallback(glDebugOutput, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);

    Utils::LogInfo("Vendor  :  %s", glGetString(GL_VENDOR));
    Utils::LogInfo("Renderer:  %s", glGetString(GL_RENDERER));
    Utils::LogInfo("Version :  %s", glGetString(GL_VERSION));

    Utils::LogInfo("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return true;
}

int Device::GetWidth() const
{

    return width;
}
int Device::GetHeight() const
{
    return height;
}

bool Device::Init(const char *windowTitle, int windowWidth, int windowHeight, bool vzync)
{
    title = windowTitle;
    width = windowWidth;
    height = windowHeight;

    if (!initSDL())
    {
        return false;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        SDL_Log("Device creation failed: %s", SDL_GetError());
        return false;
    }

    if (!initGL(vzync))
    {
        return false;
    }
    Driver::Instance().Init();
    Driver::Instance().Resize(width, height);
    Assets::Instance().Init();
    MeshManager::Instance().Init();
    Input::Init();
    return true;
}

void Device::Close()
{

  
    if (glContext)
    {
        Driver::Instance().Release();
        Assets::Instance().Release();
        MeshManager::Instance().Release();
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Utils::LogInfo("Quit :) By! By!");
    SDL_Quit();
}

void Device::Swap()
{
    SDL_GL_SwapWindow(window);

    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;
    m_frame = m_update + m_draw;

    if (m_frame < m_target)
    {
        Wait((float)(m_target - m_frame));

        m_current = GetTime();
        double waitTime = m_current - m_previous;
        m_previous = m_current;

        m_frame += waitTime;
    }

    Input::Update();
}

void Device::SetTitle(const char *newTitle)
{
    title = newTitle;
    SDL_SetWindowTitle(window, title.c_str());
}

void Device::SetSize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    SDL_SetWindowSize(window, width, height);
    Driver::Instance().Resize(width, height);
}

void Device::Update()
{
    m_current = GetTime();
    m_update = m_current - m_previous;
    m_previous = m_current;


    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {
        case SDL_QUIT:
        {
            isRunning = false;
            return;
        }
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
            {
                width = event.window.data1;
                height = event.window.data2;
                Driver::Instance().Resize(width, height);
                break;
            }
            }
            break;
        }
        }
        Input::ProcessEvent(event);
    }
}
bool Device::Running()
{
    Update();
    return (isRunning && !Input::ShouldQuit());
}

Driver * Driver::instance = nullptr;



Driver *Driver::InstancePtr()
{
    if (instance == nullptr)
    {
        instance = new Driver();
    }
    return instance;
}

Driver &Driver::Instance()
{
    if (instance == nullptr)
    {
        instance = new Driver();
    }
    return *instance;
}

void Driver::SetTransform(u8 mode, const Mat4 &m)
{
    DEBUG_BREAK_IF(mode > 2);
    matrix[mode] = m;
}

void Driver::EnableBlend(bool enable)
{
    if (enable)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void Driver::EnableDepthTest(bool enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void Driver::EnableCullFace(bool enable)
{
    if (enable)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Driver::EnableColorMask(bool enable)
{
    if (enable)
    {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else
    {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
}

void Driver::SetBlend(int src,int dst)
{
    glBlendFunc(src, dst);
}

void Driver::Init()
{
    matrix[0] = Mat4::Identity();
    matrix[1] = Mat4::Identity();
    matrix[2] = Mat4::Identity();
    viewProjection = matrix[0] * matrix[1];
    vertices = 0;
    triangles = 0;
    width = 0;
    height = 0;
    screenQuad.Init();
}

void Driver::Release()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
    screenQuad.Release();
}

void Driver::SetBlendMode(u8 mode)
{
    switch (mode)
    {
        case BLEND_NORMAL:
            SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BLEND_ADDITIVE:
            SetBlend(GL_SRC_ALPHA, GL_ONE);
            break;
        case BLEND_MULTIPLY:
            SetBlend(GL_DST_COLOR, GL_ZERO);
            break;
        case BLEND_SUBTRACT:
            SetBlend(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BLEND_SCREEN:
            SetBlend(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
            break;
    
    }
}  

void Driver::SetViewport(float x, float y, float width, float height)
{
    glViewport(x, y, width, height);
}

void Driver::SetClearColor(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
}

void Driver::Clear(int mode)
{
    glClear(mode);
}

void Driver::UpdateFrustum()
{
    viewProjection = matrix[0] * matrix[1];
    frustum.update(viewProjection);
    vertices = 0;
    triangles = 0;
    drawCalls = 0;
    cameraPosition = matrix[0].getPosition();
    cameraRotation = matrix[0].getRotationInDegrees();

}

bool Driver::IsInFrustum(const Vec3 &point)
{
   return frustum.containsPoint(point);
}

bool Driver::IsInFrustum(const Vec3 &min, const Vec3 &max)
{
    return  frustum.intersectsBox(min, max);
}

bool Driver::IsInFrustum(const BoundingBox &box)
{
    return  frustum.intersectsBox(box);
}

static int calculatePrimitiveCount(int mode, int count)
{
    if (mode == GL_TRIANGLES)
        return count / 3;
    else if (mode == GL_TRIANGLE_STRIP)
        return count - 2;
    else if (mode == GL_LINES)
        return count / 2;
    return 0;
}

void Driver::DrawArrays(int mode, int first,int vertexCount)
{
    vertices += vertexCount;
    triangles += calculatePrimitiveCount(mode, vertexCount);

    glDrawArrays(mode, first, vertexCount);
    drawCalls++;
}

void Driver::DrawElements(int mode, int indexCount, int indexType, const void *indices)
{
    vertices += indexCount;
    triangles += calculatePrimitiveCount(mode, indexCount);
    glDrawElements(mode, indexCount, indexType, indices);
    drawCalls++;
}

void Driver::Resize(u32 w, u32 h)
{
    width = w;
    height = h;
}

std::unordered_map<SDL_Keycode, bool> Input::currentKeys;
std::unordered_map<SDL_Keycode, bool> Input::previousKeys;
std::unordered_map<uint8_t, bool> Input::currentMouseButtons;
std::unordered_map<uint8_t, bool> Input::previousMouseButtons;
int Input::mouseX = 0;
int Input::mouseY = 0;
int Input::mouseDeltaX = 0;
int Input::mouseDeltaY = 0;
bool Input::quit = false;
int Input::quit_key = SDLK_ESCAPE;

void Input::Init()
{
    currentKeys.clear();
    previousKeys.clear();
    currentMouseButtons.clear();
    previousMouseButtons.clear();
    mouseX = 0;
    mouseY = 0;
    mouseDeltaX = 0;
    mouseDeltaY = 0;
    quit = false;
}

void Input::Update()
{
    // Copy current state to previous state
    previousKeys = currentKeys;
    previousMouseButtons = currentMouseButtons;

    // Reset delta movement
    mouseDeltaX = 0;
    mouseDeltaY = 0;
}

void Input::ProcessEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        currentKeys[event.key.keysym.sym] = true;
        break;

    case SDL_KEYUP:
        currentKeys[event.key.keysym.sym] = false;
        // SDL_Log( "key %d"   ,event.key.keysym.sym);
        if (Input::quit_key == event.key.keysym.sym)
            quit = true;
        break;

    case SDL_MOUSEBUTTONDOWN:
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        currentMouseButtons[event.button.button] = true;
        break;

    case SDL_MOUSEBUTTONUP:
        currentMouseButtons[event.button.button] = false;

        break;

    case SDL_MOUSEMOTION:
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        mouseDeltaX = event.motion.xrel;
        mouseDeltaY = event.motion.yrel;

        break;

    case SDL_QUIT:
        quit = true;
        break;
    }
}

bool Input::IsKeyPressed(SDL_Keycode key)
{
    return currentKeys[key] && !previousKeys[key];
}

bool Input::IsKeyDown(SDL_Keycode key)
{
    return currentKeys[key];
}

bool Input::IsKeyReleased(SDL_Keycode key)
{
    return !currentKeys[key] && previousKeys[key];
}

bool Input::IsKeyUp(SDL_Keycode key)
{
    return !currentKeys[key];
}

bool Input::IsMouseButtonPressed(uint8_t button)
{
    return currentMouseButtons[button] && !previousMouseButtons[button];
}

bool Input::IsMouseButtonDown(uint8_t button)
{
    return currentMouseButtons[button];
}

bool Input::IsMouseButtonReleased(uint8_t button)
{
    return !currentMouseButtons[button] && previousMouseButtons[button];
}

bool Input::IsMouseButtonUp(uint8_t button)
{
    return !currentMouseButtons[button];
}

int Input::GetMouseX()
{
    return mouseX;
}

int Input::GetMouseY()
{
    return mouseY;
}

void Input::GetMousePosition(int &x, int &y)
{
    x = mouseX;
    y = mouseY;
}

int Input::GetMouseDeltaX()
{
    return mouseDeltaX;
}

int Input::GetMouseDeltaY()
{
    return mouseDeltaY;
}

void Input::GetMouseDelta(int &deltaX, int &deltaY)
{
    deltaX = mouseDeltaX;
    deltaY = mouseDeltaY;
}

bool Input::ShouldQuit()
{
    return quit;
}

void Input::SetQuit(bool shouldQuit)
{
    quit = shouldQuit;
}

void Input::SetQuitKey(int key)
{
    quit_key = key;
}

Pixmap::Pixmap()
{

    width = 0;
    height = 0;
    components = 0;
    pixels = nullptr;
}

Pixmap::~Pixmap()
{
    if (pixels)
    {
        free(pixels);
    }
}

Pixmap::Pixmap(const Pixmap &image, const Rectangle &crop)
{
    width = crop.width;
    height = crop.height;
    components = image.components;
    pixels = (unsigned char *)malloc(width * height * components);
    for (int y = (int)crop.y, offsetSize = 0; y < (int)(crop.y + crop.height); y++)
    {
        memcpy(pixels + offsetSize, ((unsigned char *)image.pixels) + (y * image.width + (int)crop.x) * components, (int)crop.width * components);
        offsetSize += ((int)crop.width * components);
    }
}

Pixmap::Pixmap(int w, int h, int components)
{

    width = w;
    height = h;
    this->components = components;
    pixels = (unsigned char *)malloc(w * h * components);
}

Pixmap::Pixmap(int w, int h, int components, unsigned char *data)
{

    pixels = (unsigned char *)malloc(w * h * components);
    width = w;
    height = h;
    this->components = components;
    memcpy(pixels, data, w * h * components);
}

void Pixmap::Clear()
{
    if (pixels)
    {
        for (int i = 0; i < width * height * components; i++)
        {
            pixels[i] = 0;
        }
    }
}

void Pixmap::SetPixel(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a)
{

    if (x >= (u32)width || y >= (u32)height)
        return;

    if (components == 1)
    {
        // Vec3 coln((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
        // unsigned char gray = (unsigned char)((coln.x * 0.299f + coln.y * 0.587f + coln.z * 0.114f) * 255.0f);
        // ((u8 *)pixels)[y * width + x] = gray;
    }
    else if (components == 2)
    {
        // Vec3 coln((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
        // u8 gray = (u8)((coln.x * 0.299f + coln.y * 0.587f + coln.z * 0.114f) * 255.0f);

        // ((u8 *)pixels)[(y * width + x) * 2] = gray;
        // ((u8 *)pixels)[(y * width + x) * 2 + 1] = a;
    }
    else if (components == 3)
    {

        ((u8 *)pixels)[(y * width + x) * 3] = r;
        ((u8 *)pixels)[(y * width + x) * 3 + 1] = g;
        ((u8 *)pixels)[(y * width + x) * 3 + 2] = b;
    }
    else if (components == 4)
    {
        ((u8 *)pixels)[(y * width + x) * 4] = r;
        ((u8 *)pixels)[(y * width + x) * 4 + 1] = g;
        ((u8 *)pixels)[(y * width + x) * 4 + 2] = b;
        ((u8 *)pixels)[(y * width + x) * 4 + 3] = a;
    }
}

void Pixmap::SetPixel(u32 x, u32 y, u32 rgba)
{

    if (x >= (u32)width || y >= (u32)height)
        return;

    u8 r = rgba;
    u8 g = rgba >> 8;
    u8 b = rgba >> 16;
    u8 a = rgba >> 24;
    SetPixel(x, y, r, g, b, a);
}

u32 Pixmap::GetPixel(u32 x, u32 y) const
{

    if (x >= (u32)width || y >= (u32)height)
        return 0;

    if (components == 1)
    {
        return pixels[y * width + x];
    }
    else if (components == 2)
    {
        return pixels[(y * width + x) * 2] | (pixels[(y * width + x) * 2 + 1] << 8);
    }
    else if (components == 3)
    {
        return pixels[(y * width + x) * 3] | (pixels[(y * width + x) * 3 + 1] << 8) | (pixels[(y * width + x) * 3 + 2] << 16);
    }
    else if (components == 4)
    {
        return pixels[(y * width + x) * 4] | (pixels[(y * width + x) * 4 + 1] << 8) | (pixels[(y * width + x) * 4 + 2] << 16) | (pixels[(y * width + x) * 4 + 3] << 24);
    }

    return 0;
}

Color Pixmap::GetPixelColor(u32 x, u32 y) const
{

    Color color = Color::BLACK;

    if ((x < (u32)width) && (y < (u32)height))
    {

        if (components == 1)
        {
            color.r = (u8)((u8 *)pixels)[y * width + x];
            color.g = (u8)((u8 *)pixels)[y * width + x];
            color.b = (u8)((u8 *)pixels)[y * width + x];
            color.a = 255;
        }
        else if (components == 2)
        {
            color.r = (u8)((u8 *)pixels)[(y * width + x) * 2];
            color.g = (u8)((u8 *)pixels)[(y * width + x) * 2];
            color.b = (u8)((u8 *)pixels)[(y * width + x) * 2];
            color.a = (u8)((u8 *)pixels)[(y * width + x) * 2 + 1];
        }
        else if (components == 3)
        {
            color.r = (u8)((u8 *)pixels)[(y * width + x) * 3];
            color.g = (u8)((u8 *)pixels)[(y * width + x) * 3 + 1];
            color.b = (u8)((u8 *)pixels)[(y * width + x) * 3 + 2];
            color.a = 255;
        }
        else if (components == 4)
        {
            color.r = (u8)((u8 *)pixels)[(y * width + x) * 4];
            color.g = (u8)((u8 *)pixels)[(y * width + x) * 4 + 1];
            color.b = (u8)((u8 *)pixels)[(y * width + x) * 4 + 2];
            color.a = (u8)((u8 *)pixels)[(y * width + x) * 4 + 3];
        }
    }
    return color;
}

void Pixmap::Fill(u8 r, u8 g, u8 b, u8 a)
{
    for (u32 y = 0; y < (u32)height; y++)
    {
        for (u32 x = 0; x < (u32)width; x++)
        {
            SetPixel(x, y, r, g, b, a);
        }
    }
}

void Pixmap::Fill(u32 rgba)
{
    for (u32 y = 0; y < (u32)height; y++)
    {
        for (u32 x = 0; x < (u32)width; x++)
        {
            SetPixel(x, y, rgba);
        }
    }
}

bool Pixmap::Load(const char *file_name)
{

    pixels = stbi_load(file_name, &width, &height, &components, 0);

    if (pixels == nullptr)
    {
        Utils::LogError("Failed to load image: %s", file_name);
        return false;
    }

    Utils::LogInfo("Load image: %s (%d,%d) bpp:%d", file_name, width, height, components);

    return true;
}

bool Pixmap::LoadFromMemory(const unsigned char *buffer, unsigned int bytesRead)
{

    pixels = stbi_load_from_memory(buffer, bytesRead, &width, &height, &components, 0);
    if (pixels == nullptr)
    {
        Utils::LogError("Failed to load image from memory");
        return false;
    }
    return true;
}

bool Pixmap::Save(const char *file_name)
{
    if (pixels == nullptr)
    {
        Utils::LogError("Failed to save image: %s", file_name);
        return false;
    }

    // if (System::Instance().IsFileExtension(file_name, ".bmp"))
    //     return stbi_write_bmp(file_name, width, height, components, pixels);
    // else if (System::Instance().IsFileExtension(file_name, ".tga"))
    //     return stbi_write_tga(file_name, width, height, components, pixels);
    // else if (System::Instance().IsFileExtension(file_name, ".png"))
    //     return stbi_write_png(file_name, width, height, components, pixels, 0);
    // else
    // {
    //     Utils::LogError("Failed to save image: %s ", file_name);
    //     return false;
    // }
    return false;
}

void Pixmap::FlipVertical()
{
    if (pixels == nullptr)
    {
        Utils::LogError("Failed to flip image");
        return;
    }
    int rowSize = width * components;
    unsigned char *row = (unsigned char *)malloc(rowSize);
    for (int y = 0; y < height / 2; y++)
    {
        unsigned char *src = ((unsigned char *)pixels) + y * rowSize;
        unsigned char *dst = ((unsigned char *)pixels) + (height - y - 1) * rowSize;
        memcpy(row, src, rowSize);
        memcpy(src, dst, rowSize);
        memcpy(dst, row, rowSize);
    }
    free(row);
}

void Pixmap::FlipHorizontal()
{
    if (pixels == nullptr)
    {
        Utils::LogError("Failed to flip image");
        return;
    }
    int rowSize = width * components;
    unsigned char *row = (unsigned char *)malloc(rowSize);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width / 2; x++)
        {
            unsigned char *src = ((unsigned char *)pixels) + y * rowSize + x * components;
            unsigned char *dst = ((unsigned char *)pixels) + y * rowSize + (width - x - 1) * components;
            memcpy(row, src, components);
            memcpy(src, dst, components);
            memcpy(dst, row, components);
        }
    }
    free(row);
}

Shader::Shader()
{
    m_program = 0;
    m_numAttributes = 0;
    m_numUniforms = 0;
}

Shader::~Shader()
{
    Release();
}

void Shader::Bind() const
{
    glUseProgram(m_program);
}

void Shader::Release()
{
    if (m_program > 0)
    {
        Utils::LogInfo("SHADER: [ID %i] Release shader program.", m_program);
        glDeleteProgram(m_program);
    }
    m_program = 0;
}

bool Shader::Create(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode)
{
    // 2. compile shaders
    unsigned int vertex, fragment, geometry;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // geometry shader
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");

    // shader Program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glAttachShader(m_program, geometry);
    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");

    if (m_program > 0)
        Utils::LogInfo("SHADER: [ID %i] Create shader program.", m_program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
    glUseProgram(m_program);

    return true;
}

bool Shader::Create(const char *vShaderCode, const char *fShaderCode)
{
    // 2. compile shaders
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");

    if (m_program > 0)
    {

        Utils::LogInfo("SHADER: [ID %i] Create shader program.", m_program);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(m_program);

    return success;
}

bool Shader::Load(const char *vShaderCode, const char *fShaderCode)
{
    bool result = false;
    char *shaderCode = Utils::LoadTextFile(vShaderCode);
    char *fragCode = Utils::LoadTextFile(fShaderCode);
    if (shaderCode && fragCode)
    {
        result = Create(shaderCode, fragCode);
        free(shaderCode);
        free(fragCode);
    }

    return result;
}

bool Shader::Load(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode)
{
    bool result = false;
    char *shaderCode = Utils::LoadTextFile(vShaderCode);
    char *fragCode = Utils::LoadTextFile(fShaderCode);
    char *geomCode = Utils::LoadTextFile(gShaderCode);
    if (shaderCode && fragCode && geomCode)
    {
        result = Create(shaderCode, fragCode, geomCode);
        free(shaderCode);
        free(fragCode);
        free(geomCode);
    }

    return result;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string &type)
{

    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Utils::LogError("SHADER: Error Compile %s Msg: %s", type.c_str(), infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Utils::LogError("SHADER: Error Link %s Msg: %s", type.c_str(), infoLog);
        }
    }
}

int Shader::getUniform(const std::string &name)
{
    return getUniformLocation(name);
}
int Shader::getAttribute(const std::string &name)
{
    return getAttribLocation(name);
}
int Shader::getUniformLocation(const std::string &uniformName) const
{
    int location = glGetUniformLocation(m_program, uniformName.c_str());
    if (location == -1)
        Utils::LogError("SHADER: [ID %i] Failed to find shader uniform: %s", m_program, uniformName.c_str());
    return location;
}
int Shader::getAttribLocation(const std::string &attribName) const
{
    int location = glGetAttribLocation(m_program, attribName.c_str());
    if (location == -1)
        Utils::LogError("SHADER: [ID %i] Failed to find shader attribute: %s", m_program, attribName.c_str());
    return location;
}
bool Shader::addUniform(const std::string &name)
{
    int location = -1;
    location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1)
    {
        Utils::LogError("SHADER: [ID %i] Failed to find shader uniform: %s", m_program, name.c_str());
        return false;
    }

    //  Utils::LogInfo( "SHADER: [ID %i] shader uniform (%s) set at location: %i", m_program, name.c_str(), location);

    return true;
}
bool Shader::addAttribute(const std::string &name)
{
    int location = glGetAttribLocation(m_program, name.c_str());
    if (location == -1)
    {
        Utils::LogError("SHADER: [ID %i] Failed to find shader attribute: %s", m_program, name.c_str());
        return false;
    }

    return true;
}

void Shader::SetInt(const std::string &name, int value)
{
    int id = getUniform(name);
    if (id != -1)
        glUniform1i(id, value);
}

void Shader::SetMatrix4(const std::string &name, const float *value)
{
    int id = getUniform(name);
    if (id != -1)
        glUniformMatrix4fv(id, 1, GL_FALSE, value);
}

void Shader::SetMatrix3(const std::string &name, const float *value)
{
    int id = getUniform(name);
    if (id != -1)
        glUniformMatrix3fv(id, 1, GL_FALSE, value);
}

void Shader::SetFloat(const std::string &name, float v)
{
    int id = getUniform(name);
    if (id != -1)
        glUniform1f(id, v);
}
void Shader::SetFloat(const std::string &name, float x, float y)
{
    int id = getUniform(name);
    if (id != -1)
        glUniform2f(id, x, y);
}
void Shader::SetFloat(const std::string &name, float x, float y, float z)
{
    int id = getUniform(name);
    if (id != -1)
        glUniform3f(id, x, y, z);
}
void Shader::SetFloat(const std::string &name, float x, float y, float z, float w)
{
    int id = getUniform(name);
    if (id != -1)
        glUniform4f(id, x, y, z, w);
}

void Shader::print()
{
    Utils::LogInfo("[SHADER]  Id(%d) Num Attributes(%d)  Num Uniforms (%d)", m_program, m_numAttributes, m_numUniforms);
}
void Shader::LoadDefaults()
{
    GLint numActiveAttribs = 0;
    GLint maxAttribNameLength = 0;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
    m_numAttributes = numActiveAttribs;
    for (GLint attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;
        glGetActiveAttrib(m_program, attrib, sizeof(name) - 1, &namelen, &num, &type, name);
        name[namelen] = 0;
        addAttribute(std::string((char *)&name[0]));
        glBindAttribLocation(m_program, attrib, (char *)&name[0]);
        Utils::LogInfo("SHADER: [ID %i] Active attribute (%s) set at location: %i", m_program, name, attrib);
    }
    // Get available shader uniforms
    int uniformCount = -1;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniformCount);
    m_numUniforms = uniformCount;

    for (int i = 0; i < uniformCount; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;
        glGetActiveUniform(m_program, i, sizeof(name) - 1, &namelen, &num, &type, name);
        name[namelen] = 0;
        addUniform(std::string((char *)&name[0]));
        Utils::LogInfo("SHADER: [ID %i] Active uniform (%s) set at location: %i", m_program, name, glGetUniformLocation(m_program, name));
    }
}

Texture::Texture() : HorizontalWrap(WrapMode::Repeat),
                     VerticalWrap(WrapMode::Repeat),
                     MinificationFilter(FilterMode::NearestMipLinear),
                     MagnificationFilter(FilterMode::Linear),
                     MaxAnisotropic(0.0f)
{

    id = 0;
    width = 0;
    height = 0;
}

Texture::~Texture()
{
  
  
}

void Texture::SetMinFilter(FilterMode filter)
{
    this->MinificationFilter = filter;
    if (id != 0)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinificationFilter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetMagFilter(FilterMode filter)
{
    this->MagnificationFilter = filter;
    if (id != 0)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagnificationFilter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetWrapS(WrapMode mode)
{
    this->HorizontalWrap = mode;
    if (id != 0)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, HorizontalWrap);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetWrapT(WrapMode mode)
{
    this->VerticalWrap = mode;
    if (id != 0)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, VerticalWrap);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::SetAnisotropicFiltering(float level)
{
    this->MaxAnisotropic = level;
    if (id != 0)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropic);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::Release()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
        Utils::LogInfo("Texture: [ID %i] Release", id);
        id = 0;
    }
}

void Texture::createTexture()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, HorizontalWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, VerticalWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinificationFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagnificationFilter);
    if (MaxAnisotropic > 0.0f)
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropic);
    }
}

void Texture::Bind(u32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::Update(const Pixmap &pixmap)
{
    if (pixmap.pixels)
    {
        u32 components = pixmap.components;
        u32 width = pixmap.width;
        u32 height = pixmap.height;

        GLenum format = GL_RGBA;
        GLenum glFormat = GL_RGBA;

        switch (components)
        {
        case 1:
        {
            format = GL_R8;
            glFormat = GL_RED;
            break;
        }
        case 2:
        {
            format = GL_RG8;
            glFormat = GL_RG;
            break;
        }
        case 3:
        {
            format = GL_RGB;
            glFormat = GL_RGB;

            break;
        }
        case 4:
        {
            format = GL_RGBA8;
            glFormat = GL_RGBA;
            break;
        }
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, glFormat, GL_UNSIGNED_BYTE, pixmap.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
void Texture::Update(const unsigned char *buffer, u16 components, int width, int height)
{
    if (buffer)
    {
        GLenum format = GL_RGBA;
        GLenum glFormat = GL_RGBA;
        switch (components)
        {
        case 1:
        {
            format = GL_R8;
            glFormat = GL_RED;
            break;
        }
        case 2:
        {
            format = GL_RG8;
            glFormat = GL_RG;
            break;
        }
        case 3:
        {
            format = GL_RGB;
            glFormat = GL_RGB;

            break;
        }
        case 4:
        {
            format = GL_RGBA8;
            glFormat = GL_RGBA;
            break;
        }
        }
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, glFormat, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


Texture2D::Texture2D() : Texture()
{
}

Texture2D::Texture2D(const Pixmap &pixmap) : Texture()
{

    components = pixmap.components;
    width = pixmap.width;
    height = pixmap.height;

    GLenum format = GL_RGBA;
    GLenum glFormat = GL_RGBA;
    switch (components)
    {
    case 1:
    {
        format = GL_R8;
        glFormat = GL_RED;
        break;
    }
    case 2:
    {
        format = GL_RG8;
        glFormat = GL_RG;
        break;
    }
    case 3:
    {
        format = GL_RGB;
        glFormat = GL_RGB;

        break;
    }
    case 4:
    {
        format = GL_RGBA8;
        glFormat = GL_RGBA;
        break;
    }
    }

    createTexture();

    if (pixmap.pixels)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, glFormat, GL_UNSIGNED_BYTE, pixmap.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (components == 1)
        {
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }
        else if (components == 2)
        {
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }
    }

    //  glBindTexture(GL_TEXTURE_2D, 0);
    Utils::LogInfo("TEXTURE2D: [ID %i] Create Opengl Texture2D (%d,%d) bpp:%d", id, width, height, components);
}

bool Texture2D::Load(const Pixmap &pixmap)
{
    components = pixmap.components;
    width = pixmap.width;
    height = pixmap.height;

    GLenum format = GL_RGBA;
    GLenum glFormat = GL_RGBA;
    switch (components)
    {
    case 1:
    {
        format = GL_R8;
        glFormat = GL_RED;
        break;
    }
    case 2:
    {
        format = GL_RG8;
        glFormat = GL_RG;
        break;
    }
    case 3:
    {
        format = GL_RGB;
        glFormat = GL_RGB;

        break;
    }
    case 4:
    {
        format = GL_RGBA8;
        glFormat = GL_RGBA;
        break;
    }
    }

    createTexture();

    if (pixmap.pixels)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, glFormat, GL_UNSIGNED_BYTE, pixmap.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (components == 1)
        {
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }
        else if (components == 2)
        {
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    Utils::LogInfo("TEXTURE2D: [ID %i] Create Opengl Texture2D (%d,%d) bpp:%d", id, width, height, components);
    return true;
}

bool Texture2D::Load(const char *file_name)
{

    unsigned int bytesRead;
    unsigned char *fileData = Utils::LoadDataFile(file_name, &bytesRead);
    if (!fileData)
        return false;

    unsigned char *data = stbi_load_from_memory(fileData, bytesRead, &width, &height, &components, 0);

    if (data == NULL)
    {
        Utils::LogError("Texture2D: Failed to load image: %s", file_name);
        return false;
    }

    GLenum format = GL_RGBA;
    GLenum glFormat = GL_RGBA;
    switch (components)
    {
    case 1:
    {
        format = GL_R8;
        glFormat = GL_RED;
        break;
    }
    case 2:
    {
        format = GL_RG;
        glFormat = GL_RG;
        break;
    }
    case 3:
    {
        format = GL_RGB;
        glFormat = GL_RGB;

        break;
    }
    case 4:
    {
        format = GL_RGBA8;
        glFormat = GL_RGBA;
        break;
    }
    }

    createTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (components == 1)
    {
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }
    else if (components == 2)
    {
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    //   glBindTexture(GL_TEXTURE_2D, 0);
    Utils::LogInfo("TEXTURE2D: [ID %i] Create Opengl Texture2D (%d,%d) bpp:%d", id, width, height, components);

    free(fileData);
    free(data);
    return true;
}

Texture2D::Texture2D(const char *file_name) : Texture()
{
    Load(file_name);
}

bool Texture2D::LoadFromMemory(const unsigned char *buffer, u16 components, int width, int height)
{

    this->width = width;
    this->height = height;
    this->components = components;

    GLenum format = GL_RGBA;
    switch (components)
    {
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;

        return false;
    }

    createTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);

    glGenerateMipmap(GL_TEXTURE_2D);
    if (components == 1)
    {
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }
    else if (components == 2)
    {
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    return true;
}


Assets *Assets::instance = nullptr;

Assets *Assets::InstancePtr()
{
    if (!instance)
        instance = new Assets();
    return instance;
}

Assets &Assets::Instance()
{
    if (!instance)
        instance = new Assets();
    return *instance;
}

Texture2D *Assets::GetDefaultTexture()
{
    if (!defaultTexture)
    {
        defaultTexture = new Texture2D();
        unsigned char data[4] = {255, 255, 255, 255};
        defaultTexture->LoadFromMemory(data, 4, 1, 1);
    }
    return defaultTexture;
}

void Assets::SetFlipTexture(bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
}

bool Assets::ContainsTexture(const std::string &name)
{  
    return textures.find(name) != textures.end();
}

bool Assets::ContainsShader(const std::string &name)
{
    return shaders.find(name) != shaders.end();
}

Shader *Assets::LoadShader(const std::string &name, const std::string &vertexShader, const std::string &fragmentShader)
{
    if (shaders.find(name) != shaders.end())
        return shaders[name];
    Shader *shader = new Shader();
    if (shader->Load(vertexShader.c_str(), fragmentShader.c_str()))
    {
        shaders[name] = shader;
    }
    return shader;
}

Shader *Assets::CreateShader(const std::string &name, const std::string &vertexShader, const std::string &fragmentShader)
{
    if (shaders.find(name) != shaders.end())
        return shaders[name];
    Shader *shader = new Shader();
    if (shader->Create(vertexShader.c_str(), fragmentShader.c_str()))
    {
        shaders[name] = shader;
    }
    return shader;
}

Shader *Assets::GetShader(const std::string &name)
{
    if (shaders.find(name) == shaders.end())
    {
        return nullptr;
    }
    return shaders[name];
}

Texture2D *Assets::LoadTexture( const std::string &file_name)
{

    const char* name = Utils::GetFileNameWithoutExt(file_name.c_str());
    if (textures.find(name) != textures.end())
        return textures[name];
    Texture2D *texture = new Texture2D();
    if (texture->Load(file_name.c_str()))
    {
       // Utils::LogInfo("TEXTURE2D: [ID %s] Load Texture2D (%s)",name,  file_name.c_str());
        textures[name] = texture;
        return texture;
    }
    return defaultTexture;
}

Texture2D *Assets::GetTexture(const std::string &name)
{
    if (textures.find(name) == textures.end())
    {
        return defaultTexture;
    }
    return textures[name];
}

void Assets::Init()
{
    if (!instance)
        instance = new Assets();

{

     const char *vShader = GLSL(
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texCoord;
            layout(location = 2) in vec4 color;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            out vec2 TexCoord;
            out vec4 vertexColor;
            void main() {
                gl_Position = projection * view * model * vec4(position, 1.0);
                TexCoord = texCoord;
                vertexColor = color;
            });

        const char *fShader = GLSL(
            in vec2 TexCoord;
            out vec4 color;
            in vec4 vertexColor;
            uniform sampler2D texture0;
            void main() {
                color = texture(texture0, TexCoord) * vertexColor;
            });

        Shader *shader = new Shader();
        Utils::LogInfo("Load Default Shader");
        if (shader->Create(vShader, fShader))
        {
            instance->shaders["default"] = shader;
            shader->LoadDefaults();
        } else 
        {
            delete shader;
        }
}
{

     const char *vShader = GLSL(
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texCoord;

            out vec2 fragTexCoord;

            void main() 
            {
            fragTexCoord = texCoord;
            gl_Position = vec4(position, 1.0);
            });

        const char *fShader = GLSL(
            in vec2 fragTexCoord;
            out vec4 fragColor;

            uniform sampler2D renderTexture;
            uniform int Depth = 0;

            void main() 
            {
                if (Depth == 1)
                {
                    float r = texture(renderTexture, fragTexCoord).r; // Profundidade
                    fragColor = vec4(vec3(r), 1.0); // Tons de cinza
                } else 
                {
                    fragColor = texture(renderTexture, fragTexCoord);
                }
            });

        Shader *shader = new Shader();
        Utils::LogInfo("Load Screen Shader");
        if (shader->Create(vShader, fShader))
        {
            instance->shaders["screen"] = shader;
            shader->LoadDefaults();
        } else 
        {
            delete shader;
        }
}
}

void Assets::Release()
{

    for (auto texture : textures)
    {
        Texture2D *tex = texture.second;
        tex->Release();
        delete tex;
    }
    textures.clear();


    for (auto shader : shaders)
    {
        Shader *s = shader.second;
        s->Release();
        delete s;
    }
    shaders.clear();

    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

RenderTexture::RenderTexture()
{
        framebuffer = 0;
        depthBuffer = 0;
        depthTexture = 0;
}

bool RenderTexture::Init(int width, int height)
{
    if (framebuffer != 0)
        return false;
    this->width = width;
    this->height = height;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    return true;
}

void RenderTexture::addColorBuffer()
{
    DEBUG_BREAK_IF(framebuffer == 0);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLuint colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,  GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorBuffers.size(), GL_TEXTURE_2D, colorBuffer, 0);
    colorBuffers.push_back(colorBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderTexture::addDepthBuffer()
{

        DEBUG_BREAK_IF(framebuffer == 0);
        if (depthBuffer != 0)
            return;
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::addDepthTexture()
{
        if (depthTexture != 0)
            return;
        DEBUG_BREAK_IF(framebuffer == 0);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool RenderTexture::Process()
{
     glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
     {
        Utils::LogError("Framebuffer is not complete!");
        return false;
     }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void RenderTexture::Begin()
{
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTexture::End()
{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Driver::Instance().GetWidth(), Driver::Instance().GetHeight());
}


void RenderTexture::Release()
{
        for (GLuint colorBuffer : colorBuffers) 
        {
            glDeleteTextures(1, &colorBuffer);
        }
        colorBuffers.clear();
        if (depthBuffer) 
        {
            glDeleteRenderbuffers(1, &depthBuffer);
        }
        if (depthTexture) 
        {
            glDeleteTextures(1, &depthTexture);
        }
        if (framebuffer)
        {
            glDeleteFramebuffers(1, &framebuffer);
        }
        framebuffer = 0;
        depthBuffer = 0;
        depthTexture = 0;
}

u32 RenderTexture::GetColorBuffer(int index)
{
    DEBUG_BREAK_IF(index < 0 || index >= (int)colorBuffers.size());
    return colorBuffers[index];
}

ScreenQuad::ScreenQuad()
{
    vao = 0;
    vbo = 0;
    
}

void ScreenQuad::Init()
{
    if (vao != 0 || vbo != 0) return ;

        float vertices[24] = 
        {
        -1.0f, -1.0f, 0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f, 1.0f, 0.0f,  // Bottom-right
        -1.0f,  1.0f, 0.0f, 1.0f,  // Top-left
         1.0f,  1.0f, 1.0f, 1.0f   // Top-right
    }   ;
        
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    

}

void ScreenQuad::Release()
{
    if (vao != 0)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
}

void ScreenQuad::Render(float x, float y, float width, float height)
{
        if (vao == 0 || vbo == 0) return ;
        float left    = x;
        float right   = x + width;
        float bottom  = y;
        float top     = y + height;

        float updatedVertices[24] = 
        {
            left,  bottom, 0.0f, 0.0f,  // Bottom-left
            right, bottom, 1.0f, 0.0f,  // Bottom-right
            left,  top,    0.0f, 1.0f,  // Top-left
            right, top,    1.0f, 1.0f   // Top-right
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedVertices), updatedVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
}
