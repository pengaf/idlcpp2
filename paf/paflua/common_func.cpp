#include "common_func.h"
#include "../pafcore/primitive_type.h"
#include "../pafcore/iterator.mh"

BEGIN_PAFLUA

extern const char* g_variantMetatableName;

static void DebugTrace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int iBuf;
	char szBuffer[512];
	iBuf = vsnprintf(szBuffer, sizeof(szBuffer), format, args);
	OutputDebugStringA(szBuffer);
	va_end(args);
}

void StackDump(lua_State* L)
{
	int top = lua_gettop(L);
	DebugTrace("begin stack dump %d\n", top);
	for (int i = 1; i <= top; i++)
	{
		int t = lua_type(L, i);
		switch (t)
		{
		case LUA_TSTRING:
			DebugTrace("string: %s", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			DebugTrace(lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			if (lua_isinteger(L, i))
			{
				DebugTrace("integer: %lld", lua_tointeger(L, i));
			}
			else
			{
				DebugTrace("number: %g", lua_tonumber(L, i));
			}
			break;
		case LUA_TUSERDATA:
			DebugTrace("userdata: %s", lua_tostring(L, i));
			break;
		default:
			DebugTrace("other: %s", lua_typename(L, t));
			break;
		}
		DebugTrace("\n");
	}
	DebugTrace("end stack dump %d\n", top);
}

void Variant_Error(lua_State* L, const char* name, pafcore::ErrorCode errorCode)
{
	luaL_error(L, "idlcpp error: %s, Error: %d, %s\n", name, errorCode, pafcore::ErrorCodeToString(errorCode));
}

pafcore::Variant* LuaToVariant(pafcore::Variant* value, lua_State* L, int index)
{
	pafcore::Variant* res = value;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TNIL:
		value->setNull();
		break;

	case LUA_TSTRING: {
		size_t len;
		const char* s = lua_tolstring(L, index, &len);
		string_t str(s);
		value->assignPrimitive(str);
		break; }

	case LUA_TBOOLEAN: {
		bool b = lua_toboolean(L, index) != 0;
		value->assignPrimitive(b);
		break; }

	case LUA_TNUMBER:
		if (lua_isinteger(L, index))
		{
			lua_Integer i = lua_tointeger(L, index);
			value->assignPrimitive(i);
		}
		else
		{
			lua_Number n = lua_tonumber(L, index);
			value->assignPrimitive(n);
		}
		break;

	case LUA_TUSERDATA: {
		pafcore::Variant* variant = (pafcore::Variant*)luaL_checkudata(L, index, g_variantMetatableName);
		if (variant)
		{
			res = variant;
		}
		break;}

	default:
		break;
	}
	return res;
}

void VariantToLua(lua_State* L, pafcore::Variant* variant)
{
	if (variant->isNull())
	{
		lua_pushnil(L);
		return;
	}

	if (variant->type()->isPrimitive() && 
		(variant->typeCompound() == pafcore::TypeCompound::none || variant->typeCompound() == pafcore::TypeCompound::ref))
	{
		pafcore::PrimitiveType* primitiveType = static_cast<pafcore::PrimitiveType*>(variant->type());
		switch (primitiveType->primitiveKind())
		{
		case pafcore::string_type: {
			string_t value;
			if (primitiveType->castTo(&value, RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), variant->pointer()))
			{
				const char* str = value.c_str();
				lua_pushstring(L, str);
				return;
			}
			break; }

		case pafcore::float_type:
		case pafcore::double_type: {
			lua_Number value;
			if (primitiveType->castTo(&value, RuntimeTypeOf<lua_Number>::RuntimeType::GetSingleton(), variant->pointer()))
			{
				lua_pushnumber(L, value);
				return;
			}
			break; }

		case pafcore::bool_type: {
			bool value;
			if (primitiveType->castTo(&value, RuntimeTypeOf<bool>::RuntimeType::GetSingleton(), variant->pointer()))
			{
				lua_pushboolean(L, value ? 1 : 0);
				return;
			}
			break; }

		default: {
			lua_Integer value;
			if (primitiveType->castTo(&value, RuntimeTypeOf<lua_Integer>::RuntimeType::GetSingleton(), variant->pointer()))
			{
				lua_pushinteger(L, value);
				return;
			}
			break;}

		}
		lua_pushnil(L);
		return;
	}
	else
	{
		void* p = lua_newuserdata(L, sizeof(pafcore::Variant));
		pafcore::Variant* res = new(p)pafcore::Variant(std::move(*variant));
		luaL_getmetatable(L, g_variantMetatableName);
		lua_setmetatable(L, -2);
	}
}

SubscriptKind Variant_ParseSubscript(Subscript& subscript, lua_State* L)
{
	int index = 2;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TSTRING: {
		subscript.str = lua_tostring(L, index);
		return sc_string;
		break; }

	case LUA_TBOOLEAN: {
		subscript.num = lua_toboolean(L, index) ? 1 : 0;
		return sc_integer;
		break; }

	case LUA_TNUMBER: {
		subscript.num = lua_tointeger(L, index);
		return sc_integer;
		break; }

	case LUA_TUSERDATA: {
		pafcore::Variant* variant = (pafcore::Variant*)luaL_checkudata(L, index, g_variantMetatableName);
		if (variant)
		{
			if (variant->isNull())
			{
				return sc_error;
			}
			else if (variant->type()->isPrimitive())
			{
				pafcore::PrimitiveType* primitiveType = static_cast<pafcore::PrimitiveType*>(variant->type());
				if (pafcore::string_type == primitiveType->primitiveKind())
				{
					subscript.str = ((::string_t*)variant->pointer())->c_str();
					return sc_string;
				}
				else if (variant->castToPrimitive(RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), &subscript.num))
				{
					return sc_integer;
				}
			}
			else if (variant->type()->isEnum())
			{
				if (variant->castToPrimitive(RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), &subscript.num))
				{
					return sc_integer;
				}
			}
			else if (variant->type()->isClass())
			{
				if (static_cast<pafcore::ClassType*>(variant->type())->isType(RuntimeTypeOf<pafcore::Iterator>::RuntimeType::GetSingleton()))
				{
					subscript.iterator = variant;
					return sc_iterator;
				}
			}
		}
		break; }

	default:
		break;
	}
	return sc_error;
}

END_PAFLUA