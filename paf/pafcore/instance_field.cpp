#include "instance_field.h"
#include "instance_field.mh"
#include "instance_field.ic"
#include "instance_field.mc"

BEGIN_PAFCORE

InstanceField::InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, size_t offset, size_t arraySize, bool constant, TypeCompound tc)
: Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_offset = offset;
	m_arraySize = arraySize;
	m_constant = constant;
	m_typeCompound = tc;
}

ObserverPtr<Type> InstanceField::type() const
{
	return m_type;
}

ObserverPtr<ClassType> InstanceField::objectType() const
{
	return m_objectType;
}

bool InstanceField::isArray() const
{
	return tc_observer_array == m_typeCompound
		|| tc_unique_array == m_typeCompound
		|| tc_shared_array == m_typeCompound;
}

bool InstanceField::isPointer() const
{
	return tc_observer_ptr == m_typeCompound
		|| tc_unique_ptr == m_typeCompound
		|| tc_shared_ptr == m_typeCompound;
}

bool InstanceField::isConstant() const
{
	return m_constant;
}

size_t InstanceField::offset() const
{
	return m_offset;
}


END_PAFCORE
