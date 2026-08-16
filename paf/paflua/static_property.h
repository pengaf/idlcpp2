#pragma once
#include "utility.h"
#include "lua.hpp"
#include "../pafcore/variant.h"

BEGIN_PAFLUA

void RegisterCollectionStaticPropertyMetatable(lua_State* L);
pafcore::ErrorCode MakeCollectionStaticProperty(lua_State* L, pafcore::StaticProperty* property);
pafcore::ErrorCode ScalarStaticPropertyGet(lua_State* L, pafcore::StaticProperty* property);
pafcore::ErrorCode ScalarStaticPropertySet(lua_State* L, pafcore::StaticProperty* property);

END_PAFLUA