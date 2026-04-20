#include "system.h"
#include "system.ic"
#include "system.mh"
#include "system.mc"
#include "std_string.h"

#include <chrono>
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <unistd.h>
#endif

namespace pafcore
{
	string_t System::GetProgramPath()
	{
		static pafcore::string s_programPath;
		if (s_programPath.empty())
		{
#if defined(_WIN32)
			DWORD size = MAX_PATH;
			pafcore::string buffer(size, '\0');
			DWORD length = GetModuleFileNameA(0, buffer.data(), size);
			while (length == size)
			{
				size *= 2;
				buffer.assign(size, '\0');
				length = GetModuleFileNameA(0, buffer.data(), size);
			}
			buffer.resize(length);
			s_programPath = buffer;
#elif defined(__linux__)
			pafcore::string buffer(4096, '\0');
			ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
			if (length > 0)
			{
				buffer.resize(static_cast<size_t>(length));
				s_programPath = buffer;
			}
			else
			{
				s_programPath = std::filesystem::current_path().generic_u8string();
			}
#else
			s_programPath = std::filesystem::current_path().generic_u8string();
#endif
		}
		return s_programPath.c_str();
	}

	void System::LoadDLL(string_t fileName)
	{
#if defined(_WIN32)
		LoadLibraryA(fileName);
#else
		dlopen(fileName, RTLD_NOW | RTLD_LOCAL);
#endif
	}

	void System::OutputDebug(string_t str)
	{
		OutputDebugStringA(str);
	}

	void System::DebugBreak()
	{
#if defined(_WIN32)
		::DebugBreak();
#elif defined(__GNUC__) || defined(__clang__)
		__builtin_trap();
#else
		std::abort();
#endif
	}

	void System::Sleep(ulong_t millisecond)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
	}
}
