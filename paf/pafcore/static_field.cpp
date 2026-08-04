#include "static_field.h"
#include "static_field.mh"
#include "static_field.ic"
#include "static_field.mc"

BEGIN_PAFCORE

StaticField::StaticField(const char* name, Attributes* attributes, Type* type, size_t offset, size_t arraySize, TypeCompound typeCompound)
: Metadata(name, attributes)
{
	m_type = type;
	m_address = offset;
	m_arraySize = arraySize;
	m_typeCompound = typeCompound;
}

Type* StaticField::type() const
{
	return m_type;
}

TypeCompound StaticField::typeCompound() const
{
	return m_typeCompound;
}

bool StaticField::isArray() const
{
	return m_arraySize > 0;
}

size_t StaticField::address() const
{
	return m_address;
}

END_PAFCORE
