#include "instance_field.h"
#include "../pafcore/reflection.h"
#include "../pafcore/instance_field.h"
#include "common_func.h"

BEGIN_PAFLUA

const char* g_arrayInstanceFieldMetatableName = "idlcpp.ArrayInstanceField";

struct InstanceFieldInstance
{
	pafcore::InstanceField* field;
	pafcore::Variant* object;
};

pafcore::ErrorCode MakeArrayInstanceField(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field)
{
	PAF_ASSERT(field->isArray());
	void* p = lua_newuserdata(L, sizeof(InstanceFieldInstance));
	((InstanceFieldInstance*)p)->field = field;
	((InstanceFieldInstance*)p)->object = that;
	luaL_getmetatable(L, g_arrayInstanceFieldMetatableName);
	lua_setmetatable(L, -2);
	return pafcore::ErrorCode::s_ok;
}

pafcore::ErrorCode ScalarInstanceFieldGetRef(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarInstanceFieldGetRef(field, value, *that);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ScalarInstanceFieldSet(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarInstanceFieldSet(field, *that, *value);
	return errorCode;
}

pafcore::ErrorCode ArrayInstanceFieldGetRef(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field, uint32_t index)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayInstanceFieldGetRef(field, value, *that, index);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ArrayInstanceFieldSet(lua_State* L, pafcore::Variant* that, pafcore::InstanceField* field, uint32_t index)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayInstanceFieldSet(field, *that, index, *value);
	return errorCode;
}

int ArrayInstanceField_Index(lua_State* L)
{
	const char* error_name = "__index for arrayInstanceField";
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_integer == sckind)
	{
		errorCode = ArrayInstanceFieldGetRef(L, instance->object, instance->field, subscript.num);
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


int ArrayInstanceField_NewIndex(lua_State* L)
{
	const char* error_name = "__newindex for arrayInstanceField";
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_integer == sckind)
	{
		errorCode = ArrayInstanceFieldSet(L, instance->object, instance->field, subscript.num);
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

int ArrayInstanceField_Len(lua_State* L)
{
	const char* error_name = "# for arrayInstanceField";
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	lua_pushinteger(L, instance->field->arraySize());
	return 1;
}

struct luaL_Reg g_arrayInstanceFieldReg[] =
{
	{ "__index", ArrayInstanceField_Index},
	{ "__newindex", ArrayInstanceField_NewIndex},
	{ "__len", ArrayInstanceField_Len },
	{ NULL, NULL }
};

void RegisterArrayInstanceFieldMetatable(lua_State* L)
{
	luaL_newmetatable(L, paflua::g_arrayInstanceFieldMetatableName);
	luaL_setfuncs(L, paflua::g_arrayInstanceFieldReg, 0);
}

END_PAFLUA