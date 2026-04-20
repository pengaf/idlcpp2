#pragma once

#include "memory.h"
#include <forward_list>

namespace pafcore
{
	template<typename T, typename Allocator = StlAllocator<T> >
	using forward_list = std::forward_list<T, Allocator>;
}
