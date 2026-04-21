#include "type.h"
#include "type.mh"
#include "type.ic"
#include "type.mc"
#include "name_space.h"

BEGIN_PAFCORE

Type::Type(const char* name, Category category, const char* declarationFile) :
	Metadata(name),
	m_category(category),
	m_size(0),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

Type::~Type()
{
	if (m_enclosing && name_space == m_enclosing->_category_())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}

void Type::destroyInstance(void* address)
{
}

void Type::destroyArray(void* address)
{
}

bool Type::assign(void* dst, const void* src)
{
	return false;
}

bool Type::getSmartPointer(Variant& value, const void* address, bool constant, Metadata::TypeCompound typeCompound)
{
	(void)value;
	(void)address;
	(void)constant;
	(void)typeCompound;
	return false;
}

bool Type::setSmartPointer(void* address, Variant& value, Metadata::TypeCompound typeCompound)
{
	(void)address;
	(void)value;
	(void)typeCompound;
	return false;
}

//Metadata* Type::findMember(const char* name)
//{
//	return 0;
//}

//Metadata* Type::findTypeMember(const char* name)
//{
//	return 0;
//}

END_PAFCORE
