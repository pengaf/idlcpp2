#import "object.i"

#{
#include "utility.h"
#include <iterator>
#}

namespace pafcore
{
	class #PAFCORE_EXPORT Iterator : Object
	{
		virtual bool isEnd() const;
		virtual void advance(int offset);
		virtual void reset();
		virtual bool equal(Iterator* other) const;
#{
		void next()
		{
			advance(1);
		}
		void prev()
		{
			advance(-1);
		}
#}
	};

#{

	template<typename Iterator_t>
	void SafeAdvance(Iterator_t& iterator, int offset, Iterator_t begin, Iterator_t end)
	{
		if (offset > 0)
		{
			if (1 == offset)
			{
				if (iterator != end)
				{
					++iterator;
				}
			}
			else
			{
				int maxOffset = std::distance(iterator, end);
				if (offset > maxOffset)
				{
					iterator = end;
				}
				else
				{
					std::advance(iterator, offset);
				}
			}
		}
		else if (offset < 0)
		{
			if (-1 == offset)
			{
				if (iterator == begin)
				{
					iterator = end;
				}
				else if (iterator != end)
				{
					--iterator;
				}
			}
			else
			{
				int maxOffset = std::distance(begin, iterator);
				if (-offset > maxOffset)
				{
					iterator = end;
				}
				else
				{
					std::advance(iterator, offset);
				}
			}
		}
	}

	template<typename C, typename I = typename C::iterator>
	class IteratorImpl : public Iterator
	{
	public:
		typedef C ContainerType;
		typedef I IteratorType;
		typedef IteratorImpl<C, I> ThisType;
	public:
		IteratorImpl(C* container) :
			m_container(container),
			m_end(container->end()),
			m_iterator(m_container->begin())
		{}

		IteratorImpl(C* container, const I& iterator) :
			m_container(container),
			m_end(container->end()),
			m_iterator(iterator)
		{}

		IteratorImpl(C* container, const I& end, const I& iterator) :
			m_container(container),
			m_end(end),
			m_iterator(iterator)
		{}
	public:
		virtual bool isEnd() const override
		{
			return (m_end == m_iterator);
		}
		virtual void advance(int offset) override
		{
			SafeAdvance(m_iterator, offset, m_container->begin(), m_end);
		}
		virtual void reset() override
		{
			m_iterator = m_container->begin();
			//m_end = m_container->end();
		}
		virtual bool equal(Iterator* other) const override
		{
			return (static_cast<ThisType*>(other)->m_iterator == m_iterator);
		}
	public:
		//ContainerType* container()
		//{
		//	return m_container;
		//}
		IteratorType iterator() const
		{
			return m_iterator;
		}
	protected:
		ContainerType* m_container;
		IteratorType m_end;
		IteratorType m_iterator;
	};

	template<typename T>
	class ArrayIteratorImpl : public Iterator
	{
	public:
		typedef ArrayIteratorImpl<T> ThisType;
	public:
		ArrayIteratorImpl(T* begin, size_t size) :
			m_begin(begin),
			m_end(begin + size),
			m_iterator(begin)
		{}
		
		ArrayIteratorImpl(T* begin, size_t size, size_t index) :
			m_begin(begin),
			m_end(begin + size),
			m_iterator(begin + (index < size ? index : size))
		{}

	public:
		virtual bool isEnd() const override
		{
			return (m_iterator == m_end);
		}
		virtual void advance(int offset) override
		{
			SafeAdvance(m_iterator, offset, m_begin, m_end);
		}
		virtual void reset() override
		{
			m_iterator = m_begin;
		}
		virtual bool equal(Iterator* other) const override
		{
			ThisType* that = static_cast<ThisType*>(other);
			return (that->m_iterator == m_iterator);
		}
	public:
		T* begin() const
		{
			return m_begin;
		}
		T* end() const
		{
			return m_end;
		}
		ptrdiff_t index() const
		{
			return m_iterator - m_begin;
		}
	protected:
		T* m_begin;
		T* m_end;
		T* m_iterator;
	};

	template<typename C, typename I>
	static IteratorImpl<C, I> MakeIterator(C* c, const I& i)
	{
		return IteratorImpl<C, I>(c, i);
	}
	template<typename C>
	static IteratorImpl<C, typename C::iterator> MakeIterator(C* c)
	{
		return IteratorImpl<C, typename C::iterator>(c);
	}

	template<typename T>
	static ArrayIteratorImpl<T> MakeIterator(T* begin, size_t size)
	{
		return ArrayIteratorImpl<T>(begin, size);
	}

	template<typename T>
	static ArrayIteratorImpl<T> MakeIterator(T* begin, size_t size, size_t index)
	{
		return ArrayIteratorImpl<T>(begin, size, index);
	}

#}


}
