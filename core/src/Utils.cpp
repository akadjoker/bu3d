
#include "Utils.hpp"


 const char *strprbrk(const char *s, const char *charset)
{
    const char *latestMatch = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) { }
    return latestMatch;
}

unsigned char* LoadDataFile(const char* fileName, unsigned int* bytesRead)
{
    unsigned char* data = NULL;
    *bytesRead = 0;

 
        SDL_RWops* file = SDL_RWFromFile(fileName, "rb");


        if (file != NULL)
        {
            unsigned int size = (int)SDL_RWsize(file);

            if (size > 0)
            {
                data = (unsigned char*)malloc(size * sizeof(unsigned char));

                unsigned int count = (unsigned int)SDL_RWread(file, data, sizeof(unsigned char), size);
                *bytesRead = count;

                 SDL_LogInfo(0, "FILEIO: [%s] File loaded successfully", fileName);

            }
            else SDL_LogError(0, "FILEIO: [%s] Failed to read file", fileName);
            SDL_RWclose(file);
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open file", fileName);

    return data;
}

bool SaveDataFile(const char* fileName, void* data, unsigned int bytesToWrite)
{
    bool success = false;

   
        SDL_RWops* file = SDL_RWFromFile(fileName, "wb");
        if (file != NULL)
        {
            unsigned int count = (unsigned int)SDL_RWwrite(file, data, sizeof(unsigned char), bytesToWrite);
            if (count == 0)SDL_LogError(0, "FILEIO: [%s] Failed to write file", fileName);

            else  SDL_LogInfo(0, "FILEIO: [%s] File saved successfully", fileName);

            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open file", fileName);
 
    return success;
}

char* LoadTextFile(const char* fileName)
{
    char* text = NULL;

  
        SDL_RWops* textFile = SDL_RWFromFile(fileName, "rt");
        if (textFile != NULL)
        {
            unsigned  int size = (int)SDL_RWsize(textFile);
            if (size > 0)
            {
                text = (char*)malloc((size + 1) * sizeof(char));
                unsigned int count = (unsigned int)SDL_RWread(textFile, text, sizeof(char), size);
                if (count < size) text = (char*)realloc(text, count + 1);
                text[count] = '\0';

                SDL_LogInfo(0, "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else SDL_LogError(0, "FILEIO: [%s] Failed to read text file", fileName);

            SDL_RWclose(textFile);
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open text file", fileName);
    

    return text;
}

bool SaveTextFile(const char* fileName, char* text)
{
    bool success = false;

        SDL_RWops* file = SDL_RWFromFile(fileName, "wt");
        if (file != NULL)
        {
            size_t strLen = SDL_strlen(text);
            SDL_RWwrite(file, text, 1, strLen);
            
            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open text file", fileName);

    return success;
}
