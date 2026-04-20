#import "typedef.i"

#{
#include "utility.h"
#}

namespace pafcore
{
	//idlcpp ??????��????? toString__ ?? fromString__ ????????????????????????????��????????��????????????????????????????????????? field ?? property

	class(value_object) #PAFCORE_EXPORT String
	{
		String();
		string_t toString__() const;
		void fromString__(string_t str);
#{
	public:
		explicit String(const char* str);
		String(const String& arg);
		String(String&& arg);
		~String();
	public:
		bool empty() const;
		const char* c_str() const;
		size_t length() const;
		int compare(string_t str) const;
		int compare(const String& arg) const;
		void assign(string_t str);
		void assign(const String& arg);
		void append(string_t str);
		void append(const String& arg);
	public:
		String& operator=(String&& arg);
		String& operator=(const char* str);
		String& operator=(const String& arg);
		String& operator+=(const char* str);
		String& operator+=(const String& arg);
		bool operator == (const String& arg) const;
		bool operator != (const String& arg) const;
		bool operator < (const String& arg) const;
		bool operator == (const char* str) const;
		bool operator != (const char* str) const;
	protected:
		char* m_str;
	protected:
		static char* s_emptyString;
#}
	};

#{
	inline bool String::empty() const
	{
		return 0 == *m_str;
	}

	inline const char* String::c_str() const
	{
		return m_str;
	}

	inline void String::assign(const String& arg)
	{
		assign(arg.m_str);
	}

	inline void String::append(const String& arg)
	{
		append(arg.m_str);
	}

	inline String& String::operator=(const char* str)
	{
		assign(str);
		return *this;
	}

	inline String& String::operator=(const String& arg)
	{
		assign(arg);
		return *this;
	}

	inline String& String::operator=(String&& arg)
	{
		m_str = arg.m_str;
		arg.m_str = s_emptyString;
		return *this;
	}

	inline String& String::operator+=(const char* str)
	{
		append(str);
		return *this;
	}

	inline String& String::operator+=(const String& arg)
	{
		append(arg);
		return *this;
	}

	inline bool String::operator == (const String& arg) const
	{
		return strcmp(m_str, arg.m_str) == 0;
	}

	inline bool String::operator != (const String& arg) const
	{
		return strcmp(m_str, arg.m_str) != 0;
	}

	inline bool String::operator < (const String& arg) const
	{
		return strcmp(m_str, arg.m_str) < 0;
	}

	inline int String::compare(string_t str) const
	{
		return strcmp(m_str, str.c_str());
	}

	inline int String::compare(const String& arg) const
	{
		return strcmp(m_str, arg.m_str);
	}

	inline bool String::operator == (const char* str) const
	{
		return 0 == strcmp(m_str, str);
	}

	inline bool String::operator != (const char* str) const
	{
		return 0 != strcmp(m_str, str);
	}

	inline string_t String::toString__() const
	{
		return m_str;
	}

	inline void String::fromString__(string_t str)
	{
		assign(str);
	}

	inline void Hash_Combine(size_t& seed, size_t value)
	{
		seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline size_t StringToHash(const char* str, size_t length)
	{
		size_t seed = 0;
		for (size_t i = 0; i < length; ++i)
		{
			Hash_Combine(seed, size_t(str[i]));
		}
		return seed;
	}

	inline size_t StringToHash(const char* str)
	{
		size_t seed = 0;
		for (const char* p = str; 0 != *p; ++p)
		{
			Hash_Combine(seed, size_t(*p));
		}
		return seed;
	}

#}

}
