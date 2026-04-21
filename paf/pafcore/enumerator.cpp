#include "enumerator.h"
#include "enumerator.mh"
#include "enumerator.ic"
#include "enumerator.mc"

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
