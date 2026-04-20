#include "void_type.h"
#include "void_type.mh"
#include "void_type.ic"
#include "void_type.mc"
#include "name_space.h"
#include "auto_run.h"

BEGIN_PAFCORE

VoidType::VoidType() : Type("void", void_object, "")
{
	m_size = 1;
	NameSpace::GetGlobalNameSpace()->registerMember(this);
}

Metadata* VoidType::findMember(const char* name)
{
	return 0;
}

VoidType VoidType::s_instance;

END_PAFCORE

AUTO_REGISTER_TYPE(pafcore::VoidType)

