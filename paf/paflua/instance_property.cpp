#include "instance_property.h"
#include "../pafcore/reflection.h"
#include "../pafcore/instance_property.h"
#include "common_func.h"

BEGIN_PAFLUA

const char* g_collectionInstancePropertyMetatableName = "idlcpp.CollectionInstanceProperty";

struct InstancePropertyInstance
{
	pafcore::InstanceProperty* property;
	pafcore::Variant* object;
};

pafcore::ErrorCode MakeCollectionInstanceProperty(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property)
{
	PAF_ASSERT(!property->isScalar());
	void* p = lua_newuserdata(L, sizeof(InstancePropertyInstance));
	((InstancePropertyInstance*)p)->property = property;
	((InstancePropertyInstance*)p)->object = that;
	luaL_getmetatable(L, g_collectionInstancePropertyMetatableName);
	lua_setmetatable(L, -2);
	return pafcore::ErrorCode::s_ok;
}

pafcore::ErrorCode ScalarInstancePropertyGet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property)
{
	//PAF_ASSERT(property->isScalar());
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarInstancePropertyGet(property, value, *that);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ScalarInstancePropertySet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property)
{
	//PAF_ASSERT(property->isScalar());
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ScalarInstancePropertySet(property, *that, *value);
	return errorCode;
}

pafcore::ErrorCode CollectionInstancePropertyGet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property, pafcore::Variant* iterator)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionInstancePropertyGet(property, value, *that, *iterator);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode CollectionInstancePropertyGetByIndex(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property, uint32_t index)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionInstancePropertyGetByIndex(property, value, *that, index);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode CollectionInstancePropertySet(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property, pafcore::Variant* iterator)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionInstancePropertySet(property, *that, *iterator, *value);
	return errorCode;
}

pafcore::ErrorCode CollectionInstancePropertySetByIndex(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property, uint32_t index)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::CollectionInstancePropertySetByIndex(property, *that, index, *value);
	return errorCode;
}

pafcore::ErrorCode ArrayInstancePropertySize(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property)
{
	pafcore::Variant value;
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayInstancePropertySize(property, value, *that);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

pafcore::ErrorCode ArrayInstancePropertyResize(lua_State* L, pafcore::Variant* that, pafcore::InstanceProperty* property)
{
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	pafcore::ErrorCode errorCode = pafcore::Reflection::ArrayInstancePropertyResize(property, *that, *value);
	return errorCode;
}

int CollectionInstanceProperty_Index(lua_State* L)
{
	const char* error_name = "__index for collectionInstanceProperty";
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_iterator == sckind)
	{
		errorCode = CollectionInstancePropertyGet(L, instance->object, instance->property, subscript.iterator);
	}
	else if (sc_integer == sckind)
	{
		errorCode = CollectionInstancePropertyGetByIndex(L, instance->object, instance->property, subscript.num);
	}
	else if (sc_string == sckind)
	{
		if (strcmp("_count_", subscript.str) == 0)
		{
			errorCode = ArrayInstancePropertySize(L, instance->object, instance->property);
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

int CollectionInstanceProperty_NewIndex(lua_State* L)
{
	const char* error_name = "__newindex for collectionInstanceProperty";
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	pafcore::ErrorCode errorCode;
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);
	if (sc_iterator == sckind)
	{
		errorCode = CollectionInstancePropertySet(L, instance->object, instance->property, subscript.iterator);
	}
	else if (sc_integer == sckind)
	{
		errorCode = CollectionInstancePropertySetByIndex(L, instance->object, instance->property, subscript.num);
	}
	else if (sc_string == sckind)
	{
		if (strcmp("_count_", subscript.str) == 0)
		{
			errorCode = ArrayInstancePropertyResize(L, instance->object, instance->property);
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

int CollectionInstanceProperty_Len(lua_State* L)
{
	const char* error_name = "# for collectionInstanceProperty";
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, error_name, pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	pafcore::ErrorCode errorCode = ArrayInstancePropertySize(L, instance->object, instance->property);
	if (pafcore::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, error_name, errorCode);
		return 0;
	}
	return 1;
}

struct luaL_Reg g_collectionInstancePropertyReg[] =
{
	{ "__index", CollectionInstanceProperty_Index },
	{ "__newindex", CollectionInstanceProperty_NewIndex },
	{ "__len", CollectionInstanceProperty_Len },
	{ NULL, NULL }
};

void RegisterCollectionInstancePropertyMetatable(lua_State* L)
{
	luaL_newmetatable(L, g_collectionInstancePropertyMetatableName);
	luaL_setfuncs(L, g_collectionInstancePropertyReg, 0);
}

END_PAFLUA