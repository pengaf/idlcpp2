#include "script_invoker.h"
#include "lua.hpp"

BEGIN_PAFLUA

extern const char* g_variantMetatableName;
void StackDump (lua_State *L);
pafcore::Variant* LuaToVariant(pafcore::Variant* value, lua_State *L, int index);
void VariantToLua(lua_State *L, pafcore::Variant* variant);

//pafcore::Variant* VariantToLua2(lua_State *L, pafcore::Variant* variant)
//{
//	void* p = lua_newuserdata(L, sizeof(pafcore::Variant));
//	pafcore::Variant* res = new(p)pafcore::Variant;
//	res->move(*variant);
//	luaL_getmetatable(L, g_variantMetatableName);
//	lua_setmetatable(L, -2);
//	return res;
//}

ScriptInvoker::ScriptInvoker(lua_State* luaState)
{
	m_luaState = luaState;
	lua_rawsetp(luaState, LUA_REGISTRYINDEX, this);
}

ScriptInvoker::~ScriptInvoker()
{
	lua_pushnil(m_luaState);
	lua_rawsetp(m_luaState, LUA_REGISTRYINDEX, this);
}

pafcore::ErrorCode ScriptInvoker::invoke(const char* name, pafcore::Variant* self, pafcore::Variant* results, uint32_t numResults, pafcore::Variant* arguments, uint32_t numArguments)
{
	int oldTop = lua_gettop(m_luaState);
	
	lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, this);

	//StackDump(m_luaState);

	lua_getfield(m_luaState, -1, name);

	//StackDump(m_luaState);

	if (!(lua_isfunction(m_luaState, -1) || lua_iscfunction(m_luaState, -1)))
	{
		lua_pop(m_luaState, 2);

		return pafcore::ErrorCode::e_script_dose_not_override;
	}

	lua_insert(m_luaState, -2);

	//StackDump(m_luaState);

	for(uint32_t i = 0; i < numArguments; ++i)
	{
		VariantToLua(m_luaState, &arguments[i]);
	}

	//StackDump(m_luaState);

	int error = lua_pcall(m_luaState, int(numArguments + 1), LUA_MULTRET, 0);

	//StackDump(m_luaState);
	
	if (LUA_OK != error)
	{
		const char* str = lua_tostring(m_luaState, -1);
#ifdef _DEBUG
		OutputDebugStringA(str);
		OutputDebugStringA("\n");
#endif
		lua_pop(m_luaState, 1);
		return pafcore::ErrorCode::e_script_dose_not_override;
	}

	int newTop = lua_gettop(m_luaState);
	int actualResults = newTop - oldTop;
	if (actualResults != (int)numResults)
	{
		lua_pop(m_luaState, actualResults);
#ifdef _DEBUG
		char errMsg[256];
		sprintf_s(errMsg, "Lua function '%s' returned %d values, but expected %d\n", name, actualResults, numResults);
		OutputDebugStringA(errMsg);
#endif
		return pafcore::ErrorCode::e_script_dose_not_override;
	}
	
	for (uint32_t i = 0; i < numResults; ++i)
	{
		int index = oldTop + 1 + i;
		pafcore::Variant* tmp = LuaToVariant(&results[i], m_luaState, index);
		if (tmp != &results[i])
		{
			results[i].move(*tmp);
		}
	}
	lua_pop(m_luaState, actualResults);
	return pafcore::ErrorCode::s_ok;
}

END_PAFLUA
