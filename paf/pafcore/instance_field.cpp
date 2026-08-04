#include "instance_field.h"
#include "instance_field.mh"
#include "instance_field.ic"
#include "instance_field.mc"

BEGIN_PAFCORE

InstanceField::InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, size_t offset, size_t arraySize, TypeCompound typeCompound)
: Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_offset = offset;
	m_arraySize = arraySize;
	m_typeCompound = typeCompound;
}

Type* InstanceField::type() const
{
	return m_type;
}

ClassType* InstanceField::objectType() const
{
	return m_objectType;
}

bool InstanceField::isArray() const
{
	return m_arraySize > 0;
}

TypeCompound InstanceField::typeCompound() const
{
	return m_typeCompound;
}

size_t InstanceField::offset() const
{
	return m_offset;
}

END_PAFCORE
