
#include "OS.hpp"
#include "Utils.hpp"
#include "Device.hpp"
#include "String.hpp"
#include "Chars.hpp"



#if defined(PLATFORM_DESKTOP) && defined(_WIN32) && (defined(_MSC_VER) || defined(__TINYC__))

    #include "wdirent.h"    // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#else
    #include <dirent.h>             // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#endif

#if defined(_WIN32)
    #include <direct.h>             // Required for: _getch(), _chdir()
    #define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
    #define CHDIR _chdir
    #include <io.h>                 // Required for: _access() [Used in FileExists()]
#else
    #include <unistd.h>             // Required for: getch(), chdir() (POSIX), access()
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



static void LogMessage( int level, const char *msg, va_list args )
{
	    time_t rawTime;
		struct tm* timeInfo;
		char timeBuffer[80];



		time(&rawTime);
		timeInfo = localtime(&rawTime);

		strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

        char buffer[1024];
        vsnprintf( buffer, sizeof(buffer), msg, args );

    switch (level)
    {
        case 0:
            SDL_LogInfo(0, "%s%s: %s%s", CONSOLE_COLOR_GREEN,timeBuffer, buffer,CONSOLE_COLOR_RESET);
            break;
        case 1:
            SDL_LogError(0,  "%s%s: %s%s", CONSOLE_COLOR_RED,timeBuffer, buffer,CONSOLE_COLOR_RESET);
            break;
        case 2:
            SDL_LogWarn(0,  "%s%s: %s%s", CONSOLE_COLOR_PURPLE,timeBuffer, buffer,CONSOLE_COLOR_RESET);
            break;
 
    }

}


void LogError( const char *msg, ... )
{
	

	va_list args;
	va_start( args, msg );
	LogMessage( 1, msg, args );
	va_end( args );
}


void LogWarning( const char *msg, ... )
{
	
	va_list args;
	va_start( args, msg );
	LogMessage( 2, msg, args );
	va_end( args );
}


void LogInfo( const char *msg, ... )
{
	va_list args;
	va_start( args, msg );
	LogMessage( 0, msg, args );
	va_end( args );
}


Logger* Logger::m_instance =0x0;

Logger& Logger::Instance()
{
    if (m_instance == 0x0)
    {
        LogInfo("[LOGGER] Created.");
        m_instance = new Logger();
    }
    return *m_instance;
}

Logger::Logger():Ref()
{
   setDebugName("Logger");
}

Logger::~Logger()
{
    
        LogInfo("[LOGGER] Destroyed");
        m_instance = 0x0;
        
}


void Logger::Error( const char *msg, ... )
{
	va_list args;
	va_start( args, msg );
	LogMessage( 1, msg, args );
	va_end( args );
}


void Logger::Warning( const char *msg, ... )
{
  
	va_list args;
	va_start( args, msg );
	LogMessage( 2, msg, args );
	va_end( args );
}


void Logger::Info( const char *msg, ... )
{
   va_list args;
	va_start( args, msg );
	LogMessage( 0, msg, args );
	va_end( args );
}






namespace priv
{


struct ThreadFunc
{
    virtual ~ThreadFunc() {}
    virtual int run() = 0;
};

// Specialization using a functor (including free functions) with no argument
template <typename T>
struct ThreadFunctor : ThreadFunc
{
    ThreadFunctor(T functor) : m_functor(functor) {}
    virtual int run() 
    {
        return m_functor();
    }
    T m_functor;
};

// Specialization using a functor (including free functions) with one argument
template <typename F, typename A>
struct ThreadFunctorWithArg : ThreadFunc
{
    ThreadFunctorWithArg(F function, A arg) : m_function(function), m_arg(arg) {}
    virtual int run() 
    {
        return  (m_arg->*m_function)();
    }
    F m_function;
    A m_arg;
};


template <typename C>
struct ThreadMemberFunc : ThreadFunc
{
    ThreadMemberFunc(void(C::*function)(), C* object) : m_function(function), m_object(object) {}
    virtual int run()
     {
        return (m_object->*m_function)();
    }
    void(C::*m_function)();
    C* m_object;
};

} 


