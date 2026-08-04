#include "result.h"
#include "result.mh"
#include "result.ic"
#include "result.mc"

BEGIN_PAFCORE

Result::Result(const char* name, Type* type, TypeCompound typeCompound, bool byRef)
: Metadata(name)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_byRef = byRef;
}

END_PAFCORE
