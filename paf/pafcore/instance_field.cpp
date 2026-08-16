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

END_PAFCORE
