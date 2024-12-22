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

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_CORE_DLL
#ifdef __GNUC__
#define CORE_PUBLIC __attribute__((dllexport))
#else
#define CORE_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define CORE_PUBLIC __attribute__((dllimport))
#else
#define CORE_PUBLIC __declspec(dllimport)
#endif
#endif
#define CORE_LOCAL
#else
#if __GNUC__ >= 4
#define CORE_PUBLIC __attribute__((visibility("default")))
#define CORE_LOCAL __attribute__((visibility("hidden")))
#else
#define CORE_PUBLIC
#define CORE_LOCAL
#endif
#endif



#include <stddef.h>
#include <cstddef> 
#include <cstdio>
#include <cstdlib> 
#include <stdarg.h>
#include <cstring>
#include <math.h>
#include <cassert>
#include <time.h>
#include <climits>
#include <utility>
#include <SDL2/SDL.h>

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

#define GLSL(src) "#version 400 core\n" #src




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

inline size_t CalculateCapacityGrow(size_t capacity, size_t minCapacity)
{
    if (capacity < minCapacity)
        capacity = minCapacity;
    if (capacity < 8)
    {
        capacity = 8;
    }
    else
    {
        // Round up to the next power of 2 and multiply by 2 (http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2)
        capacity--;
        capacity |= capacity >> 1;
        capacity |= capacity >> 2;
        capacity |= capacity >> 4;
        capacity |= capacity >> 8;
        capacity |= capacity >> 16;
        capacity++;
    }
    return capacity;
}

template <typename T>
class Allocator
{

public:
    T *allocate(size_t n)
    {
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    void deallocate(T *p, size_t n)
    {
        (void)n;
        ::operator delete(p);
    }

    void deallocate(T *ptr)
    {
        operator delete(ptr);
    }

    template <typename... Args>
    void construct(T *p, Args &&...args)
    {

        new (p) T(std::forward<Args>(args)...);
    }

    void construct(T *ptr, const T &value)
    {
        new (ptr) T(value);
    }
    void destroy(T *p)
    {

        p->~T();
    }
};

class  Ref
{
public:
    Ref()
        : DebugName(0), ReferenceCounter(1)
    {
    }

    virtual ~Ref()
    {
        //  std::cout <<DebugName<<" destroyed" << std::endl;
    }

    void Grab() const { ++ReferenceCounter; }

    bool Drop() const
    {

        --ReferenceCounter;
        if (!ReferenceCounter)
        {
            delete this;
            return true;
        }

        return false;
    }

    s32 getReferenceCount() const
    {
        return ReferenceCounter;
    }

    const c8 *getDebugName() const
    {
        return DebugName;
    }

protected:
    void setDebugName(const c8 *newName)
    {
        DebugName = newName;
    }

private:
    const c8 *DebugName;
    mutable s32 ReferenceCounter;
};