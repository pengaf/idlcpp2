#pragma once
#include "utility.h"
#include "lua.hpp"
#include "../pafcore/variant.h"

BEGIN_PAFLUA

void RegisterCollectionInstancePropertyMetatable(lua_State* L);

pafcore::ErrorCode MakeCollectionInstanceProperty(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property);
pafcore::ErrorCode ScalarInstancePropertyGet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property);
pafcore::ErrorCode ScalarInstancePropertySet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property);

END_PAFLUA
