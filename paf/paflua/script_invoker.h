#pragma once

#include "utility.h"
#include "../pafcore/script_invoker.h"
#include "lua.hpp"

BEGIN_PAFLUA

struct ScriptInvoker : public pafcore::ScriptInvoker
{
public:
	ScriptInvoker(lua_State* luaState);
	~ScriptInvoker();
public:
	virtual pafcore::ErrorCode invoke(const char* name, pafcore::Variant* self, pafcore::Variant* results, uint32_t numResults, pafcore::Variant* arguments, uint32_t numArguments) override;
public:
	lua_State* m_luaState;
};

END_PAFLUA
