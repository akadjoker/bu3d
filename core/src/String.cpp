#include "String.hpp"



inline void String::say()
{
    printf("%s", c_str());
}

inline String operator+(const String &lhs, const String &rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

inline String operator+(const char *lhs, const String &rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

inline String operator+(const String &lhs, const char *rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}
inline char tolower(char c)
{
    if (c >= 'A' && c <= 'Z')
        c = c - 'A' + 'a';
    return c;
}

inline char toupper(char c)
{
    if (c >= 'a' && c <= 'z')
        c = c - 'a' + 'A';
    return c;
}


void String::Move(u32 dest, u32 src, u32 count)
{
    if (count)
        memmove(m_buffer + dest, m_buffer + src, count);
}

void String::Copy(const char *src, u32 count)
{
    if (count)
        memcpy(m_buffer, src, count);
}

void String::reserve(u32 newCapacity)
{
    if (newCapacity < m_length + 1)
        newCapacity = m_length + 1;
    if (newCapacity == m_capacity)
        return;

    char *newBuffer = new char[newCapacity];
    if (m_length)
        memcpy(newBuffer, m_buffer, m_length + 1);

    delete[] m_buffer;
    m_buffer = newBuffer;
    m_capacity = newCapacity;
}
void String::resize(u32 newLength)
{
    if (!m_capacity)
    {
        if (!newLength)
            return;

        // Calculate initial capacity
        m_capacity = newLength + 1;
        if (m_capacity < 8)
            m_capacity = 8;

        m_buffer = new char[m_capacity];
    }
    else
    {
        if (newLength && m_capacity < newLength + 1)
        {
            // Increase the capacity with half each time it is exceeded
            while (m_capacity < newLength + 1)
                m_capacity += (m_capacity + 1) >> 1;

            char *newBuffer = new char[m_capacity];
            if (m_length)
                memcpy(newBuffer, m_buffer, m_length);
            delete[] m_buffer;
            m_buffer = newBuffer;
        }
    }

    m_buffer[newLength] = '\0';
    m_length = newLength;
}

//************************************** 
   float String::ToFloat() const
    {
        return (float)atof(m_buffer);
    }
    double String::ToDouble() const
    {
        return atof(m_buffer);
    }
    int String::ToInt() const
    {
        return atoi(m_buffer);
    }


    char &String::operator[](u32 index)
    {
        DEBUG_BREAK_IF(index > m_length);
        return m_buffer[index];
    }
    const char &String::operator[](u32 index) const
    {
        DEBUG_BREAK_IF(index > m_length);
        return m_buffer[index];
    }

    bool String::operator==(const String &rhs) const
    {
        if (m_length != rhs.m_length)
            return false;
        return memcmp(m_buffer, rhs.m_buffer, m_length) == 0;
    }

    bool String::operator!=(const String &rhs) const
    {
        if (m_length != rhs.m_length)
            return true;
        return memcmp(m_buffer, rhs.m_buffer, m_length) != 0;
    }

    void String::ToLower()
    {
        for (u32 i = 0; i < m_length; i++)
            m_buffer[i] = tolower(m_buffer[i]);
    }
    void String::ToUpper()
    {
        for (u32 i = 0; i < m_length; i++)
            m_buffer[i] = toupper(m_buffer[i]);
    }

    String String::Lower() const
    {
        String ret(*this);
        ret.ToLower();
        return ret;
    }
    String String::Upper() const
    {
        String ret(*this);
        ret.ToUpper();
        return ret;
    }

    String String::Left(u32 count) const
    {
        if (count >= m_length)
            return *this;
        return String(m_buffer, count);
    }

    String String::Right(u32 count) const
    {
        if (count >= m_length)
            return *this;
        return String(m_buffer + m_length - count, count);
    }

    String String::Mid(u32 pos, u32 count) const
    {
        if (pos >= m_length)
            return "";
        if (pos + count >= m_length)
            return String(m_buffer + pos);
        return String(m_buffer + pos, count);
    }

    String String::Section(u32 Pos1, u32 Pos2) const
    {
        if (Pos1 >= m_length || Pos2 >= m_length || Pos1 > Pos2)
            return "";

        return this->substr(Pos1, Pos2 - Pos1);
    }

    u64 String::ToHash() const
    {
        u64 hash = 2166136261u;
        const char *ptr = m_buffer;
        while (*ptr)
        {
            hash ^= (u64)(*ptr);
            hash *= 16777619;
            ++ptr;
        }
        return hash;
    }

    void String::replace(char replaceThis, char replaceWith, bool caseSensitive)
    {
        if (caseSensitive)
        {
            for (u32 i = 0; i < m_length; ++i)
            {
                if (m_buffer[i] == replaceThis)
                    m_buffer[i] = replaceWith;
            }
        }
        else
        {
            replaceThis = (char)tolower(replaceThis);
            for (u32 i = 0; i < m_length; ++i)
            {
                if (tolower(m_buffer[i]) == replaceThis)
                    m_buffer[i] = replaceWith;
            }
        }
    }

    void  String::replace(const String &replaceThis, const String &replaceWith, bool caseSensitive)
    {
        u32 nextPos = 0;

        while (nextPos < m_length)
        {
            u32 pos = find(replaceThis, nextPos, caseSensitive);
            if (pos == NPOS)
                break;
            replace(pos, replaceThis.m_length, replaceWith);
            nextPos = pos + replaceWith.m_length;
        }
    }

    void  String::replace(u32 pos, u32 length, const String &replaceWith)
    {
        // If substring is illegal, do nothing
        if (pos + length > m_length)
            return;

        replace(pos, length, replaceWith.m_buffer, replaceWith.m_length);
    }

    void  String::replace(u32 pos, u32 length, const char *replaceWith)
    {
        // If substring is illegal, do nothing
        if (pos + length > m_length)
            return;

        replace(pos, length, replaceWith, (u32)strlen(replaceWith));
    }

    u32  String::find(char c, u32 startPos, bool caseSensitive) const
    {
        if (caseSensitive)
        {
            for (u32 i = startPos; i < m_length; ++i)
            {
                if (m_buffer[i] == c)
                    return i;
            }
        }
        else
        {
            c = (char)tolower(c);
            for (u32 i = startPos; i < m_length; ++i)
            {
                if (tolower(m_buffer[i]) == c)
                    return i;
            }
        }

        return NPOS;
    }

    u32  String::find(const String &str, u32 startPos, bool caseSensitive) const
    {
        if (!str.m_length || str.m_length > m_length)
            return NPOS;

        char first = str.m_buffer[0];
        if (!caseSensitive)
            first = (char)tolower(first);

        for (u32 i = startPos; i <= m_length - str.m_length; ++i)
        {
            char c = m_buffer[i];
            if (!caseSensitive)
                c = (char)tolower(c);

            if (c == first)
            {
                u32 skip = NPOS;
                bool found = true;
                for (u32 j = 1; j < str.m_length; ++j)
                {
                    c = m_buffer[i + j];
                    char d = str.m_buffer[j];
                    if (!caseSensitive)
                    {
                        c = (char)tolower(c);
                        d = (char)tolower(d);
                    }

                    if (skip == NPOS && c == first)
                        skip = i + j - 1;

                    if (c != d)
                    {
                        found = false;
                        if (skip != NPOS)
                            i = skip;
                        break;
                    }
                }
                if (found)
                    return i;
            }
        }

        return NPOS;
    }

    String  String::replace(char replaceThis, char replaceWith, bool caseSensitive) const
    {
        String ret(*this);
        ret.replace(replaceThis, replaceWith, caseSensitive);
        return ret;
    }

    String  String::replace(const String &replaceThis, const String &replaceWith, bool caseSensitive ) const
    {
        String ret(*this);
        ret.replace(replaceThis, replaceWith, caseSensitive);
        return ret;
    }
    void  String::replace(u32 pos, u32 length, const char *srcStart, u32 srcLength)
    {
        int delta = (int)srcLength - (int)length;

        if (pos + length < m_length)
        {
            if (delta < 0)
            {
                Move(pos + srcLength, pos + length, m_length - pos - length);
                resize(m_length + delta);
            }
            if (delta > 0)
            {
                resize(m_length + delta);
                Move(pos + srcLength, pos + length, m_length - pos - length - delta);
            }
        }
        else
            resize(m_length + delta);

        CopyChars(m_buffer + pos, srcStart, srcLength);
    }
    String  String::substr(u32 pos) const
    {
        if (pos < m_length)
        {
            String ret;
            ret.resize(m_length - pos);
            CopyChars(ret.m_buffer, m_buffer + pos, ret.m_length);

            return ret;
        }
        else
            return String();
    }

    String  String::substr(u32 pos, u32 length) const
    {
        if (pos < m_length)
        {
            String ret;
            if (pos + length > m_length)
                length = m_length - pos;
            ret.resize(length);
            CopyChars(ret.m_buffer, m_buffer + pos, ret.m_length);

            return ret;
        }
        else
            return String("");
    }

    String  String::trim() const
    {
        u32 trimStart = 0;
        u32 trimEnd = m_length;

        while (trimStart < trimEnd)
        {
            char c = m_buffer[trimStart];
            if (c != ' ' && c != 9)
                break;
            ++trimStart;
        }
        while (trimEnd > trimStart)
        {
            char c = m_buffer[trimEnd - 1];
            if (c != ' ' && c != 9)
                break;
            --trimEnd;
        }

        return substr(trimStart, trimEnd - trimStart);
    }

    String  String::ltrim() const
    {
        u32 trimStart = 0;
        while (trimStart < m_length)
        {
            char c = m_buffer[trimStart];
            if (c != ' ' && c != 9)
                break;
            ++trimStart;
        }

        return substr(trimStart);
    }

    String  String::rtrim() const
    {
        u32 trimEnd = m_length;
        while (trimEnd > 0)
        {
            char c = m_buffer[trimEnd - 1];
            if (c != ' ' && c != 9)
                break;
            --trimEnd;
        }

        return substr(0, trimEnd);
    }

    void  String::join(const Vector<String> &subStrings, const String &glue)
    {
        *this = joined(subStrings, glue);
    }

    Vector<String>  String::split(char separator, bool keepEmptyStrings )
    {
        return split(c_str(), separator, keepEmptyStrings);
    }

    Vector<String>  String::split(const char *str, char separator, bool keepEmptyStrings )
    {
        Vector<String> ret;
        const char *strEnd = str + strlen(str);

        for (const char *splitEnd = str; splitEnd != strEnd; ++splitEnd)
        {
            if (*splitEnd == separator)
            {
                const u64 splitLen = static_cast<u64>(splitEnd - str);
                if (splitLen > 0 || keepEmptyStrings)
                    ret.push_back(String(str, (u32)splitLen));
                str = splitEnd + 1;
            }
        }

        const ptrdiff_t splitLen = strEnd - str;
        if (splitLen > 0 || keepEmptyStrings)
            ret.push_back(String(str, (u32)splitLen));

        return ret;
    }

    String  String::joined(const Vector<String> &subStrings, const String &glue)
    {
        if (subStrings.empty())
            return String();

        String joinedString(subStrings[0]);
        for (unsigned i = 1; i < subStrings.size(); ++i)
            joinedString.append(glue).append(subStrings[i]);

        return joinedString;
    }

    u32  String::find_last_of(char c, u32 startPos, bool caseSensitive ) const
    {
        if (startPos >= m_length)
            startPos = m_length - 1;

        if (caseSensitive)
        {
            for (u32 i = startPos; i < m_length; --i)
            {
                if (m_buffer[i] == c)
                    return i;
            }
        }
        else
        {
            c = (char)tolower(c);
            for (u32 i = startPos; i < m_length; --i)
            {
                if (tolower(m_buffer[i]) == c)
                    return i;
            }
        }

        return NPOS;
    }

    u32  String::find_last_of(const String &str, u32 startPos , bool caseSensitive ) const
    {
        if (!str.m_length || str.m_length > m_length)
            return NPOS;
        if (startPos > m_length - str.m_length)
            startPos = m_length - str.m_length;

        char first = str.m_buffer[0];
        if (!caseSensitive)
            first = (char)tolower(first);

        for (u32 i = startPos; i < m_length; --i)
        {
            char c = m_buffer[i];
            if (!caseSensitive)
                c = (char)tolower(c);

            if (c == first)
            {
                bool found = true;
                for (u32 j = 1; j < str.m_length; ++j)
                {
                    c = m_buffer[i + j];
                    char d = str.m_buffer[j];
                    if (!caseSensitive)
                    {
                        c = (char)tolower(c);
                        d = (char)tolower(d);
                    }

                    if (c != d)
                    {
                        found = false;
                        break;
                    }
                }
                if (found)
                    return i;
            }
        }

        return NPOS;
    }

    bool  String::starts_with(const String &str, bool caseSensitive ) const
    {
        return find(str, 0, caseSensitive) == 0;
    }

    bool  String::ends_with(const String &str, bool caseSensitive ) const
    {
        u32 pos = find_last_of(str, length() - 1, caseSensitive);
        return pos != NPOS && pos == (u32)length() - str.length();
    }
        String &String::append(const char *str, u32 length)
    {
        if (str)
        {
            u32 oldm_Length = m_length;
            resize(m_length + length);
            memcpy(&m_buffer[oldm_Length], str, length);
            m_buffer[m_length] = '\0';
        }

        return *this;
    }