template <typename F>
Thread::Thread(const char *name,F functor) :Ref(),
m_entryPoint(new priv::ThreadFunctor<F>(functor))
{
    m_thread =  SDL_CreateThread(&Thread::entryPoint, name, this);
    if (!m_thread)
    {
       SDL_LogError(0, "Failed to create thread!");
        m_isActive = false;
    }
    m_isActive = true;
    m_exitCode = 0;
}


template <typename F, typename A>
Thread::Thread(const char *name,F function, A argument) :Ref(),
m_entryPoint(new priv::ThreadFunctorWithArg<F, A>(function, argument))
{
    m_thread =  SDL_CreateThread(&Thread::entryPoint, name, this);
    if (!m_thread)
    {
        SDL_LogError(0, "Failed to create thread!");
        m_isActive = false;
    }
    m_isActive = true;
    m_exitCode = 0;
}

template <typename F, typename... Args>
Thread::Thread(const char *name,F function, Args... args):Ref()
{
   // Create a lambda to capture the function and its arguments
    auto lambda = [function, args...]() -> int
    {
        return function(args...);
    };

    m_entryPoint = new priv::ThreadFunctor<decltype(lambda)>(lambda);
    m_thread =  SDL_CreateThread(&Thread::entryPoint, name, this);
    if (!m_thread)
    {
        SDL_LogError(0, "Failed to create thread!");
        m_isActive = false;
    }
    m_isActive = true;
    m_exitCode = 0;
}

template <typename C>
Thread::Thread(const char *name,void(C::*function)(), C* object) :Ref(),
m_entryPoint(new priv::ThreadMemberFunc<C>(function, object))
{
    m_thread =  SDL_CreateThread(&Thread::entryPoint, name, this);
    if (!m_thread)
    {
        SDL_LogError(0, "Failed to create thread!");
        m_isActive = false;
    }
    m_isActive = true;
    m_exitCode = 0;
}




Thread::~Thread()
{
    wait();
    m_thread = nullptr;
    delete m_entryPoint;
    m_entryPoint = nullptr;
  
}

 void Thread::SetPriority(int priority)
{
    SDL_SetThreadPriority((SDL_ThreadPriority)priority);
}

void Thread::wait()
{
    if (m_thread != nullptr)
    {
        SDL_WaitThread(m_thread, &m_exitCode);
        m_thread = nullptr;
    }
}

void Thread::detach()
{
    if (m_thread != nullptr)
    {
        SDL_DetachThread(m_thread);
        m_thread = nullptr;
    }
}

void Thread::launch(bool detach)
{
    if (m_thread != nullptr)
    {
        if (detach)
            this->detach();
        else
            this->wait();
        m_thread = nullptr;
    }
  
}


int Thread::entryPoint(void *userData)
{
    Thread* owner = static_cast<Thread*>(userData);
    owner->run();
    return owner->m_exitCode;
   
}



 const char *Thread::GetName() const
{
    if (m_thread == nullptr)
        return nullptr;
    return SDL_GetThreadName(m_thread);
}

inline unsigned long Thread::GetID()
{
    if (m_thread == nullptr)
        return 0;
    return SDL_GetThreadID(m_thread);
}

void Thread::run()
{
    m_exitCode = m_entryPoint->run();
}


Semaphore::Semaphore(unsigned int count):Ref()
{
    m_semaphore = SDL_CreateSemaphore(count);
}

Semaphore::~Semaphore()
{
    SDL_DestroySemaphore(m_semaphore);
}

void Semaphore::Wait()
{
    if (!m_semaphore)
        return;
    SDL_SemWait(m_semaphore);
}

 bool Semaphore::TryWait()
{
    if (!m_semaphore)
        return false;
    return SDL_SemTryWait(m_semaphore) == 0;
}

void Semaphore::TimeWait(unsigned int ms)
{
    if (!m_semaphore)
        return;
    SDL_SemWaitTimeout(m_semaphore, ms);
}

