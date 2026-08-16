#pragma once
#include "utility.h"
#include "lua.hpp"
#include "../pafcore/variant.h"

BEGIN_PAFLUA

void RegisterArrayInstanceFieldMetatable(lua_State* L);
pafcore::ErrorCode MakeArrayInstanceField(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field);
pafcore::ErrorCode ScalarInstanceFieldGetRef(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field);
pafcore::ErrorCode ScalarInstanceFieldSet(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field);

END_PAFLUA