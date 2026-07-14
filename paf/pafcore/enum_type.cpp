#include "enum_type.h"
#include "enum_type.mh"
#include "enum_type.ic"
#include "enum_type.mc"
#include "instance_property.h"
#include <algorithm>

BEGIN_PAFCORE

EnumType::EnumType(const char* name, const char* declarationFile)
: Type(name, enum_instance, declarationFile)
{
	m_enumerators = 0;
	m_enumeratorCount = 0;
	static pafcore::InstanceProperty s_properties[] = 
	{
		::pafcore::InstanceProperty("_name_", 0, 0, RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), false, Enum_get__name_, 0, 0, 0),
	};
	m_instanceProperties = s_properties;
	m_instancePropertyCount = paf_array_size_of(s_properties);
}

size_t EnumType::_getEnumeratorCount_()
{
	return m_enumeratorCount;
}

ObserverPtr<Enumerator> EnumType::_getEnumerator_(size_t index)
{
	if(index < m_enumeratorCount)
	{
		return &m_enumerators[index];
	}
	return nullptr;
}

ObserverPtr<Enumerator> EnumType::_getEnumeratorByValue_(int value)
{
	for(size_t i = 0; i < m_enumeratorCount; ++i)
	{
		Enumerator* enum_member = &m_enumerators[i];
		if(value == enum_member->m_value)
		{
			return enum_member;
		}
	}
	return nullptr;
}

ObserverPtr<Enumerator> EnumType::_getEnumeratorByName_(string_t name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return nullptr;
}

Enumerator* EnumType::findEnumerator(const char* name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}

pafcore::ErrorCode EnumType::Enum_get__name_(pafcore::InstanceProperty* instanceProperty, pafcore::Variant* that, pafcore::Variant* value)
{
	if(enum_instance != that->m_type->m_kind)
	{
		return pafcore::e_invalid_this_type;
	}
	int e;
	if(!that->castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &e))
	{
		return pafcore::e_invalid_this_type;
	}
	EnumType* enumType = static_cast<EnumType*>(that->m_type);
	Enumerator* enum_member = enumType->_getEnumeratorByValue_(e);
	if(0 == enum_member)
	{	
		return pafcore::e_invalid_this_type;
	}
	string_t res = enum_member->_name_();
	value->assignPrimitive(RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), &res);
	return pafcore::s_ok;
}

Metadata* EnumType::findMember(const char* name)
{
	Metadata dummy(name);
	InstanceProperty* res = std::lower_bound(m_instanceProperties, m_instanceProperties + m_instancePropertyCount, dummy);
	if (m_instanceProperties + m_instancePropertyCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}


END_PAFCORE
