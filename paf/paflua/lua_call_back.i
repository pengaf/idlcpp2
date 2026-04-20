#import "../pafcore/delegate.i"

#{
#include "../pafcore/std_string.h"
#include "utility.h"
#include "lua.hpp"
#}

namespace paflua
{
	class #PAFLUA_EXPORT LuaCallBack : CallBack
	{
#{
	public:
		LuaCallBack(lua_State* luaState, const char* funcName);
		~LuaCallBack();
	public:
		virtual void invoke(pafcore::Variant* result, pafcore::Variant** args, int_t numArgs);
		virtual bool equal(pafcore::CallBack* arg);
	public:
		lua_State* m_luaState;
		pafcore::string m_funcName;
#}
	};

	class #PAFLUA_EXPORT LuaCallBack2 : CallBack
	{
#{
	public:
		LuaCallBack2(lua_State* luaState);
		~LuaCallBack2();
	public:
		virtual void invoke(pafcore::Variant* result, pafcore::Variant** args, int_t numArgs);
		virtual bool equal(pafcore::CallBack* arg);
	public:
		lua_State* m_luaState;
#}
	};

}
