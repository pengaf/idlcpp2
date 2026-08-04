#include "type.h"
#include "type.mh"
#include "type.ic"
#include "type.mc"
#include "name_space.h"

BEGIN_PAFCORE

Type::Type(const char* name, MetadataKind kind, const char* declarationFile) :
	Metadata(name),
	m_kind(kind),
	m_size(0),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

Type::~Type()
{
	if (m_enclosing && MetadataKind::name_space == m_enclosing->_kind_())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}

void Type::destruct(void* address, size_t count)
{
}

void Type::assign(void* dst, const void* src)
{
}

size_t Type::_size_() const
{
	return m_size;
}

bool Type::isPrimitive() const
{
	return MetadataKind::primitive_instance == m_kind;
}

bool Type::isEnum() const
{
	return MetadataKind::enum_instance == m_kind;
}

bool Type::isClass() const
{
	return MetadataKind::class_instance == m_kind;
}

const char* Type::getDeclarationFile() const
{
	return m_declarationFile;
}

END_PAFCORE
