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

END_PAFCORE
