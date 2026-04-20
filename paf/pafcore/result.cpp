#include "result.h"
#include "result.mh"
#include "result.ic"
#include "result.mc"

BEGIN_PAFCORE

Result::Result(Type* type, bool constant, Passing passing)
: Metadata(0)
{
	m_type = type;
	m_constant = constant;
	m_passing = passing;
}

bool Result::isConstant() const
{
	return m_constant;
}

Type* Result::type() const
{
	return m_type;
}

bool Result::byValue() const
{
	return by_value == m_passing;
}
bool Result::byRef() const
{
	return by_ref == m_passing;
}
bool Result::byPtr() const
{
	return by_ptr == m_passing;
}

bool Result::byNew() const
{
	return by_new == m_passing;
}

bool Result::byNewArray() const
{
	return by_new_array == m_passing;
}

END_PAFCORE