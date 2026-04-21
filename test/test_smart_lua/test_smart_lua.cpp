#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include "lua.hpp"
#include "paflua.h"
#include "../../paf/pafcore/std_string.h"
#include "../test_smart_types/test_smart_types.h"

void GetExePath(pafcore::string& path)
{
#if defined(_WIN32)
    char fileName[MAX_PATH];
    DWORD length = GetModuleFileNameA(0, fileName, sizeof(fileName));
    if (0 == length)
    {
        path.clear();
        return;
    }
    const char* end = strrchr(fileName, '\\');
#else
    char fileName[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", fileName, sizeof(fileName) - 1);
    if (0 >= length)
    {
        path.clear();
        return;
    }
    fileName[length] = '\0';
    const char* end = strrchr(fileName, '/');
#endif
    if (0 == end)
    {
        path.clear();
        return;
    }
    path.assign(fileName, static_cast<size_t>(end - fileName + 1));
}

int RunLuaFile(const char* filePath)
{
    lua_State* luaState = luaL_newstate();
    if (0 == luaState)
    {
        return 1;
    }

    luaL_openlibs(luaState);
    luaopen_paflua(luaState);

    int error = luaL_loadfile(luaState, filePath) || lua_pcall(luaState, 0, 0, 0);
    if (error)
    {
        fprintf(stderr, "%s\n", lua_tostring(luaState, -1));
        lua_pop(luaState, 1);
        lua_close(luaState);
        return 1;
    }

    lua_close(luaState);
    return 0;
}

int main()
{
    testsmart::TestValueSmall::GetType();
    testsmart::TestValueLarge::GetType();
    testsmart::TestObject::GetType();
    testsmart::TestHolder::GetType();

    pafcore::string path;
    GetExePath(path);
    path += "test_smart_lua.lua";
    return RunLuaFile(path.c_str());
}
