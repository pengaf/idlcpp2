#include "static_property.h"
#include "../pafcore/static_property.h"
#include "../pafcore/reflection.h"
#include "common_func.h"

BEGIN_PAFLUA

const char* g_collectionStaticPropertyMetatableName = "idlcpp.CollectionStaticProperty";

struct StaticPropertyInstance
{
	pafcore::StaticProperty* property;
};

pafcore::ErrorCode MakeCollectionStaticProperty(lua_State* L, pafcore::StaticProperty* property)
{
	PAF_ASSERT(!property->isScalar());
	void* p = lua_newuserdata(L, sizeof(StaticPropertyInstance));
	((StaticPropertyInstance*)p)->property = property;
	luaL_getmetatable(L, g_collectionStaticPropertyMetatableName);
	lua_setmetatable(L, -2);
	return pafcore::ErrorCode::s_ok;
}

pafcore::ErrorCode ScalarStaticPropertyGet(lua_State* L, pafcore::StaticProperty* property)
{
	PAF_ASSERT(property->isScalar());
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarStaticPropertyGet(property, value);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ScalarStaticPropertySet(lua_State* L, pafcore::StaticProperty* property)
{
	PAF_ASSERT(property->isScalar());
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarStaticPropertySet(property, *value);
	return errorCode;
}


pafcore::ErrorCode CollectionStaticPropertyGet(lua_State* L, pafcore::StaticProperty* property, pafcore::Variant* iterator)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionStaticPropertyGet(property, value, *iterator);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode CollectionStaticPropertyGetByIndex(lua_State* L, pafcore::StaticProperty* property, uint32_t index)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionStaticPropertyGetByIndex(property, value, index);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode CollectionStaticPropertySet(lua_State* L, pafcore::StaticProperty* property, pafcore::Variant* iterator)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionStaticPropertySet(property, *iterator, *value);
	return errorCode;
}

pafcore::ErrorCode CollectionStaticPropertySetByIndex(lua_State* L, pafcore::StaticProperty* property, uint32_t index)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionStaticPropertySetByIndex(property, index, *value);
	return errorCode;
}

pafcore::ErrorCode ArrayStaticPropertySize(lua_State* L, pafcore::StaticProperty* property)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayStaticPropertySize(property, value);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ArrayStaticPropertyResize(lua_State* L, pafcore::StaticProperty* property)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayStaticPropertyResize(property, *value);
	return errorCode;
}


int CollectionStaticProperty_Index(lua_State* L)
{
	const char* error_name = "__index for collectionStaticProperty";
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_iterator == sckind)
	{
		errorCode = CollectionStaticPropertyGet(L, instance->property, subscript.iterator);
	}
	else if (sc_integer == sckind)
	{
		errorCode = CollectionStaticPropertyGetByIndex(L, instance->property, subscript.num);
	}
	else if (sc_string == sckind)
	{
		if (strcmp("_count_", subscript.str) == 0)
		{
			errorCode = ArrayStaticPropertySize(L, instance->property);
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

int CollectionStaticProperty_NewIndex(lua_State* L)
{
	const char* error_name = "__newindex for collectionStaticProperty";
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_iterator == sckind)
	{
		errorCode = CollectionStaticPropertySet(L, instance->property, subscript.iterator);
	}
	else if (sc_integer == sckind)
	{
		errorCode = CollectionStaticPropertySetByIndex(L, instance->property, subscript.num);
	}
	else if (sc_string == sckind)
	{
		if (strcmp("_count_", subscript.str) == 0)
		{
			errorCode = ArrayStaticPropertyResize(L, instance->property);
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

int CollectionStaticProperty_Len(lua_State* L)
{
	const char* error_name = "# for collectionStaticProperty";
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	pafcore::ErrorCode errorCode = ArrayStaticPropertySize(L, instance->property);
	if (pafcore::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, error_name, errorCode);
		return 0;
	}
	return 1;
}

struct luaL_Reg g_collectionStaticPropertyReg[] =
{
	{ "__index", CollectionStaticProperty_Index },
	{ "__newindex", CollectionStaticProperty_NewIndex },
	{ "__len", CollectionStaticProperty_Len },
	{ NULL, NULL }
};

void RegisterCollectionStaticPropertyMetatable(lua_State* L)
{
	luaL_newmetatable(L, g_collectionStaticPropertyMetatableName);
	luaL_setfuncs(L, g_collectionStaticPropertyReg, 0);
}

END_PAFLUA