void Semaphore::Post()
{
    if (!m_semaphore)
        return;
    SDL_SemPost(m_semaphore);
}

unsigned int Semaphore::GetValue()
{
    if (!m_semaphore)
        return 0;
    return SDL_SemValue(m_semaphore);
}


System* System::m_instance = 0x0;

System::System():Ref()
{
    setDebugName("System");

}


System& System::Instance()
{
    if (m_instance == 0x0)
    {
        LogInfo("[SYSTEM] Created.");
        m_instance = new System();
    }
    return *m_instance;
}

System::~System()
{
    
        LogInfo("[SYSTEM] Destroyed.");
      
        m_instance = 0x0;
      
}

void System::Sleep(unsigned int ms)
{
    SDL_Delay(ms);
}

void System::DebugBreak()
{
    SDL_TriggerBreakpoint();
}

void System::Abort()
{
    SDL_Quit();
    exit(1);
}

void System::Exit(int code)
{
    SDL_Quit();
    exit(code);
}

void System::SetEnvironment(const char *name, const char *value)
{
    #if defined(CORE_LINUX)
        setenv(name, value, 1);
    #else
         _putenv((char*)name);
    #endif
}

const char *System::GetEnvironment(const char *name)
{
    return getenv(name);
}

void *System::LoadObjectLibrary(const char *path)
{
    return SDL_LoadObject(path);
}

void System::FreeObjectLibrary(void *handle)
{
    SDL_UnloadObject(handle);
}

void *System::GetProcAddress(void *handle, const char *name)
{
    return SDL_LoadFunction(handle, name);
}

bool System::FileExists(const char *fileName)
{
   bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1) result = true;
#else
    if (access(fileName, F_OK) != -1) result = true;
#endif
    return result;
}

bool System::DirectoryExists(const char *dirPath)
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

bool System::IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;

    const char *fileExt = std::strrchr(fileName, '.');

    if (fileExt != NULL)
    {
        if (strcmp(fileExt, ext) == 0) result = true;
    }

    return result;
}

int System::GetFileLength(const char *fileName)
{
    int length = 0;

    SDL_RWops *rw = SDL_RWFromFile(fileName, "rb");
    if (rw != NULL)
    {
        length = static_cast<int>(SDL_RWsize(rw));
        SDL_RWclose(rw);
    }


    return length;
}

const char *System::GetFileExtension(const char *fileName)
{
    const char *fileExt = std::strrchr(fileName, '.');

    if (fileExt != NULL) return fileExt;

    return NULL;
}

const char *System::GetFileName(const char *filePath)
{
    const char *fileName = std::strrchr(filePath, '/');

    if (fileName != NULL) return fileName + 1;

    return filePath;
}

const char *System::GetFileNameWithoutExt(const char *filePath)
{
    static char baseName[256];
    std::strcpy(baseName, GetFileName(filePath));

    char *dot = std::strrchr(baseName, '.');
    if (dot) *dot = '\0';

    return baseName;
}

const char *System::GetDirectoryPath(const char *filePath)
{
    static char dirPath[256];
    std::strcpy(dirPath, filePath);

    char *lastSlash = std::strrchr(dirPath, '/');

    if (lastSlash != NULL) lastSlash[1] = '\0';
    else dirPath[0] = '\0';

    return dirPath;
}

const char *System::GetPrevDirectoryPath(const char *dirPath)
{
    static char prevDirPath[256];
    std::strcpy(prevDirPath, dirPath);

    char *lastSlash = std::strrchr(prevDirPath, '/');

    if (lastSlash != NULL)
    {
        lastSlash[0] = '\0';
        lastSlash = std::strrchr(prevDirPath, '/');
        if (lastSlash != NULL) lastSlash[1] = '\0';
        else prevDirPath[0] = '\0';
    }
    else prevDirPath[0] = '\0';

    return prevDirPath;
}

const char *System::GetWorkingDirectory()
{
    static char currentDir[256];
    GETCWD(currentDir, 256);
    return currentDir;
}

