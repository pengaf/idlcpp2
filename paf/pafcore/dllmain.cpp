#include "utility.h"
#include <Windows.h>

HINSTANCE g_pafcore_instance = 0;
DWORD g_pafcore_mainThreadID = 0;
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		g_pafcore_instance = hInstance;
		g_pafcore_mainThreadID = ::GetCurrentThreadId();
		OutputDebugStringA("pafcore Process Attach\n");
	}
	else if (DLL_PROCESS_DETACH == dwReason)
	{
		OutputDebugStringA("pafcore Process Detach\n");
	}
	return TRUE;
}
