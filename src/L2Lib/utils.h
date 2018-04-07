#pragma once

char DLLEXPORT *UTIL_CopyString( const char * );
bool DLLEXPORT UTIL_Match( const char* string, const char* pattern );
void DLLEXPORT UTIL_StripExtension( char * );
void DLLEXPORT UTIL_FixSlashes( char * );
char DLLEXPORT *UTIL_FileName( char * );
char DLLEXPORT *UTIL_FileExt( char * );
char DLLEXPORT *UTIL_FilePath( const char * );
uint32 DLLEXPORT  UTIL_32BitChecksum( const void *, int );
int8 DLLEXPORT UTIL_strcmpi(const char *str1, const char *str2);

jfArray<char *, uint32> DLLEXPORT *UTIL_FindFiles(char *dir, char *pattern);
