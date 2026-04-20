#include "type_alias.h"
#include "type_alias.mh"
#include "type_alias.ic"
#include "type_alias.mc"
#include "name_space.h"

BEGIN_PAFCORE

TypeAlias::TypeAlias(const char* name, Type* type, const char* declarationFile) :
	Metadata(name),
	m_type(type),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

TypeAlias::~TypeAlias()
{
	if (m_enclosing && name_space == m_enclosing->_category_())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}


Type* TypeAlias::type() const
{
	return m_type;
}


END_PAFCORE
