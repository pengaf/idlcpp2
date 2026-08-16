#pragma once

#include "utility.h"
#include "lua.hpp"

BEGIN_PAFLUA

void RegisterVariantMetatable(lua_State* L);
void InitGlobalNameSpaceVariant(lua_State* L);

END_PAFLUA


