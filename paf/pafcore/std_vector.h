#pragma once

#include "memory.h"
#include <vector>

namespace pafcore
{
	template<typename T, typename Allocator = StlAllocator<T> >
	using vector = std::vector<T, Allocator>;
}
