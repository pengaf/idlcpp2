#pragma once
#include "utility.h"
#include "lua.hpp"
#include "../pafcore/variant.h"

BEGIN_PAFLUA

enum SubscriptKind
{
	sc_error,
	sc_integer,
	sc_string,
	sc_iterator,
};

struct Subscript
{
	union
	{
		size_t num;
		const char* str;
		pafcore::Variant* iterator;
	};
};

void Variant_Error(lua_State* L, const char* name, pafcore::ErrorCode errorCode);
pafcore::Variant* LuaToVariant(pafcore::Variant* value, lua_State* L, int index);
void VariantToLua(lua_State* L, pafcore::Variant* variant);

SubscriptKind Variant_ParseSubscript(Subscript& subscript, lua_State* L);

END_PAFLUA
