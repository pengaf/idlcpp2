#include "parameter.h"
#include "parameter.mh"
#include "parameter.ic"
#include "parameter.mc"

BEGIN_PAFCORE

Parameter::Parameter(const char* name, Type* type, TypeCompound typeCompound, bool byRef, bool allowNull)
: Metadata(name)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_byRef = byRef;
	m_allowNull = allowNull;
}

END_PAFCORE
