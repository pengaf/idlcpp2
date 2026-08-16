#pragma once
#include "utility.h"
#include "lua.hpp"
#include "../pafcore/variant.h"

BEGIN_PAFLUA

void RegisterArrayStaticFieldMetatable(lua_State* L);
pafcore::ErrorCode MakeArrayStaticField(lua_State* L, pafcore::StaticField* field);
pafcore::ErrorCode ScalarStaticFieldGetRef(lua_State* L, pafcore::StaticField* field);
pafcore::ErrorCode ScalarStaticFieldSet(lua_State* L, pafcore::StaticField* field);

END_PAFLUA