const char *System::GetApplicationDirectory()
{
    static char appDir[256];
    GETCWD(appDir, 256);
    return appDir;
}

bool System::ChangeDirectory(const char *dir)
{
    return CHDIR(dir) == 0;
}

bool System::IsPathFile(const char *path)
{
    bool result = false;

    DIR *dir = opendir(path);
    if (dir == NULL) result = true;

    return result;
}

unsigned char *System::LoadFileData(const char *fileName, unsigned int *bytesRead)
{
    return LoadDataFile(fileName, bytesRead);
}

bool System::SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite)
{
    return SaveDataFile(fileName, data, bytesToWrite);
}

char *System::LoadFileText(const char *fileName)
{
    return LoadTextFile(fileName);
}

bool System::SaveFileText(const char *fileName, char *text)
{
    return SaveTextFile(fileName, text);
}

bool System::LoadDirectoryFiles(const char *dirPath,  Vector<String> &files)
{
    
     DIR *dir = opendir(dirPath);

    if (dir != NULL) // It's a directory
    {
        closedir(dir);
        ScanDirectoryFiles(dirPath, files, NULL);     
        return true;  
    }
    LogError("FILEIO: Failed to open requested directory");  // Maybe it's a file...
    return false;

   
}

bool System::LoadDirectoryFiles(const char *dirPath, const char *filter, Vector<String> &files)
{
     DIR *dir = opendir(dirPath);

    if (dir != NULL) // It's a directory
    {
        closedir(dir);
        ScanDirectoryFiles(dirPath, files, filter);     
        return true;  
    }
    LogError("FILEIO: Failed to open requested directory");  // Maybe it's a file...
    return false;
}

bool System::LoadDirectoryFiles(const char *basePath, const char *filter, bool scanSubdirs,  Vector<String> &files)
{

    if (scanSubdirs) 
        return ScanDirectoryFilesRecursively(basePath, files, filter);
    else 
        return ScanDirectoryFiles(basePath,files, filter);
    
}

u64 System::GetFileModTime(const char *fileName)
{
    struct stat fileStat;
    if (stat(fileName, &fileStat) == 0) return fileStat.st_mtime;
    return 0;
}

bool System::ScanDirectoryFiles(const char *basePath,   Vector<String> &files, const char *filter)
{
    static char path[512] = { 0 };
    memset(path, 0, 512);

    struct dirent *dp = NULL;
    DIR *dir = opendir(basePath);

    if (dir != NULL)
    {
        while ((dp = readdir(dir)) != NULL)
        {
            if ((strcmp(dp->d_name, ".") != 0) &&
                (strcmp(dp->d_name, "..") != 0))
            {
                sprintf(path, "%s/%s", basePath, dp->d_name);

                if (filter != NULL)
                {
                    if (IsFileExtension(path, filter))
                    {
                        files.push_back(path);
                    }
                }
                else
                {
                    files.push_back(path);
                }
            }
        }

        closedir(dir);
        return true;
    }
    
    LogError( "FILEIO: Directory cannot be opened (%s)", basePath);
    return false;
}

// Scan all files and directories recursively from a base path
 bool System::ScanDirectoryFilesRecursively(const char *basePath,   Vector<String> &files, const char *filter)
{
    char path[512] = { 0 };
    memset(path, 0, 512);

    struct dirent *dp = NULL;
    DIR *dir = opendir(basePath);

    if (dir != NULL)
    {
        while (((dp = readdir(dir)) != NULL) )
        {
            if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
            {
                // Construct new path from our base path
                sprintf(path, "%s/%s", basePath, dp->d_name);

                if (IsPathFile(path))
                {
                    if (filter != NULL)
                    {
                        if (IsFileExtension(path, filter))
                        {
                            files.push_back(path);
                        }
                    }
                    else
                    {
                         files.push_back(path);
                       }

                    }
                else ScanDirectoryFilesRecursively(path, files, filter);
            }
        }

        closedir(dir);
        return true;
    }
    LogError("FILEIO: Directory cannot be opened (%s)", basePath);
    return false;
}