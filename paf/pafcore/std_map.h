#pragma once

#include "memory.h"
#include <map>

namespace pafcore
{
	template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = StlAllocator<std::pair<const Key, T> > >
	using map = std::map<Key, T, Compare, Allocator>;

	template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = StlAllocator<std::pair<const Key, T> > >
	using multimap = std::multimap<Key, T, Compare, Allocator>;
}
