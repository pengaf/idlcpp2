#pragma once

#include "utility.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

BEGIN_PAFCORE

template<class T, class Pred = std::less<T>, class A = pafcore::StlAllocator<T> >
class FlatSet
{
public:
	typedef T key_type;
	typedef Pred key_compare;
	typedef Pred value_compare;
	typedef A allocator_type;
	typedef uint32_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef value_type& reference;
	typedef value_type const& const_reference;
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> _Pairib;
	typedef std::pair<iterator, iterator> _Pairii;
	typedef std::pair<const_iterator, const_iterator> _Paircc;

	FlatSet()
		: m_ptr(0)
		, m_size(0)
		, m_capacity(0)
	{
	}

	FlatSet(const FlatSet& other)
		: m_ptr(0)
		, m_size(0)
		, m_capacity(0)
	{
		reserve(other.m_size);
		for (size_type i = 0; i < other.m_size; ++i)
		{
			constructAt(m_ptr + i, other.m_ptr[i]);
		}
		m_size = other.m_size;
	}

	FlatSet& operator=(const FlatSet& other)
	{
		if (this != &other)
		{
			clear();
			reserve(other.m_size);
			for (size_type i = 0; i < other.m_size; ++i)
			{
				constructAt(m_ptr + i, other.m_ptr[i]);
			}
			m_size = other.m_size;
		}
		return *this;
	}

