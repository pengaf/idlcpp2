#include "argument.h"
#include "argument.mh"
#include "argument.ic"
#include "argument.mc"

BEGIN_PAFCORE

Argument::Argument(const char* name, Type* type, Passing passing, bool constant)
: Metadata(name)
{
	m_type = type;
	m_passing = passing;
	m_constant = constant;
}

Type* Argument::type() const
{
	return m_type;
}

bool Argument::byValue() const
{
	return by_value == m_passing;
}

bool Argument::byRef() const
{
	return by_ref == m_passing;
}

bool Argument::byPtr() const
{
	return by_ptr == m_passing;
}

bool Argument::byOutPtr() const
{
	return by_out_ptr == m_passing;
}

bool Argument::byOutRef() const
{
	return by_out_ref == m_passing;
}

bool Argument::byNewPtr() const
{
	return by_new_ptr == m_passing;
}

bool Argument::byNewRef() const
{
	return by_new_ref == m_passing;
}

bool Argument::byNewArrayPtr() const
{
	return by_new_array_ptr == m_passing;
}

bool Argument::byNewArrayRef() const
{
	return by_new_array_ref == m_passing;
}

bool Argument::isConstant() const
{
	return m_constant;
}


END_PAFCORE