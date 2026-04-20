#include "platform.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>

namespace
{
	const char* FindLastPathSeparator(const char* fileName)
	{
		const char* lastSlash = strrchr(fileName, '/');
		const char* lastBackSlash = strrchr(fileName, '\\');
		if (lastSlash == 0)
		{
			return lastBackSlash;
		}
		if (lastBackSlash == 0)
		{
			return lastSlash;
		}
		return lastSlash > lastBackSlash ? lastSlash : lastBackSlash;
	}
}

bool fileExisting(const char* fileName)
{
	std::error_code ec;
	return std::filesystem::is_regular_file(std::filesystem::u8path(fileName), ec);
}

bool isAbsolutePath(const char* fileName)
{
	return std::filesystem::path(fileName).is_absolute();
}

void normalizeFileName(std::string& str, const char* fileName)
{
	std::error_code ec;
	std::filesystem::path filePath = std::filesystem::u8path(fileName);
	std::filesystem::path normalizedPath = std::filesystem::absolute(filePath, ec);
	if (ec)
	{
		ec.clear();
		normalizedPath = filePath;
	}
	normalizedPath = normalizedPath.lexically_normal();
	str = normalizedPath.generic_u8string();
}

const char* getExtNameBegin(const char* normalizedFileName)
{
	const char* lastDot = strrchr(normalizedFileName, '.');
	const char* lastSlash = FindLastPathSeparator(normalizedFileName);
	if (lastDot == 0 || (lastSlash != 0 && lastDot < lastSlash))
	{
		return 0;
	}
	return lastDot;
}

const char* getDirNameEnd(const char* normalizedFileName)
{
	return FindLastPathSeparator(normalizedFileName);
}

void GetRelativePath(std::string& str, const char* fileFrom, const char* fileTo)
{
	std::error_code ec;
	std::filesystem::path fromPath = std::filesystem::u8path(fileFrom).parent_path();
	std::filesystem::path toPath = std::filesystem::u8path(fileTo);
	std::filesystem::path relativePath = std::filesystem::relative(toPath, fromPath, ec);
	if (ec)
	{
		str = fileTo;
		return;
	}
	str = relativePath.generic_u8string();
	if (str == ".")
	{
		str.clear();
	}
}

void FormatPathForInclude(std::string& str)
{
	std::replace(str.begin(), str.end(), '\\', '/');
}

void FormatPathForLine(std::string& str)
{
	std::replace(str.begin(), str.end(), '\\', '/');
}

bool compareFileName(const std::string& str1, const std::string& str2)
{
#if defined(_WIN32)
	return std::lexicographical_compare(
		str1.begin(),
		str1.end(),
		str2.begin(),
		str2.end(),
		[](unsigned char lhs, unsigned char rhs)
		{
			return std::tolower(lhs) < std::tolower(rhs);
		});
#else
	return str1 < str2;
#endif
}
