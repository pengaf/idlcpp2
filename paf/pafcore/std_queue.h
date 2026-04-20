#pragma once

#include "std_deque.h"
#include "std_vector.h"
#include <queue>
#include <stack>

namespace pafcore
{
	template<typename T, typename Container = pafcore::deque<T> >
	using queue = std::queue<T, Container>;

	template<typename T, typename Container = pafcore::deque<T> >
	using stack = std::stack<T, Container>;

	template<typename T, typename Container = pafcore::vector<T>, typename Compare = std::less<typename Container::value_type> >
	using priority_queue = std::priority_queue<T, Container, Compare>;
}
