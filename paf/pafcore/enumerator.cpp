#include "enum_member.h"
#include "enum_member.mh"
#include "enum_member.ic"
#include "enum_member.mc"

BEGIN_PAFCORE

Enumerator::Enumerator(const char* name, Attributes* attributes, Type* type, int value)
: Metadata(name, attributes)
{
	m_type = type;
	m_value = value;
}

ObserverPtr<Type> Enumerator::_type_() const
{
	return m_type;
}

int Enumerator::_value_() const
{
	return m_value;
}

END_PAFCORE
