
#include "File.hpp"
#include "Utils.hpp"


//********************************************************************************************************************
// FILE/MEMORY STREAM
//********************************************************************************************************************

Stream::Stream()
{
    f_file = nullptr;
    m_open = false;
    m_size = 0;
}

Stream::~Stream()
{
    Close();
}

void Stream::Close()
{
    if (f_file != nullptr)
    {
        SDL_RWclose(f_file);
        f_file = nullptr;
    }
}

u64 Stream::Read(void *buffer, int size)
{
    return SDL_RWread(f_file, buffer, 1, size);
}

u64 Stream::Write(const void *buffer, int size)
{
    return SDL_RWwrite(f_file, buffer, 1, size);
}

u64 Stream::Cursor() const
{
    return SDL_RWtell(f_file);
}   

String Stream::ReadString() 
{
     String value;
    value="";
    char c;
    while ((c = ReadChar()) != 0)
    {
        value += c;
    }
    return value;
}

size_t  Stream::WriteString(const String &value)
{
    size_t  size = 0;
    for (u32 i = 0; i < value.length(); i++)
    {
        size += WriteChar(value[i]);
    }
    size += WriteChar(0);
    return size;
}

size_t  Stream::WriteUTFString(const String &value)
{
    size_t  size = 0;
    size = WriteInt(value.length());
    for (u32 i = 0; i < value.length(); i++)
    {
        size += WriteChar(value[i]);
    }
    return size;
}

 String Stream::ReadUTFString() 
{

    int size = ReadInt();
    String value;

    for (int i = 0; i < size; i++)
    {
       value +=  ReadChar();
    }
 
    return value;
}

char Stream::ReadChar()
{
    char value;
    SDL_RWread(f_file, &value, sizeof(char), 1);
    return value;
}

size_t  Stream::WriteChar(char value)
{
    return SDL_RWwrite(f_file, &value, sizeof(char), 1);
}

unsigned char Stream::ReadByte()
{
    unsigned char value;
    SDL_RWread(f_file, &value, sizeof(unsigned char), 1);
    return value;
}

short Stream::ReadShort()
{
    short value = ReadByte();
    value |= (ReadByte() << 8);
    return value;
}

int Stream::ReadInt()
{
    int value = ReadByte();
    value |= (ReadByte() << 8);
    value |= (ReadByte() << 16);
    value |= (ReadByte() << 24);
    return value;
}

long Stream::ReadLong()
{
    long value = ReadByte();
    value |= (ReadByte() << 8);
    value |= (ReadByte() << 16);
    value |= (ReadByte() << 24);
    return value;
}

float Stream::ReadFloat()
{
    float value;
    SDL_RWread(f_file, &value, sizeof(float), 1);
    return value;
}


double Stream::ReadDouble()
{
    double value;
    SDL_RWread(f_file, &value, sizeof(double), 1);
    return value;
}

size_t  Stream::WriteByte(unsigned char value)
{
   return SDL_RWwrite(f_file, &value, sizeof(unsigned char), 1);
}

size_t  Stream::WriteShort(short value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    return 2;
}

size_t  Stream::WriteInt(int value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    WriteByte((value >> 16) & 0xFF);
    WriteByte((value >> 24) & 0xFF);
    return 4;
}

size_t  Stream::WriteLong(long value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    WriteByte((value >> 16) & 0xFF);
    WriteByte((value >> 24) & 0xFF);
    return 4;
}

size_t  Stream::WriteFloat(float value)
{
    SDL_RWwrite(f_file, &value, sizeof(float), 1);
    return 4;
}

size_t  Stream::WriteDouble(double value)
{
    SDL_RWwrite(f_file, &value, sizeof(double), 1);
    return 8;
}

u64 Stream::Seek(u64 offset, bool relative)
{
    return SDL_RWseek(f_file, offset, relative ? RW_SEEK_CUR : RW_SEEK_SET);
}


FileStream::FileStream():Stream()
{   


}

FileStream::FileStream(const String &filePath, const String &mode):Stream()
{
    f_file = SDL_RWFromFile(filePath.c_str(), mode.c_str());
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, "[FILESTRAM] Cant open: %s", filePath.c_str());
        m_open = false;
        return;
    }

    SDL_RWseek(f_file, 0, RW_SEEK_END);
    m_size = SDL_RWtell(f_file);
    SDL_RWseek(f_file, 0, RW_SEEK_SET);
    m_open = true;
    LogInfo( "FILE: file  read: %s", filePath.c_str());
}

bool FileStream::Open(const String &filePath, const String &mode)
{
    f_file = SDL_RWFromFile(filePath.c_str(), mode.c_str());
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, " Cant open: %s", filePath.c_str());
        m_open = false;
        return false;
    }
    SDL_RWseek(f_file, 0, RW_SEEK_END);
    m_size = SDL_RWtell(f_file);
    SDL_RWseek(f_file, 0, RW_SEEK_SET);
    m_open = true;
    LogInfo( "FILE: file  read: %s", filePath.c_str());
    return true;
}

bool FileStream::Create(const String &filePath, bool overwrite)
{
    if (overwrite)
    {
        f_file = SDL_RWFromFile(filePath.c_str(), "wb");
    }
    else
    {
        f_file = SDL_RWFromFile(filePath.c_str(), "ab");
    }
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, " Cant create: %s", filePath.c_str());
        m_open = false;
        return false;
    }
    m_open = true;
    LogInfo( "FILE: file created: %s", filePath.c_str());
    return true;
}






