#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <cstddef>
#include <cstring>
#include <string>

#if !defined(_MSC_VER)
template<size_t N>
inline int sprintf_s(char (&buffer)[N], const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, N, format, args);
	va_end(args);
	return result;
}

template <size_t N>
inline int strcpy_s(char(&dest)[N], const char* src)
{
	if (!src) return 1;
	size_t len = strlen(src);
	if (len >= N) return 2;
	strcpy(dest, src);
	return 0;
}
#endif

bool isNumAlpha_(char c);
void writeStringToFile(const char* str, FILE* file, int indentation = 0);
void writeStringToFile(const char* str, size_t length, FILE* file, int indentation = 0);
void writeSpaceToFile(FILE* file);
char GetLastWrittenChar();
void ConvertFileBaseNameToSnakeCase(std::string& path);
