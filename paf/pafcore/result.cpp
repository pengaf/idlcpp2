#include "result.h"
#include "result.mh"
#include "result.ic"
#include "result.mc"

BEGIN_PAFCORE

Result::Result(Type* type, bool constant, Passing passing, TypeCompound typeCompound)
: Metadata(0)
{
	m_type = type;
	m_constant = constant;
	m_passing = passing;
	m_typeCompound = typeCompound;
}

bool Result::isConstant() const
{
	return m_constant;
}

ObserverPtr<Type> Result::type() const
{
	return m_type;
}

byte_t Result::typeCompound() const
{
	return m_typeCompound;
}

bool Result::byValue() const
{
	return by_value == m_passing && tc_none == m_typeCompound;
}

bool Result::byRef() const
{
	return by_ref == m_passing && tc_none == m_typeCompound;
}

bool Result::byPtr() const
{
	return tc_none != m_typeCompound;
}

END_PAFCORE
