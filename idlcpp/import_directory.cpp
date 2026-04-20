#include "import_directory.h"
#include "platform.h"
#include <filesystem>

namespace
{
	std::string NormalizeDirectory(const char* dir)
	{
		std::string fullName;
		normalizeFileName(fullName, dir);
		FormatPathForInclude(fullName);
		if (!fullName.empty() && fullName.back() != '/')
		{
			fullName.push_back('/');
		}
		return fullName;
	}
}

ImportDirectories g_importDirectories;

ImportDirectories::ImportDirectories()
{
	m_hasCurrentDirectory = false;
}

void ImportDirectories::calcImportDirectories(const char* fileName)
{
	auto it = m_directories.begin();
	auto end = m_directories.end();
	for (; it != end; ++it)
	{
		*it = NormalizeDirectory(it->c_str());
	}
}

void ImportDirectories::addImportDirectory(const char* dir)
{
	m_directories.insert(m_directories.begin(), dir);
}

void ImportDirectories::setCurrentDirectory(const char* dir)
{
	std::string str = NormalizeDirectory(dir);
	if(str.length() > 0)
	{
		if(m_hasCurrentDirectory)
		{
			m_directories.pop_back();
		}
		m_directories.push_back(str);
		m_hasCurrentDirectory = true;
	}
}

