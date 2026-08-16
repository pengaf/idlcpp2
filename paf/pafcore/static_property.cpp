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
	StaticPropertyScalarGet get,
	StaticPropertyScalarSet set)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_scalarGet = get;
	m_scalarSet = set;
	m_kind = PropertyKind::scalar_property;
}

StaticProperty::StaticProperty(
	const char* name,
	Attributes* attributes,
	Type* type,
	TypeCompound typeCompound,
	StaticPropertyEnumerate enumerate, 
	StaticPropertyCollectionIterate arrayIterate,
	StaticPropertyCollectionGet arrayGet,
	StaticPropertyCollectionSet arraySet,
	StaticPropertyArraySize arraySize,
	StaticPropertyArrayResize arrayResize)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_collectionIterate = arrayIterate;
	m_collectionGet = arrayGet;
	m_collectionSet = arraySet;
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
	StaticPropertyCollectionIterate listIterate,
	StaticPropertyCollectionGet listGet,
	StaticPropertyCollectionSet listSet,
	StaticPropertyListInsert listInsert,
	StaticPropertyListErase listErase)
	: Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_enumerate = enumerate;
	m_collectionIterate = listIterate;
	m_collectionGet = listGet;
	m_collectionSet = listSet;
	m_listInsert = listInsert;
	m_listErase = listErase;
	m_kind = PropertyKind::list_property;
}


//bool StaticProperty::serializable() const
//{
//	return m_serializable;
//}

END_PAFCORE
