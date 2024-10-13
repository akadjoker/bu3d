

#pragma once

#include <cstring>
#include "Config.hpp"
#include "Vector.hpp"

const u32 NPOS = 0xffffffff;

class String
{
public:
    String() : m_length(0), m_capacity(1)
    {
        m_buffer = new char[1];
        m_buffer[0] = '\0';
    }

    String(const String &str) : m_length(str.m_length), m_capacity(str.m_capacity)
    {

        m_buffer = new char[m_capacity];
        memcpy(m_buffer, str.m_buffer, m_length + 1);
    }

    String(const char *str) : m_length(0), m_capacity(0), m_buffer(nullptr)
    {
        if (str)
        {
            m_length = (u32)strlen(str);
            m_capacity = m_length + 1;
            m_buffer = new char[m_capacity];
            memcpy(m_buffer, str, m_length + 1);
        }
        else
        {
            m_length = 0;
            m_capacity = 1;
            m_buffer = new char[1];
            m_buffer[0] = '\0';
        }
    }

    String(const char *str, u32 length) : m_length(length), m_capacity(length + 1)
    {
        m_buffer = new char[m_capacity];
        memcpy(m_buffer, str, length);
        m_buffer[length] = '\0';
    }

    explicit String(int number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%d", number);
        *this = tempBuffer;
    }

    explicit String(u32 number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%u", number);
        *this = tempBuffer;
    }

    explicit String(long number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%ld", number);
        *this = tempBuffer;
    }

    explicit String(unsigned long number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%lu", number);
        *this = tempBuffer;
    }

    explicit String(float number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%f", number);
        *this = tempBuffer;
    }

    explicit String(double number) : String()
    {
        char tempBuffer[128];
        sprintf(tempBuffer, "%f", number);
        *this = tempBuffer;
    }

    String(char c) : m_length(1), m_capacity(2)
    {
        m_buffer = new char[m_capacity];
        m_buffer[0] = c;
        m_buffer[1] = '\0';
    }

    ~String()
    {
        delete[] m_buffer;
    }
    void say();

    static String toString(int number)
    {
        String str(number);
        return str;
    }

    void compact()
    {
        if (m_capacity)
            reserve(m_length + 1);
    }

    String &operator=(const String &str)
    {
        if (this == &str)
            return *this;

        delete[] m_buffer;

        m_length = str.m_length;
        m_capacity = str.m_capacity;
        m_buffer = new char[m_capacity];
        memcpy(m_buffer, str.m_buffer, m_length + 1);

        return *this;
    }

    String &operator=(String &&str) noexcept
    {
        if (this == &str)
            return *this;

        delete[] m_buffer;

        m_length = str.m_length;
        m_capacity = str.m_capacity;
        m_buffer = str.m_buffer;

        str.m_length = 0;
        str.m_capacity = 1;
        str.m_buffer = new char[1];
        str.m_buffer[0] = '\0';

        return *this;
    }

    friend String operator+(const String &lhs, const String &rhs);
    friend String operator+(const char *lhs, const String &rhs);
    friend String operator+(const String &lhs, const char *rhs);

    String &operator+=(const String &rhs)
    {
        u32 oldm_Length = m_length;
        resize(m_length + rhs.m_length);
        memcpy(&m_buffer[oldm_Length], rhs.m_buffer, rhs.m_length);
        return *this;
    }

    String &operator+=(const char *rhs)
    {
        u32 rhsm_Length = (u32)strlen(rhs);
        u32 oldm_Length = m_length;
        resize(m_length + rhsm_Length);
        memcpy(&m_buffer[oldm_Length], rhs, rhsm_Length);
        return *this;
    }

    String &operator+=(const char rhs)
    {
        u32 oldm_Length = m_length;
        resize(m_length + 1);
        m_buffer[oldm_Length] = rhs;
        m_buffer[m_length] = '\0';
        return *this;
    }

    String &append(const String &str) { return *this += str; }
    String &append(const char *str) { return *this += str; }
    String &append(char c) { return *this += c; }
    String &append(const char *str, u32 length);

    float ToFloat() const;
    double ToDouble() const;
    int ToInt() const;
    const char *c_str() const { return m_buffer; }
    u32 length() const { return m_length; }
    u32 size() const { return m_length; }
    u32 capacity() const { return m_capacity; }
    bool empty() const { return m_length == 0; }

    char &operator[](u32 index);
    const char &operator[](u32 index) const;
    bool operator==(const String &rhs) const;
    bool operator!=(const String &rhs) const;

    void ToLower();
    void ToUpper();

    String Lower() const;
    String Upper() const;
    String Left(u32 count) const;
    String Right(u32 count) const;
    String Mid(u32 pos, u32 count) const;
    String Section(u32 Pos1, u32 Pos2) const;
    u64 ToHash() const;

    void replace(char replaceThis, char replaceWith, bool caseSensitive);
    void replace(const String &replaceThis, const String &replaceWith, bool caseSensitive);
    void replace(u32 pos, u32 length, const String &replaceWith);
    void replace(u32 pos, u32 length, const char *replaceWith);
    void replace(u32 pos, u32 length, const char *srcStart, u32 srcLength);

    u32 find(char c, u32 startPos, bool caseSensitive) const;
    u32 find(const String &str, u32 startPos, bool caseSensitive) const;
    
    String replace(char replaceThis, char replaceWith, bool caseSensitive) const;
    String replace(const String &replaceThis, const String &replaceWith, bool caseSensitive = true) const;

    String substr(u32 pos) const;
    String substr(u32 pos, u32 length) const;
    String trim() const;
    String ltrim() const;
    String rtrim() const;

    void join(const Vector<String> &subStrings, const String &glue);
    
    Vector<String> split(char separator, bool keepEmptyStrings = false);
    Vector<String> split(const char *str, char separator, bool keepEmptyStrings = false);
    
    String joined(const Vector<String> &subStrings, const String &glue);
    
    u32 find_last_of(char c, u32 startPos, bool caseSensitive = true) const;
    u32 find_last_of(const String &str, u32 startPos = NPOS, bool caseSensitive = true) const;
    
    bool starts_with(const String &str, bool caseSensitive = true) const;
    bool ends_with(const String &str, bool caseSensitive = true) const;

private:
    u32 m_length;
    u32 m_capacity;
    char *m_buffer;

    void Move(u32 dest, u32 src, u32 count);
    void Copy(const char *src, u32 count);
    void reserve(u32 newCapacity);
    void resize(u32 newLength);
    static void CopyChars(char *dest, const char *src, unsigned count)
    {
        if (count)
            memcpy(dest, src, count);
    }
};
