#{
#include "utility.h"

typedef bool					bool_t;
typedef char					char_t;
typedef signed char				schar_t;
typedef unsigned char			uchar_t;
typedef short					short_t;
typedef unsigned short			ushort_t;
typedef long					long_t;
typedef unsigned long			ulong_t;
typedef long long				longlong_t;
typedef unsigned long long		ulonglong_t;
typedef int						int_t;
typedef unsigned int			uint_t;
typedef float					float_t;
typedef double					double_t;
typedef long double				longdouble_t;

typedef unsigned char			byte_t;

#if defined(_MSC_VER)
typedef signed char				int8_t;
typedef unsigned char			uint8_t;
typedef short					int16_t;
typedef unsigned short			uint16_t;
typedef int						int32_t;
typedef unsigned int			uint32_t;
typedef long long				int64_t;
typedef unsigned long long		uint64_t;

#if defined(_WIN64)
typedef unsigned __int64		size_t;
typedef __int64					ptrdiff_t;
#else
typedef unsigned int			size_t;
typedef int						ptrdiff_t;
#endif
#else
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#endif

#if defined(_MSC_VER)
extern "C" int strcmp(const char* lhs, const char* rhs);
extern "C" size_t strlen(const char* str);
#endif

//????????????????????????????????????
class string_t
{
public:
	string_t() : m_str("")
	{}
	string_t(const char* str) : m_str(str ? str : "")
	{}
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

#}

typename bool_t;
typename char_t;
typename schar_t;
typename uchar_t;
typename short_t;
typename ushort_t;
typename long_t;
typename ulong_t;
typename longlong_t;
typename ulonglong_t;
typename int_t;
typename uint_t;
typename float_t;
typename double_t;
typename longdouble_t;
typename byte_t;
typename int8_t;
typename uint8_t;
typename int16_t;
typename uint16_t;
typename int32_t;
typename uint32_t;
typename int64_t;
typename uint64_t;
typename size_t;
typename ptrdiff_t;

struct Nil
{
};

namespace pafcore
{
	//????????????????????????????????????
	//?????��????????(xml cdata)
	class(value_object) #PAFCORE_EXPORT text_t
	{
		nocode text_t();
#{
	public:
		text_t() : m_str("")
		{}
		text_t(const char* str) : m_str(str ? str : "")
		{}
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
#}
	};

	//????????????????????????????????????
	class(value_object) #PAFCORE_EXPORT buffer_t
	{
		nocode buffer_t();
#{
	public:
		buffer_t() : buffer_t(0, 0)
		{}
		buffer_t(void* pointer, size_t size) : m_pointer(pointer), m_size(size)
		{}
	public:
		void* getData() const
		{
			return m_pointer;
		}
		size_t getLength() const
		{
			return m_size;
		}
		void assign(void* pointer, size_t size)
		{
			m_pointer = pointer;
			m_size = size;
		}
	protected:
		void* m_pointer;
		size_t m_size;
#}
	};

	//?????????��????????��????????????????????,?????????????????????? get ?????????
	//???????????,get??????????? Status::ready ????
	//????? set ???????????��??
	struct #PAFCORE_EXPORT MethodAsProperty
	{
		//????????????????????????��?????? set_
#{
		enum Status : uint8_t
		{
			ready,
			running,
			running_allow_cancel,
		};
		Status status;
		float progress;
		MethodAsProperty() : status(ready), progress(0)
		{}
#}
	};

}