#import "typedef.i"
#{
#include "utility.h"
#}

namespace pafcore
{
	struct #PAFCORE_EXPORT System
	{
		static string_t GetProgramPath();
		static void LoadDLL(string_t fileName);
		static void OutputDebug(string_t str);
		static void DebugBreak();
		static void Sleep(ulong_t millisecond);
	};
}

