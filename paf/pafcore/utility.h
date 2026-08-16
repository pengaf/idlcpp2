#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <type_traits>

using bool_t		= bool;
using char_t		= char;
using schar_t		= signed char;
using uchar_t		= unsigned char;
using short_t		= short;
using ushort_t		= unsigned short;
using long_t		= long;
using ulong_t		= unsigned long;
using longlong_t	= long long;
using ulonglong_t	= unsigned long long;
using int_t			= int;
using uint_t		= unsigned int;
using float_t		= float;
using double_t		= double;
using longdouble_t	= long double;
using byte_t		= unsigned char;


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

#if defined(_MSC_VER)
extern "C" int strcmp(const char* lhs, const char* rhs);
extern "C" size_t strlen(const char* str);
#endif

class string_t
{
public:
	string_t() : m_str("")
	{
	}
	string_t(const char* str) : m_str(str ? str : "")
	{
	}
	//string_t(const string& str) : m_str(str.m_str)
	//{}
public:
	const char* c_str() const
	{
		return m_str;
	}
	operator const char* () const
	{
		return m_str;
	}
	bool empty() const
	{
		return (0 == *m_str);
	}
	void assign(const char* str)
	{
		m_str = str ? str : "";
	}
	bool operator == (const char* str) const
	{
		return 0 == strcmp(m_str, str);
	}
	bool operator != (const char* str) const
	{
		return 0 != strcmp(m_str, str);
	}
	size_t length() const
	{
		return strlen(m_str);
	}
protected:
	const char* m_str;
};

template<typename T>
struct RuntimeTypeOf
{};


#define BEGIN_PAFCORE namespace pafcore {
#define END_PAFCORE }

namespace pafcore
{
	class Interface;
	class Object;
	class STRCObject;
	class MTRCObject;


	template<typename Base, typename Derived>
	struct is_base_or_same
	{
		static constexpr bool value =
			std::is_base_of<Base, Derived>::value ||
			std::is_same<Base, Derived>::value;
	};

	template<typename T>
	struct is_introspectable : is_base_or_same<Interface, T> {};

	template<typename T>
	inline constexpr bool is_introspectable_v = is_introspectable<T>::value;

	template<typename T>
	struct is_interface : is_base_or_same<Interface, T> {};

	template<typename T>
	inline constexpr bool is_interface_v = is_interface<T>::value;

	template<typename T>
	struct is_object : is_base_or_same<Object, T> {};

	template<typename T>
	inline constexpr bool is_object_v = is_object<T>::value;

	//template<typename T>
	//struct is_rc_object : is_base_or_same<RCObject, T> {};

	//template<typename T>
	//inline constexpr bool is_rc_object_v = is_rc_object<T>::value;

	template<typename T>
	struct is_strc_object : is_base_or_same<STRCObject, T> {};

	template<typename T>
	inline constexpr bool is_strc_object_v = is_strc_object<T>::value;

	template<typename T>
	struct is_mtrc_object : is_base_or_same<MTRCObject, T> {};

	template<typename T>
	inline constexpr bool is_mtrc_object_v = is_mtrc_object<T>::value;

	enum class RefCountPolicy : signed char
	{
		unknown,
		single_thread,
		multi_thread,
	};

	template<typename T>
	inline RefCountPolicy ClassRefCountPolicy()
	{
		if constexpr (is_strc_object_v<T>)
		{
			return RefCountPolicy::single_thread;
		}
		else if constexpr (is_mtrc_object_v<T>)
		{
			return RefCountPolicy::multi_thread;
		}
		else if constexpr (is_interface_v<T>)
		{
			return RefCountPolicy::unknown;
		}
		else
		{
			return RefCountPolicy::single_thread;
		}
	}

}

//#include "typedef.h"
