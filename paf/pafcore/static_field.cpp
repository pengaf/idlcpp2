#include "static_field.h"
#include "static_field.mh"
#include "static_field.ic"
#include "static_field.mc"

BEGIN_PAFCORE

StaticField::StaticField(const char* name, Attributes* attributes, Type* type, size_t offset, size_t arraySize, bool constant, TypeCompound tc)
: Metadata(name, attributes)
{
	m_type = type;
	m_address = offset;
	m_arraySize = arraySize;
	m_constant = constant;
	m_typeCompound = tc;
}

ObserverPtr<Type> StaticField::type() const
{
	return m_type;
}

bool StaticField::isArray() const
{
	return tc_observer_array == m_typeCompound
		|| tc_unique_array == m_typeCompound
		|| tc_shared_array == m_typeCompound;
}

bool StaticField::isPointer() const
{
	return tc_observer_ptr == m_typeCompound
		|| tc_unique_ptr == m_typeCompound
		|| tc_shared_ptr == m_typeCompound;
}

bool StaticField::isConstant() const
{
	return m_constant;
}

size_t StaticField::address() const
{
	return m_address;
}


END_PAFCORE
