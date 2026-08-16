#include "static_field.h"
#include "../pafcore/reflection.h"
#include "../pafcore/static_field.h"
#include "common_func.h"

BEGIN_PAFLUA

const char* g_arrayStaticFieldMetatableName = "idlcpp.ArrayStaticField";

struct StaticFieldInstance
{
	pafcore::StaticField* field;
};

pafcore::ErrorCode MakeArrayStaticField(lua_State* L, pafcore::StaticField* field)
{
	PAF_ASSERT(field->isArray());
	void* p = lua_newuserdata(L, sizeof(StaticFieldInstance));
	((StaticFieldInstance*)p)->field = field;
	luaL_getmetatable(L, g_arrayStaticFieldMetatableName);
	lua_setmetatable(L, -2);
	return pafcore::ErrorCode::s_ok;
}

pafcore::ErrorCode ScalarStaticFieldGetRef(lua_State* L, pafcore::StaticField* field)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarStaticFieldGetRef(field, value);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ScalarStaticFieldSet(lua_State* L, pafcore::StaticField* field)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarStaticFieldSet(field, *value);
	return errorCode;
	return pafcore::ErrorCode::s_ok;
}


pafcore::ErrorCode ArrayStaticFieldGetRef(lua_State* L, pafcore::StaticField* field, uint32_t index)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayStaticFieldGetRef(field, value, index);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ArrayStaticFieldSet(lua_State* L, pafcore::StaticField* field, uint32_t index)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayStaticFieldSet(field, index, *value);
	return errorCode;
}

int ArrayStaticField_Index(lua_State* L)
{
	const char* error_name = "__index for arrayStaticField";
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_integer == sckind)
	{
		errorCode = ArrayStaticFieldGetRef(L, instance->field, subscript.num);
	}
	else if (sc_string == sckind)
	{
		if (strcmp("_count_", subscript.str) == 0)
		{
			lua_pushinteger(L, instance->field->arraySize());
			errorCode = pafcore::ErrorCode::s_ok;
		}
		else
		{
			errorCode = pafcore::ErrorCode::e_member_not_found;
		}
	}
	else
	{
		errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
	}
	if (pafcore::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, error_name, errorCode);
		return 0;
	}
	return 1;
}

int ArrayStaticField_NewIndex(lua_State* L)
{
	const char* error_name = "__newindex for arrayStaticField";
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_integer == sckind)
	{
		errorCode = ArrayStaticFieldSet(L, instance->field, subscript.num);
	}
	else
	{
		errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
	}
	if (pafcore::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, error_name, errorCode);
		return 0;
	}
	return 1;
}

int ArrayStaticField_Len(lua_State* L)
{
	//const char* error_name = "# for arrayStaticField";
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	lua_pushinteger(L, instance->field->arraySize());
	return 1;
}

struct luaL_Reg g_arrayStaticPropertyReg[] =
{
	{ "__index", ArrayStaticField_Index},
	{ "__newindex", ArrayStaticField_NewIndex},
	{ "__len", ArrayStaticField_Len },
	{ NULL, NULL }
};

void RegisterArrayStaticFieldMetatable(lua_State* L)
{
	luaL_newmetatable(L, paflua::g_arrayStaticFieldMetatableName);
	luaL_setfuncs(L, paflua::g_arrayStaticPropertyReg, 0);
}

END_PAFLUA