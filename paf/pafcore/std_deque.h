#pragma once

#include "memory.h"
#include <deque>

namespace pafcore
{
	template<typename T, typename Allocator = StlAllocator<T> >
	using deque = std::deque<T, Allocator>;
}
