#pragma once

#include "memory.h"
#include <list>

namespace pafcore
{
	template<typename T, typename Allocator = StlAllocator<T> >
	using list = std::list<T, Allocator>;
}
