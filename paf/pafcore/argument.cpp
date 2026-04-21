#include "argument.h"
#include "argument.mh"
#include "argument.ic"
#include "argument.mc"

BEGIN_PAFCORE

Argument::Argument(const char* name, Type* type, Passing passing, TypeCompound typeCompound, byte_t out, bool constant)
: Metadata(name)
{
	m_type = type;
	m_passing = passing;
	m_typeCompound = typeCompound;
	m_out = out;
	m_constant = constant;
}

ObserverPtr<Type> Argument::type() const
{
	return m_type;
}

byte_t Argument::typeCompound() const
{
	return m_typeCompound;
}

bool Argument::byValue() const
{
	return by_value == m_passing && tc_none == m_typeCompound && 0 == m_out;
}

bool Argument::byRef() const
{
	return by_ref == m_passing && tc_none == m_typeCompound && 0 == m_out;
}

bool Argument::byPtr() const
{
	return tc_none != m_typeCompound && 0 == m_out;
}

bool Argument::isOutput() const
{
	return 0 != m_out;
}

bool Argument::isConstant() const
{
	return m_constant;
}


END_PAFCORE