ByteStream::ByteStream():Stream()
{
    m_open = false;
    m_size = 0;
    m_capacity = 0;
    m_offset = 0;
    m_data = nullptr;
    m_owner = false;

}

bool ByteStream::Load(const void *data, int size)
{
    f_file = SDL_RWFromConstMem(data, size);
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, " Cant open: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    LogInfo( "BYTE: file  read: %s", "ByteStream");
    return true;
}

bool ByteStream::Load(void *data, int size)
{
    f_file = SDL_RWFromMem(data, size);
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, " Cant open: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    LogInfo( "BYTE: file  read: %s", "ByteStream");
    return true;
}

bool ByteStream::Create(int size)
{
    m_data = malloc(size);
    f_file = SDL_RWFromMem(m_data, size);
    if (f_file == nullptr)
    {
        SDL_LogInfo(1, " Cant create: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    m_owner = true;
    LogInfo( "BYTE: file created: %s", "ByteStream");
    return true;
}

void ByteStream::Close()
{
    if (!m_open)
    {
        return ;
    }
    m_open = false;
    if (m_owner)
    {
        free(m_data);
    }
    Stream::Close();
}

void* ByteStream::GetPointer() const
{
    if (m_data == nullptr)
    {
        return nullptr;
    }
    return m_data;
}

void* ByteStream::GetPointer(u64 offset) const
{
    if (m_data == nullptr)
    {
        return nullptr;
    }
    return (void*)((u8*)m_data + offset);
}

StreamText::StreamText()
{
    m_data = nullptr;
    m_owner = false;
    m_position = 0;
    m_length = 0;
}

StreamText::StreamText(const String &text, bool copy)
{
    if (copy)
    {
        m_data = new char[text.length() + 1];
        std::strcpy(m_data, text.c_str());
        m_data[text.length()] = 0;
        m_owner = true;
    }
    else
    {
        m_data = (char*)text.c_str();
        m_owner = false;
    }
    m_length = text.length();
    m_position = 0;
}


StreamText::StreamText(const char *text, bool copy)
{
    if (copy)
    {
        m_data = new char[strlen(text) + 1];
        std::strcpy(m_data, text);
        m_data[strlen(text)] = 0;
        m_owner = true;
    }
    else
    {
        m_data = (char*)text;
        m_owner = false;
    }
    m_length = static_cast<u32>(strlen(text));
    m_position = 0;
}


StreamText::~StreamText()
{
    if (m_owner)
    {
        delete [] m_data;
    }
}

void StreamText::copy(const char *text)
{
    if (m_owner)
    {
        delete [] m_data;
    }
    m_length =  static_cast<u32>(strlen(text));
    m_data =  new char[m_length + 1];
     std::strcpy(m_data, text);
    m_data[m_length] = 0;
    m_owner = true;
    m_position = 0;
}

void StreamText::copy(const String &text)
{
    if (m_owner)
    {
        delete [] m_data;
    }
    m_length = text.length();
    m_data =  new char[m_length + 1];
    std::strcpy(m_data, text.c_str());
    m_data[m_length] = 0;
    m_owner = true;
    m_position = 0;
}



bool StreamText::read(char &value) 
{
    if (m_position >= m_length) {
        return false;
    }
    value = m_data[m_position++];
    return true;
}

bool StreamText::read(short &value)
 {
    char* endptr;
    long result = std::strtol(m_data + m_position, &endptr, 10);
    if (endptr == m_data + m_position)
        return false;
    value = static_cast<short>(result);
    m_position +=static_cast<u32>( endptr - (m_data + m_position) + 1);
    return true;
}

bool StreamText::read(int &value) 
{
    char* endptr;
    long result = std::strtol(m_data + m_position, &endptr, 10);
    if (endptr == m_data + m_position)
        return false;
    value = static_cast<int>(result);
    m_position +=static_cast<u32>( endptr - (m_data + m_position) + 1);
    return true;
}

bool StreamText::read(float &value) 
{
    char* endptr;
    value = std::strtof(m_data + m_position, &endptr);
    if (endptr == m_data + m_position)
        return false;
    m_position += static_cast<u32>(endptr - (m_data + m_position) + 1);
    return true;
}

bool StreamText::getline(String &line)
{
    if (m_position >= m_length)
    {
        return false;
    }
    u32 start = m_position;
    u32 end = start;
    while (end < m_length && m_data[end] != '\n')
        ++end;
    line = String(m_data + start, end - start);
    m_position = end + 1;
    return true;
    
}



StreamText& operator>>(StreamText& stream, char& value) 
{
    if (!stream.read(value))
        SDL_LogError(1, "Failed to read char from stream");
    return stream;
}

StreamText& operator>>(StreamText& stream, short& value)
 {
    if (!stream.read(value))
        SDL_LogError(1, "Failed to read short from stream");
    return stream;
}

StreamText& operator>>(StreamText& stream, int& value) 
{
    if (!stream.read(value))
        SDL_LogError(1, "Failed to read int from stream");
    return stream;
}

StreamText& operator>>(StreamText& stream, float& value) 
{
    if (!stream.read(value))
        SDL_LogError(1, "Failed to read float from stream");
    return stream;
}

StreamText& operator>>(StreamText& stream, String& value) 
{
    if (!stream.getline(value))
        SDL_LogError(1, "Failed to read line from stream");
    return stream;
}
