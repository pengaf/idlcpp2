#pragma once

#include "memory.h"
#include <string>

namespace pafcore
{
	template<typename CharT, typename Traits = std::char_traits<CharT>, typename Allocator = StlAllocator<CharT> >
	using basic_string = std::basic_string<CharT, Traits, Allocator>;

	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
}
