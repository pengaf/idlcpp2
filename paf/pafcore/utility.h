#pragma once

#include <cstdarg>
#include <cstdio>
#include <cwchar>

#if defined(_MSC_VER)
#pragma warning(disable:4251)
#pragma warning(error:4150)
#pragma warning(error:4390)
#pragma warning(error:4715)
#define PAFCORE_CDECL __cdecl
#else
#define PAFCORE_CDECL
#endif

#if defined(_WIN32)
#if defined(PAFCORE_EXPORTS)
	#define PAFCORE_EXPORT __declspec(dllexport)
	#define PAFCORE_TEMPLATE __declspec(dllexport)
#else
	#define PAFCORE_EXPORT __declspec(dllimport)
	#define PAFCORE_TEMPLATE
#endif
#else
#if defined(PAFCORE_EXPORTS)
	#define PAFCORE_EXPORT __attribute__((visibility("default")))
#else
	#define PAFCORE_EXPORT
#endif
	#define PAFCORE_TEMPLATE
#endif

#if !defined(_MSC_VER)
template<size_t N>
inline int sprintf_s(char (&buffer)[N], const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, N, format, args);
	va_end(args);
	return result;
}

inline void OutputDebugStringA(const char* str)
{
	if (0 != str)
	{
		fputs(str, stderr);
	}
}
#endif

#define BEGIN_PAFCORE namespace pafcore {
#define END_PAFCORE }

#define PAF_CONCAT_(a, b) a ## b
#define PAF_CONCAT(a, b)  PAF_CONCAT_(a, b)

#define PAF_WIDE_(s) L ## s
#define PAF_WIDE(s) PAF_WIDE_(s)

#ifdef _DEBUG
PAFCORE_EXPORT void PafAssert(wchar_t const* _Message, wchar_t const* _File, unsigned _Line, wchar_t const* format, ...);

#if defined(_MSC_VER)
#define PAF_ASSERT_MSG(expression, format, ...) (void)(			\
		(!!(expression)) ||							\
		(PafAssert(PAF_WIDE(#expression), PAF_WIDE(__FILE__), (unsigned)(__LINE__), PAF_WIDE(format), __VA_ARGS__), 0) \
	)
#else
#define PAF_ASSERT_MSG(expression, format, ...) (void)(			\
		(!!(expression)) ||							\
		(PafAssert(PAF_WIDE(#expression), PAF_WIDE(__FILE__), (unsigned)(__LINE__), PAF_WIDE(format), ##__VA_ARGS__), 0) \
	)
#endif

#else

#define PAF_ASSERT_MSG(expression, format, ...) ((void)0)

#endif

#define PAF_ASSERT_MSG0(expression, msg)  PAF_ASSERT_MSG(expression, "%s", msg)
#define PAF_ASSERT(expression)  PAF_ASSERT_MSG(expression, "")

#define paf_array_size_of(a)	(sizeof(a)/sizeof(a[0]))
#define paf_field_size_of(s, m)	sizeof(((s*)0)->m)
#define paf_field_array_size_of(s, m)	(sizeof(((s*)0)->m)/sizeof(((s*)0)->m[0]))
#define paf_field_offset_of(s, m) (reinterpret_cast<size_t>(&(reinterpret_cast<s*>(1)->m)) - 1)
#define paf_base_offset_of(d, b) (reinterpret_cast<ptrdiff_t>(static_cast<b*>(reinterpret_cast<d*>(1))) - 1)
#define paf_verify

#include "typedef.h"

