#pragma once

#include "memory.h"
#include <set>

namespace pafcore
{
	template<typename Key, typename Compare = std::less<Key>, typename Allocator = StlAllocator<Key> >
	using set = std::set<Key, Compare, Allocator>;

	template<typename Key, typename Compare = std::less<Key>, typename Allocator = StlAllocator<Key> >
	using multiset = std::multiset<Key, Compare, Allocator>;
}
