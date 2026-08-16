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
	InstancePropertyScalarGet get,
	InstancePropertyScalarSet set)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_collectionIterate = nullptr;
	m_scalarGet = get;
	m_scalarSet = set;
	m_kind = PropertyKind::scalar_property;
	m_serializable = !_hasAttribute_("NonSerialized");
}

InstanceProperty::InstanceProperty(
	const char* name,
	Attributes* attributes,
	ClassType* objectType,
	Type* type,
	TypeCompound typeCompound,
	InstancePropertyEnumerate enumerate,
	InstancePropertyCollectionIterate collectionIterate,
	InstancePropertyCollectionGet collectionGet,
	InstancePropertyCollectionSet collectionSet,
	InstancePropertyArraySize arraySize,
	InstancePropertyArrayResize arrayResize)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_collectionIterate = collectionIterate;
	m_collectionGet = collectionGet;
	m_collectionSet = collectionSet;
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
	InstancePropertyCollectionIterate collectionIterate,
	InstancePropertyCollectionGet collectionGet,
	InstancePropertyCollectionSet collectionSet,
	InstancePropertyListInsert listInsert,
	InstancePropertyListErase listErase)
	:Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_collectionIterate = collectionIterate;
	m_collectionGet = collectionGet;
	m_collectionSet = collectionSet;
	m_listInsert = listInsert;
	m_listErase = listErase;
	m_kind = PropertyKind::list_property;
	m_serializable = !_hasAttribute_("NonSerialized");
}

END_PAFCORE