	FlatSet(FlatSet&& other) noexcept
		: m_ptr(other.m_ptr)
		, m_size(other.m_size)
		, m_capacity(other.m_capacity)
	{
		other.m_ptr = 0;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	FlatSet& operator=(FlatSet&& other) noexcept
	{
		if (this != &other)
		{
			destroyStorage();
			m_ptr = other.m_ptr;
			m_size = other.m_size;
			m_capacity = other.m_capacity;
			other.m_ptr = 0;
			other.m_size = 0;
			other.m_capacity = 0;
		}
		return *this;
	}

	~FlatSet()
	{
		destroyStorage();
	}

	void reserve(size_type count)
	{
		if (count <= m_capacity)
		{
			return;
		}

		value_type* newPtr = allocate(count);
		for (size_type i = 0; i < m_size; ++i)
		{
			constructAt(newPtr + i, std::move(m_ptr[i]));
			destroyAt(m_ptr + i);
		}
		deallocate(m_ptr, m_capacity);
		m_ptr = newPtr;
		m_capacity = count;
	}

	T* data()
	{
		return m_ptr;
	}
	
	const T* data() const
	{
		return m_ptr;
	}

	iterator begin()
	{
		return m_ptr;
	}

	const_iterator begin() const
	{
		return m_ptr;
	}

	iterator end()
	{
		return m_ptr ? (m_ptr + m_size) : 0;
	}

	const_iterator end() const
	{
		return m_ptr ? (m_ptr + m_size) : 0;
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	size_type size() const
	{
		return m_size;
	}

	size_type max_size() const
	{
		return static_cast<size_type>(AllocatorTraits::max_size(m_allocator));
	}

	bool empty() const
	{
		return 0 == m_size;
	}

	reference operator[](size_type n)
	{
		PAF_ASSERT(n < m_size);
		return m_ptr[n];
	}

	const_reference operator[](size_type n) const
	{
		PAF_ASSERT(n < m_size);
		return m_ptr[n];
	}

	reference front()
	{
		PAF_ASSERT(0 != m_size);
		return m_ptr[0];
	}

	const_reference front() const
	{
		PAF_ASSERT(0 != m_size);
		return m_ptr[0];
	}

	reference back()
	{
		PAF_ASSERT(0 != m_size);
		return m_ptr[m_size - 1];
	}

	const_reference back() const
	{
		PAF_ASSERT(0 != m_size);
		return m_ptr[m_size - 1];
	}

	A get_allocator() const
	{
		return m_allocator;
	}

	_Pairib insert(const value_type& x)
	{
		size_type pos = lowerBoundIndex(x);
		if (pos < m_size && isEquivalent(x, m_ptr[pos]))
		{
			return _Pairib(m_ptr + pos, false);
		}

		insertAt(pos, x);
		return _Pairib(m_ptr + pos, true);
	}

	iterator insert(iterator it, const value_type& x)
	{
		size_type pos = iteratorToIndex(it);
		PAF_ASSERT(empty() ||
			(pos == m_size && key_compare()(m_ptr[pos - 1], x)) ||
			(pos < m_size && key_compare()(x, m_ptr[pos])));
		insertAt(pos, x);
		return m_ptr + pos;
	}

	iterator erase(iterator it)
	{
		size_type pos = iteratorToIndex(it);
		PAF_ASSERT(pos < m_size);
		for (size_type i = pos; i + 1 < m_size; ++i)
		{
			m_ptr[i] = std::move(m_ptr[i + 1]);
		}
		destroyAt(m_ptr + m_size - 1);
		--m_size;
		return pos < m_size ? (m_ptr + pos) : end();
	}

	iterator erase(iterator first, iterator last)
	{
		size_type firstPos = iteratorToIndex(first);
		size_type lastPos = iteratorToIndex(last);
		PAF_ASSERT(firstPos <= lastPos);
		PAF_ASSERT(lastPos <= m_size);

		size_type count = lastPos - firstPos;
		if (0 == count)
		{
			return firstPos < m_size ? (m_ptr + firstPos) : end();
		}

		for (size_type i = firstPos; i + count < m_size; ++i)
		{
			m_ptr[i] = std::move(m_ptr[i + count]);
		}
		for (size_type i = 0; i < count; ++i)
		{
			destroyAt(m_ptr + m_size - 1 - i);
		}
		m_size -= count;
		return firstPos < m_size ? (m_ptr + firstPos) : end();
	}

	size_type erase(const T& t)
	{
		iterator it = find(t);
		if (end() == it)
		{
			return 0;
		}
		erase(it);
		return 1;
	}

	void clear()
	{
		for (size_type i = 0; i < m_size; ++i)
		{
			destroyAt(m_ptr + i);
		}
		m_size = 0;
	}

	const_iterator find(const T& t) const
	{
		size_type pos = lowerBoundIndex(t);
		if (pos < m_size && isEquivalent(t, m_ptr[pos]))
		{
			return m_ptr + pos;
		}
		return end();
	}

	iterator find(const T& t)
	{
		size_type pos = lowerBoundIndex(t);
		if (pos < m_size && isEquivalent(t, m_ptr[pos]))
		{
			return m_ptr + pos;
		}
		return end();
	}

	size_type count(const T& t) const
	{
		return end() != find(t) ? 1 : 0;
	}

	const_iterator lower_bound(const T& t) const
	{
		size_type pos = lowerBoundIndex(t);
		return pos < m_size ? (m_ptr + pos) : end();
	}

	const_iterator upper_bound(const T& t) const
	{
		size_type pos = upperBoundIndex(t);
		return pos < m_size ? (m_ptr + pos) : end();
	}

	_Paircc equal_range(const T& t) const
	{
		size_type lower = lowerBoundIndex(t);
		if (lower < m_size && isEquivalent(t, m_ptr[lower]))
		{
			return _Paircc(m_ptr + lower, m_ptr + lower + 1);
		}
		const_iterator it = lower < m_size ? (m_ptr + lower) : end();
		return _Paircc(it, it);
	}

	iterator lower_bound(const T& t)
	{
		size_type pos = lowerBoundIndex(t);
		return pos < m_size ? (m_ptr + pos) : end();
	}

	iterator upper_bound(const T& t)
	{
		size_type pos = upperBoundIndex(t);
		return pos < m_size ? (m_ptr + pos) : end();
	}

	_Pairii equal_range(const T& t)
	{
		size_type lower = lowerBoundIndex(t);
		if (lower < m_size && isEquivalent(t, m_ptr[lower]))
		{
			return _Pairii(m_ptr + lower, m_ptr + lower + 1);
		}
		iterator it = lower < m_size ? (m_ptr + lower) : end();
		return _Pairii(it, it);
	}

private:
	typedef std::allocator_traits<allocator_type> AllocatorTraits;

	value_type* allocate(size_type count)
	{
		return 0 != count ? AllocatorTraits::allocate(m_allocator, count) : 0;
	}

	void deallocate(value_type* ptr, size_type count)
	{
		if (0 != ptr)
		{
			AllocatorTraits::deallocate(m_allocator, ptr, count);
		}
	}

	template<typename U>
	void constructAt(value_type* ptr, U&& value)
	{
		AllocatorTraits::construct(m_allocator, ptr, std::forward<U>(value));
	}

	void destroyAt(value_type* ptr)
	{
		AllocatorTraits::destroy(m_allocator, ptr);
	}

	void destroyStorage()
	{
		clear();
		deallocate(m_ptr, m_capacity);
		m_ptr = 0;
		m_capacity = 0;
	}

	void growIfNeeded()
	{
		if (m_size < m_capacity)
		{
			return;
		}

		size_type newCapacity = 0 == m_capacity ? 1 : (m_capacity * 2);
		reserve(newCapacity);
	}

	size_type iteratorToIndex(const_iterator it) const
	{
		if (0 == m_ptr || 0 == it)
		{
			return 0;
		}
		return static_cast<size_type>(it - m_ptr);
	}

	size_type lowerBoundIndex(const T& t) const
	{
		key_compare pred;
		size_type first = 0;
		size_type count = m_size;
		while (0 < count)
		{
			size_type step = count / 2;
			size_type mid = first + step;
			if (pred(m_ptr[mid], t))
			{
				first = mid + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}
		return first;
	}

	size_type upperBoundIndex(const T& t) const
	{
		key_compare pred;
		size_type first = 0;
		size_type count = m_size;
		while (0 < count)
		{
			size_type step = count / 2;
			size_type mid = first + step;
			if (!pred(t, m_ptr[mid]))
			{
				first = mid + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}
		return first;
	}

	bool isEquivalent(const T& lhs, const T& rhs) const
	{
		key_compare pred;
		return !pred(lhs, rhs) && !pred(rhs, lhs);
	}

	void insertAt(size_type pos, const value_type& value)
	{
		growIfNeeded();

		if (pos < m_size)
		{
			constructAt(m_ptr + m_size, std::move(m_ptr[m_size - 1]));
			for (size_type i = m_size - 1; i > pos; --i)
			{
				m_ptr[i] = std::move(m_ptr[i - 1]);
			}
			m_ptr[pos] = value;
		}
		else
		{
			constructAt(m_ptr + m_size, value);
		}
		++m_size;
	}

private:
	allocator_type m_allocator;
	value_type* m_ptr;
	size_type m_size;
	size_type m_capacity;
};

END_PAFCORE
