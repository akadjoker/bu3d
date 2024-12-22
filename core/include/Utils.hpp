#pragma once

#include "Config.hpp"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define MAX_TEXT_BUFFER_LENGTH              1024      




const char *strprbrk(const char *s, const char *charset);

bool SaveDataFile(const char *fileName, void *data, unsigned int bytesToWrite);
char* LoadTextFile(const char *fileName);

unsigned char *LoadDataFile(const char *fileName, unsigned int *bytesRead);
 bool SaveTextFile(const char *fileName, char *text);
 const char *FormatText(const char *text, ...);
 
 void LogError(const char *msg, ...);
 void LogInfo(const char *msg, ...);
 void LogWarning(const char *msg, ...);
