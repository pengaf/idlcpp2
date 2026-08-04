#include "instance_property.h"
#include "instance_property.mh"
#include "instance_property.ic"
#include "instance_property.mc"

BEGIN_PAFCORE

InstanceProperty::InstanceProperty(
	const char* name,
	Attributes* attributes,
	ClassType* objectType,
	Type* type,
	TypeCompound typeCompound,
	InstancePropertyEnumerate enumerate,
	InstancePropertyGet get,
	InstancePropertySet set)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_get = get;
	m_set = set;
	m_kind = PropertyKind::simple_property;
	m_serializable = !_hasAttribute_("NonSerialized");
}

InstanceProperty::InstanceProperty(
	const char* name,
	Attributes* attributes,
	ClassType* objectType,
	Type* type,
	TypeCompound typeCompound,
	InstancePropertyEnumerate enumerate,
	InstancePropertyArrayGet arrayGet,
	InstancePropertyArraySet arraySet,
	InstancePropertyArraySize arraySize,
	InstancePropertyArrayResize arrayResize)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_arrayGet = arrayGet;
	m_arraySet = arraySet;
	m_arraySize = arraySize;
	m_arrayResize = arrayResize;
	m_kind = arrayResize ? PropertyKind::dynamic_array_property : PropertyKind::fixed_array_property;
	m_serializable = !_hasAttribute_("NonSerialized");
}

InstanceProperty::InstanceProperty(
	const char* name,
	Attributes* attributes,
	ClassType* objectType,
	Type* type,
	TypeCompound typeCompound,
	InstancePropertyEnumerate enumerate,
	InstancePropertyListGet listGet,
	InstancePropertyListSet listSet,
	InstancePropertyListIterate listIterate,
	InstancePropertyListInsert listInsert,
	InstancePropertyListErase listErase)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_listGet = listGet;
	m_listSet = listSet;
	m_listIterate = listIterate;
	m_listInsert = listInsert;
	m_listErase = listErase;
	m_kind = PropertyKind::list_property;
	m_serializable = !_hasAttribute_("NonSerialized");
}

ClassType* InstanceProperty::objectType() const
{
	return m_objectType;
}

bool InstanceProperty::isSimple() const
{
	return PropertyKind::simple_property == m_kind;
}

bool InstanceProperty::isFixedArray() const
{
	return PropertyKind::fixed_array_property == m_kind;
}

bool InstanceProperty::isDynamicArray() const
{
	return PropertyKind::dynamic_array_property == m_kind;
}

bool InstanceProperty::isList() const
{
	return PropertyKind::list_property == m_kind;
}

bool InstanceProperty::hasEnumerate() const
{
	return (0 != m_enumerate);
}

bool InstanceProperty::hasGet() const
{
	return (0 != m_get);
}

bool InstanceProperty::hasSet() const
{
	return (0 != m_set);
}

Type* InstanceProperty::type() const
{
	return m_type;
}

TypeCompound InstanceProperty::typeCompound() const
{
	return m_typeCompound;
}

bool InstanceProperty::serializable() const
{
	return m_serializable;
}
END_PAFCORE