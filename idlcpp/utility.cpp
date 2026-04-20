#include "utility.h"
#include <string.h>
#include <string>


const char g_tabs[] = { "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" };
const char g_strSpaces[] = { "                " };

char g_lastWrittenChar = 0;

bool isNumAlpha_(char c)
{
	return (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || '_' == c);
}

void writeStringToFile(const char* str, size_t length, FILE* file, int indentation)
{
	if (file)
	{
		if (indentation > 0)
		{
			fwrite(g_tabs, indentation, 1, file);
			g_lastWrittenChar = '\t';
		}
		fwrite(str, length, 1, file);
		if (length > 0)
		{
			g_lastWrittenChar = str[length - 1];
		}
	}
}

void writeStringToFile(const char* str, FILE* file, int indentation)
{
	writeStringToFile(str, strlen(str), file, indentation);
}

void writeSpaceToFile(FILE* file)
{
	writeStringToFile(" ", 1, file, 0);
}

char GetLastWrittenChar()
{
	return g_lastWrittenChar;
}

void ConvertFileBaseNameToSnakeCase(std::string& path)
{
	size_t slashPos = path.find_last_of("\\/");
	size_t nameStart = (std::string::npos == slashPos) ? 0 : slashPos + 1;
	size_t dotPos = path.find_last_of('.');
	if (std::string::npos != dotPos && dotPos < nameStart)
	{
		dotPos = std::string::npos;
	}

	size_t nameEnd = (std::string::npos == dotPos) ? path.length() : dotPos;
	std::string name = path.substr(nameStart, nameEnd - nameStart);
	std::string snakeName;
	snakeName.reserve(name.length() * 2);

	for (size_t i = 0; i < name.length(); ++i)
	{
		char ch = name[i];
		if ('A' <= ch && ch <= 'Z')
		{
			if (i > 0)
			{
				char prev = name[i - 1];
				char next = (i + 1 < name.length()) ? name[i + 1] : 0;
				bool needsUnderscore =
					(('a' <= prev && prev <= 'z') || ('0' <= prev && prev <= '9'))
					|| (('A' <= prev && prev <= 'Z') && ('a' <= next && next <= 'z'));
				if (needsUnderscore && !snakeName.empty() && '_' != snakeName.back())
				{
					snakeName += '_';
				}
			}
			snakeName += static_cast<char>(ch - 'A' + 'a');
		}
		else
		{
			snakeName += ch;
		}
	}

	path.replace(nameStart, nameEnd - nameStart, snakeName);
}
