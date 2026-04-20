#pragma once

#include "../pafcore/utility.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(_WIN32)
#if defined(PAFLUA_EXPORTS)
	#define PAFLUA_EXPORT __declspec(dllexport)
#else
	#define PAFLUA_EXPORT __declspec(dllimport)
#endif
#else
#if defined(PAFLUA_EXPORTS)
	#define PAFLUA_EXPORT __attribute__((visibility("default")))
#else
	#define PAFLUA_EXPORT
#endif
#endif

#define BEGIN_PAFLUA namespace paflua {
#define END_PAFLUA }

#define BEGIN_PAFLUA2 namespace paflua2 {
#define END_PAFLUA2 }
