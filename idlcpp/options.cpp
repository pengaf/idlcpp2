#include "options.h"
#include "platform.h"
#include <algorithm>

Options::Options()
{
	m_metaTypePostfix = "_Type";
	m_subclassProxyPostfix = "_Proxy";
	m_internalSourceFilePostfix = ".ic";
	m_metaHeaderFilePostfix = ".mh";
	m_metaSourceFilePostfix = ".mc";
	m_outputLineDirective = false;
	m_strictArgumentsCount = false;
	m_checkConstant = false;
	//m_forceU8AttributeContent = true;
}


void Options::setPafcorePath(const char* path)
{
	normalizeFileName(m_pafcorePath, path);
	FormatPathForInclude(m_pafcorePath);
	if(m_pafcorePath.length() > 0 && m_pafcorePath.back() != '/')
	{
		m_pafcorePath += '/';
	}
}

Options g_options;