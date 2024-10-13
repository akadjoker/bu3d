#pragma once


#include "Config.hpp"
#include "String.hpp"





class    StreamText 
{
public:
    StreamText();
    StreamText(const String& text, bool copy=false);
    StreamText(const char* text,   bool copy=false);
    ~StreamText();

    void copy(const char* text);
    void copy(const String& text);


    bool read(char& value);
    bool read(short& value);
    bool read(int& value);
    bool read(float& value);


    bool getline(String& line);

    friend StreamText& operator>>(StreamText& stream, char& value);
    friend StreamText& operator>>(StreamText& stream, short& value);
    friend StreamText& operator>>(StreamText& stream, int& value);
    friend StreamText& operator>>(StreamText& stream, float& value);
    friend StreamText& operator>>(StreamText& stream, String& value);


 
private:
    char *m_data;
    bool m_owner;
    u32 m_position;
    u32 m_length;
};


class     Stream 
{
    public:
        Stream();
        virtual ~Stream();


 
    u64 Read(void* buffer, int size) ;
    u64 Write(const void* buffer, int size) ;
    u64 Cursor() const ;
    u64 Seek(u64 offset, bool relative = false) ;

    virtual void Close() ;

    u64  Size() const { return m_size; }
    bool IsOpen() const { return m_open; }

    char ReadChar();
    unsigned char ReadByte();
    short         ReadShort();
    int           ReadInt();
    long          ReadLong();
    float         ReadFloat();
    double        ReadDouble();



    String ReadString() ;
    size_t  WriteString(const String& value);

    size_t  WriteUTFString(const String& value);
    String  ReadUTFString()  ;

    size_t  WriteChar(char value);
    size_t  WriteByte(unsigned char value);
    size_t  WriteShort(short value);
    size_t  WriteInt(int value);
    size_t  WriteLong(long value);
    size_t  WriteFloat(float value);
    size_t  WriteDouble(double value);
protected:
    SDL_RWops* f_file;
    bool m_open;
    u64 m_size;


};

class     FileStream : public Stream
{
public:
    FileStream();
    FileStream(const String& filePath, const String& mode);
    bool Open(const String& filePath, const String& mode);
    bool Create(const String& filePath, bool overwrite = false);

};


class     ByteStream : public Stream
{
public:
    ByteStream();


    bool Load(const void* data, int size);
    bool Load(void* data, int size);
    bool Create(int size);

    void Close() override;



    void* GetPointer() const;

    void* GetPointer(u64 offset) const;


    private:
        void* m_data;
        bool m_owner;
        u64 m_offset;
        u64 m_capacity;
        u64 m_size;

 
};

