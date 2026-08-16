#include "paflua.h"
#include "lua.hpp"
#include "variant.h"
#include "instance_field.h"
#include "static_field.h"
#include "instance_property.h"
#include "static_property.h"
#include "../pafcore/variant.h"
#include "../pafcore/name_space.h"
#include "../pafcore/name_space.mh"
#include <new>

#if defined(_WIN32)
#include <Windows.h>
#endif

void init_paf(lua_State *L)
{
	paflua::RegisterArrayInstanceFieldMetatable(L);
	paflua::RegisterArrayStaticFieldMetatable(L);
	paflua::RegisterCollectionInstancePropertyMetatable(L);
	paflua::RegisterCollectionStaticPropertyMetatable(L);
	paflua::RegisterVariantMetatable(L);
	paflua::InitGlobalNameSpaceVariant(L);
}

int luaopen_paflua(lua_State *L)
{
	init_paf(L);
	return 1;
}

#if defined(_WIN32)
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		OutputDebugStringA("paflua Process Attach\n");
	}
	else if (DLL_PROCESS_DETACH == dwReason)
	{
		OutputDebugStringA("paflua Process Detach\n");
	}
	return TRUE;
}
#endif

