#include "static_property.h"
#include "static_property.mh"
#include "static_property.ic"
#include "static_property.mc"

BEGIN_PAFCORE

StaticProperty::StaticProperty(
	const char* name,
	Attributes* attributes,
	Type* type,
	TypeCompound typeCompound,
	StaticPropertyEnumerate enumerate, 
	StaticPropertyGet get,
	StaticPropertySet set)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_get = get;
	m_set = set;
	m_kind = PropertyKind::simple_property;
}

StaticProperty::StaticProperty(
	const char* name,
	Attributes* attributes,
	Type* type,
	TypeCompound typeCompound,
	StaticPropertyEnumerate enumerate, 
	StaticPropertyArrayGet arrayGet,
	StaticPropertyArraySet arraySet,
	StaticPropertyArraySize arraySize,
	StaticPropertyArrayResize arrayResize)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_arrayGet = arrayGet;
	m_arraySet = arraySet;
	m_arraySize = arraySize;
	m_arrayResize = arrayResize;
	m_kind = arrayResize ? PropertyKind::dynamic_array_property : PropertyKind::fixed_array_property;
}

StaticProperty::StaticProperty(
	const char* name,
	Attributes* attributes,
	Type* type,
	TypeCompound typeCompound,
	StaticPropertyEnumerate enumerate,
	StaticPropertyListGet listGet,
	StaticPropertyListSet listSet,
	StaticPropertyListIterate listIterate,
	StaticPropertyListInsert listInsert,
	StaticPropertyListErase listErase)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_listGet = listGet;
	m_listSet = listSet;
	m_listIterate = listIterate;
	m_listInsert = listInsert;
	m_listErase = listErase;
	m_kind = PropertyKind::list_property;
}


bool StaticProperty::isSimple() const
{
	return PropertyKind::simple_property == m_kind;
}

bool StaticProperty::isFixedArray() const
{
	return PropertyKind::fixed_array_property == m_kind;
}

bool StaticProperty::isDynamicArray() const
{
	return PropertyKind::dynamic_array_property == m_kind;
}

bool StaticProperty::isList() const
{
	return PropertyKind::list_property == m_kind;
}

bool StaticProperty::hasEnumerate() const
{
	return (0 != m_enumerate);
}

bool StaticProperty::hasGet() const
{
	return (0 != m_get);
}

bool StaticProperty::hasSet() const
{
	return (0 != m_set);
}

Type* StaticProperty::type() const
{
	return m_type;
}

TypeCompound StaticProperty::typeCompound() const
{
	return m_typeCompound;
}

//bool StaticProperty::serializable() const
//{
//	return m_serializable;
//}

END_PAFCORE
