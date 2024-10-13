#pragma once

#include "Config.hpp"
#include "String.hpp"
#include "Vector.hpp"



class   Logger : public Ref
{
    public:

        void Error( const char *msg, ... );
        void Warning( const char *msg, ... );
        void Info( const char *msg, ... );
        static Logger &Instance();
    private:
         Logger() ;
        virtual ~Logger() ;

        static Logger *m_instance;
};

class    System : public Ref
{
    public:

     void Sleep(unsigned int ms);
     void DebugBreak();
     void Abort();
     void Exit(int code);
     void SetEnvironment(const char* name, const char* value);
     const char* GetEnvironment(const char* name);
   
     void* LoadObjectLibrary(const char* path);
     void  FreeObjectLibrary(void* handle);
     void* GetProcAddress(void* handle, const char* name);
 
    bool FileExists(const char *fileName);
    bool DirectoryExists(const char *dirPath);              
    bool IsFileExtension(const char *fileName, const char *ext); // Check file extension (including point: .png, .wav)
    int GetFileLength(const char *fileName);                    
    
    const char *GetFileExtension(const char *fileName);         // Get pointer to extension for a filename string (includes dot: '.png')
    const char *GetFileName(const char *filePath);              // Get pointer to filename for a path string
    const char *GetFileNameWithoutExt(const char *filePath);    // Get filename string without extension (uses static string)
    const char *GetDirectoryPath(const char *filePath);         // Get full path for a given fileName with path (uses static string)
    const char *GetPrevDirectoryPath(const char *dirPath);      // Get previous directory path for a given path (uses static string)
    const char *GetWorkingDirectory(void);                      // Get current working directory (uses static string)
    const char *GetApplicationDirectory(void);                  // Get the directory if the running application (uses static string)


    bool ChangeDirectory(const char *dir);                      // Change working directory, return true on success
    bool IsPathFile(const char *path);                          // Check if a given path is a file or a directory

    
    unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead);      
    bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite);  
    char *LoadFileText(const char *fileName);                   
    bool SaveFileText(const char *fileName, char *text);        

    bool LoadDirectoryFiles(const char *dirPath,  Vector<String> &files);
    bool LoadDirectoryFiles(const char *dirPath, const char *filter, Vector<String> &files);
    bool LoadDirectoryFiles(const char *dirPath, const char *filter, bool scanSubdirs,  Vector<String> &files);    

    u64 GetFileModTime(const char *fileName);       

    static System &Instance(); 

private:
    bool ScanDirectoryFilesRecursively(const char *basePath,   Vector<String> &files, const char *filter);
    bool ScanDirectoryFiles(const char *basePath,   Vector<String> &files, const char *filter);
    static System *m_instance;

        System();
        virtual ~System();
};

class    Mutex : public Ref
{
    public:
    Mutex();
    virtual ~Mutex();

    void Lock();
    void Unlock();
    bool TryLock();
    private:
         SDL_mutex *m_mutex;
             
};

class    Semaphore: public Ref
{
    public:
    Semaphore(unsigned int count = 0);
    virtual ~Semaphore();

    void Wait();
    bool TryWait();
    void TimeWait(unsigned int ms);
    
    void Post();
    
    unsigned int GetValue();

    private:
        SDL_sem *m_semaphore;
      
};
 

namespace priv
{
    struct ThreadFunc;
}

class    Thread : public Ref
{
    public:
    virtual ~Thread();

    template <typename F>
    Thread(const char *name,F function);


    template <typename F, typename A>
    Thread(const char *name,F function, A argument);

    template <typename F, typename... Args>
    Thread(const char *name,F function, Args... args);



    template <typename C>
    Thread(const char *name,void(C::*function)(), C* object);




    void SetPriority(int priority);


    
    void launch(bool detach=false);
    void terminate();

    int GetExitCode() const { return m_exitCode; }

    const char* GetName() const ;
    unsigned long GetID() ;






private:
        SDL_Thread *m_thread;
        priv::ThreadFunc* m_entryPoint; ///< Abstraction of the function to run
        bool  m_isActive; 
        int m_exitCode;
        
  
        void run();
        void wait();
        void detach();


        static int entryPoint(void* userData);
          


};