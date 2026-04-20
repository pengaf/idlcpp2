#pragma once

#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <new>

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

#include "memory.h"

PAFCORE_EXPORT void* PAFCORE_CDECL operator new(size_t size, const char* fileName, int line, int);
PAFCORE_EXPORT void* PAFCORE_CDECL operator new[](size_t size, const char* fileName, int line, int);
PAFCORE_EXPORT void PAFCORE_CDECL operator delete(void* block, const char* fileName, int line, int);
PAFCORE_EXPORT void PAFCORE_CDECL operator delete[](void* block, const char* fileName, int line, int);

#define PAF_CONCAT_(a, b) a ## b
#define PAF_CONCAT(a, b)  PAF_CONCAT_(a, b)

#define PAF_WIDE_(s) L ## s
#define PAF_WIDE(s) PAF_WIDE_(s)

#ifdef _DEBUG
#define paf_new new(__FILE__, __LINE__, 0)


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

#define paf_new new
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
#define AUTO_NEW_ARRAY_SIZE


#ifdef AUTO_NEW_ARRAY_SIZE

template<typename T>
inline size_t paf_new_array_size_of(void* p)
{
	return pafcore::ArraySizeOf(static_cast<T*>(p));
}

template<typename T>
inline T* paf_new_array(size_t count)
{
	return pafcore::CreateArray<T>(count);
}

template<typename T>
inline void paf_delete_array(T* p)
{
	pafcore::DestroyArray(p);
}
#else

inline size_t paf_new_array_size_of(void* p)
{
	return 0 != p ? 1 : 0;
}

template<typename T>
inline T* paf_new_array(size_t count)
{
	return pafcore::CreateArray<T>(count);
}

template<typename T>
inline void paf_delete_array(T* p)
{
	pafcore::DestroyArray(p);
}

#endif

BEGIN_PAFCORE


PAFCORE_EXPORT void DummyDestroyInstance(void* address);
PAFCORE_EXPORT void DummyDestroyArray(void* address);
PAFCORE_EXPORT void DummyAssign(void* dst, const void* src);

class PAFCORE_EXPORT VirtualDestructor
{
public:
	virtual ~VirtualDestructor()
	{}
};

END_PAFCORE

template<typename T>
inline void paf_delete(T* p)
{
	pafcore::Destroy(p);
}

template<typename T>
inline void DeleteSetNull(T*& p)
{
	paf_delete(p);
	p = 0;
}

template<typename T>
inline void DeleteArraySetNull(T*& p)
{
	paf_delete_array(p);
	p = 0;
}

template<typename T>
inline void SafeAddRef(T* p)
{
	if (0 != p)
	{
		p->addRef();
	}
}

template<typename T>
inline void SafeAddRefArray(T* const * p, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (0 != p[i])
		{
			p[i]->addRef();
		}
	}
}

template<typename T>
inline void SafeRelease(T* p)
{
	if (0 != p)
	{
		p->release();
	}
}

template<typename T>
inline void SafeReleaseSetNull(T*& p)
{
	if (0 != p)
	{
		p->release();
		p = 0;
	}
}

template<typename T>
inline void SafeReleaseArray(T* const* p, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (0 != p[i])
		{
			p[i]->release();
		}
	}
}

template<typename T>
inline void SafeReleaseArraySetNull(T** p, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (0 != p[i])
		{
			p[i]->release();
			p[i] = 0;
		}
	}
}

template<typename T>
inline void CoSafeAddRef(T* p)
{
	if (0 != p)
	{
		p->AddRef();
	}
}

template<typename T>
inline void CoSafeRelease(T* p)
{
	if (0 != p)
	{
		p->Release();
	}
}

template<typename T>
inline void CoSafeReleaseSetNull(T*& p)
{
	if (0 != p)
	{
		p->Release();
		p = 0;
	}
}

template<typename T>
inline void CoSafeReleaseArray(T* const* p, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (0 != p[i])
		{
			p[i]->Release();
		}
	}
}

template<typename T>
inline void CoSafeReleaseArraySetNull(T** p, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (0 != p[i])
		{
			p[i]->Release();
			p[i] = 0;
		}
	}
}

template<typename T>
class AutoDelete
{
public:
	AutoDelete(T* p) : m_p(p)
	{}
	~AutoDelete()
	{
		paf_delete(m_p);
	}
public:
	T* m_p;
};

template<typename T>
class AutoDeleteArray
{
public:
	AutoDeleteArray(T* p) : m_p(p)
	{}
	~AutoDeleteArray()
	{
		paf_delete_array(m_p);
	}
public:
	T* m_p;
};

template<typename T>
struct AutoRelease
{
	AutoRelease(T* p)
	{
		m_p = p;
	}
	~AutoRelease()
	{
		SafeRelease(m_p);
	}
	T* m_p;
};

template<typename T>
struct CoAutoRelease
{
	CoAutoRelease(T* p)
	{
		m_p = p;
	}
	~CoAutoRelease()
	{
		CoSafeRelease(m_p);
	}
	T* m_p;
};

#include "typedef.h"

