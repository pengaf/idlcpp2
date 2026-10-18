#pragma
#include "memory.h"
#include <unordered_set>
#include <mutex>

namespace pafcore
{
	template<typename T>	
	struct DefaultCopy
	{
		T operator()(const T& t) const
		{
			return t;
		}
	};

	template<typename T>
	struct DefaultRelease
	{
		void operator()(T& t) const
		{}
	};

	struct CStringCopy
	{
		const char* operator() (const char* str) const
		{
			size_t len = strlen(str) + 1;
			void* p = Malloc(len);
			memcpy(p, str, len);
			return (const char*)p;
		}
	};

	struct CStringRelease
	{
		void operator() (const char* str) const
		{
			Free((void*)str);
		}
	};

	struct CStringHash
	{
		size_t operator ()(const char* str) const
		{
			size_t seed = 0;
			for (const char* p = str; 0 != *p; ++p)
			{
				seed ^= size_t(*p) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};

	struct CStringEqual
	{
		bool operator() (const char* lhs, const char* rhs) const
		{
			return (0 == strcmp(lhs, rhs));
		}
	};


	template<typename Element_t_, 
		typename Hash_t_ = std::hash<Element_t_>,
		typename Equal_t_ = std::equal_to<Element_t_>,
		typename Copy_t_ = DefaultCopy<Element_t_>,
		typename Release_t_ = DefaultRelease<Element_t_>>
	struct PoolTraits
	{
		using Container_t = std::unordered_set<Element_t_, Hash_t_, Equal_t_>;
		using Copy_t = Copy_t_;
		using Release_t = Release_t_;
		static constexpr bool is_trivial_release = std::is_same_v<Release_t_, DefaultRelease<Element_t_>>;
	};

	template<typename Element_t_, typename PoolTraits_t_ = PoolTraits<Element_t_>>
	class Pool
	{
	public:
		using Element_t = Element_t_;
		using PoolTraits_t = PoolTraits_t_;
		using Container_t = PoolTraits_t::Container_t;
		using iterator = Container_t::iterator;
	public:
		Pool(const Pool&) = delete;
		Pool& operator = (const Pool&) = delete;
		~Pool()
		{
			if constexpr (!PoolTraits_t::is_trivial_release)
			{
				typename PoolTraits_t::Release_t release;
				auto it = m_container.begin();
				auto end = m_container.end();
				while (it != end)
				{
					auto elem = *it;
					it = m_container.erase(it);
					release(elem);
				}
			}
		}
	public:
		iterator begin()
		{
			return m_container.begin();
		}
		
		iterator end()
		{
			return m_container.end();
		}
		
		size_t size() const
		{
			return m_container.size();
		}
		
		iterator find(const Element_t& element) const
		{
			return m_container.find(element);
		}

		const Element_t& getElement(const Element_t& element)
		{
			auto it = m_container.find(element);
			if (it != m_container.end())
			{
				return *it;
			}
			else
			{
				typename PoolTraits_t::Copy_t copy;
				Element_t elm = copy(element);
				return *m_container.insert(elm).first;
			}
		}

	protected:
		Container_t m_container;
	};

	template<typename Pool_t_>
	class SyncPool
	{
	public:
		using Pool_t = Pool_t_;
		using Element_t = Pool_t::Element_t;
		using iterator = Pool_t::iterator;
	public:
		iterator begin()
		{
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			return m_pool.begin();
		}

		iterator end()
		{
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			return m_pool.end();
		}

		size_t size() const
		{
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			return m_pool.size();
		}

		iterator find(const Element_t& element) const
		{
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			return m_pool.find(element);
		}

		const Element_t& getElement(const Element_t& element)
		{
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			return m_pool.getElement(element);
		}

	protected:
		Pool_t m_pool;
		std::mutex m_mutex;
	};

	using StringPool = Pool<const char*, PoolTraits<const char*, CStringHash, CStringEqual, CStringCopy, CStringRelease>>;
	using SyncStringPool = SyncPool<StringPool>;

}