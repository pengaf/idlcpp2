#pragma once
#include "utility.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#define PAF_MEMORY_BACKEND_STD 0
#define PAF_MEMORY_BACKEND_MIMALLOC 1

#if !defined(PAF_MEMORY_BACKEND)
#define PAF_MEMORY_BACKEND PAF_MEMORY_BACKEND_STD
#endif

#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
#include <mimalloc.h>
#include <mimalloc-new-delete.h>
#endif


namespace pafcore
{

	inline void* Malloc(size_t size)
	{
#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
		return mi_malloc(size);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_STD
		return ::operator new(size);
#else
#error Unsupported PAF_MEMORY_BACKEND value.
#endif
	}

	inline void Free(void* p) noexcept
	{
#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
		mi_free(p);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_STD
		::operator delete(p);
#else
#error Unsupported PAF_MEMORY_BACKEND value.
#endif
	}

	template<typename T, typename... Args>
	void Construct(void* ptr, Args&&... args)
	{
		PAF_ASSERT(nullptr != ptr);
		new (ptr) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void ConstructArray(void* ptr, size_t count)
	{
		PAF_ASSERT(nullptr != ptr);
		if constexpr (!std::is_trivially_default_constructible_v<T>)
		{
			T* p = static_cast<T*>(ptr);
			for (size_t i = 0; i < count; ++i)
			{
				new (p + i) T;
			}
		}
	}

	template<typename T>
	bool Destruct(T* ptr, size_t count = 1)
	{
		PAF_ASSERT(nullptr != ptr);
		if constexpr (std::is_destructible_v<T>)
		{
			for (size_t i = 0; i < count; ++i)
			{
				ptr[i].~T();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	bool CopyConstruct(T* dst, const T* src, size_t count = 1)
	{
		PAF_ASSERT(nullptr != dst && nullptr != src);
		if constexpr (std::is_copy_constructible_v<T>)
		{
			for (size_t i = 0; i < count; ++i)
			{
				new (dst + i) T(src[i]);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	//template<typename T>
	//T* CopyNew(const T* src, size_t count = 1)
	//{
	//	if constexpr (std::is_copy_constructible_v<T>)
	//	{
	//		T* dst = (T*)Malloc(sizeof(T) * count);
	//		CopyConstruct(dst, src, count);
	//		return dst;
	//	}
	//	else
	//	{
	//		return nullptr;
	//	}
	//}

	template<typename T>
	bool CopyAssign(T* dst, const T* src, size_t count = 1)
	{
		PAF_ASSERT(nullptr != dst && nullptr != src);
		if constexpr (std::is_copy_assignable_v<T>)
		{
			for (size_t i = 0; i < count; ++i)
			{
				dst[i] = src[i];
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	bool MoveConstruct(T* dst, T* src, size_t count = 1)
	{
		PAF_ASSERT(nullptr != dst && nullptr != src);
		if constexpr (std::is_move_constructible_v<T>)
		{
			for (size_t i = 0; i < count; ++i)
			{
				new (dst + i) T(std::move(src[i]));
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	//template<typename T>
	//T* MoveNew(const T* src, size_t count = 1)
	//{
	//	if constexpr (std::is_move_constructible_v<T>)
	//	{
	//		T* dst = (T*)Malloc(sizeof(T) * count);
	//		MoveConstruct(dst, src, count);
	//		return dst;
	//	}
	//	else
	//	{
	//		return nullptr;
	//	}
	//}

	template<typename T>
	bool MoveAssign(T* dst, T* src, size_t count = 1)
	{
		PAF_ASSERT(nullptr != dst && nullptr != src);
		if constexpr (std::is_move_assignable_v<T>)
		{
			for (size_t i = 0; i < count; ++i)
			{
				dst[i] = std::move(src[i]);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	//struct ArrayHeader
	//{
	//	uint64_t m_size;
	//	ArrayHeader(uint64_t count) :
	//		m_size(count)
	//	{
	//	}
	//};

	struct STRCHeader
	{
		uint32_t m_strongRefCount{ 1 };
		uint32_t m_weakRefCount{ 1 };

		uint32_t incStrongRefCount() noexcept
		{
			return ++m_strongRefCount;
		}

		uint32_t incWeakRefCount() noexcept
		{
			return ++m_weakRefCount;
		}

		uint32_t decStrongRefCount() noexcept
		{
			return --m_strongRefCount;
		}

		uint32_t decWeakRefCount() noexcept
		{
			return --m_weakRefCount;
		}

		bool incStrongRefCountNotZero() noexcept
		{
			if (m_strongRefCount != 0)
			{
				++m_strongRefCount;
				return true;
			}
			return false;
		}
	};

	struct MTRCHeader
	{
		std::atomic<uint32_t> m_strongRefCount{ 1 };
		std::atomic<uint32_t> m_weakRefCount{ 1 };

		uint32_t incStrongRefCount() noexcept
		{
			return ++m_strongRefCount;
		}

		uint32_t incWeakRefCount() noexcept
		{
			return ++m_weakRefCount;
		}

		uint32_t decStrongRefCount() noexcept
		{
			return --m_strongRefCount;
		}

		uint32_t decWeakRefCount() noexcept
		{
			return --m_weakRefCount;
		}

		bool incStrongRefCountNotZero() noexcept
		{
			uint32_t current = m_strongRefCount.load(std::memory_order_relaxed);
			while (current != 0)
			{
				if (m_strongRefCount.compare_exchange_weak(current, current + 1, std::memory_order_acq_rel, std::memory_order_relaxed))
				{
					return true;
				}
			}
			return false;
		}
	};

	struct RefCountedArrayHeader
	{
		uint32_t m_refCount{ 1 };
		uint32_t m_size;
		RefCountedArrayHeader(uint32_t size) :
			m_size(size)
		{
		}
		uint32_t incRefCount() noexcept
		{
			return ++m_refCount;
		}
		uint32_t decRefCount() noexcept
		{
			return --m_refCount;
		}
	};

	inline uint32_t RefCountedArraySize(const void* ptr)
	{
		const RefCountedArrayHeader* header = reinterpret_cast<const RefCountedArrayHeader*>(ptr) - 1;
		return header->m_size;
	}

	template<typename RefCountHeader_t>
	inline uint32_t IncWeakRefCount(void* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incWeakRefCount();
	}

	template<typename RefCountHeader_t>
	inline uint32_t IncStrongRefCount(void* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incStrongRefCount();
	}

	template<typename RefCountHeader_t>
	inline uint32_t IncStrongRefCountNotZero(void* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incStrongRefCountNotZero();
	}

	template<typename RefCountHeader_t>
	inline uint32_t DecWeakRefCount(void* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		uint32_t result = header->decWeakRefCount();
		if (0 == result)
		{
			header->~RefCountHeader_t();
			Free(header);
		}
		return result;
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t DecStrongRefCount(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		uint32_t result = header->decStrongRefCount();
		if (0 == result)
		{
			object->~Object_t();
			DecWeakRefCount<RefCountHeader_t>(object);
		}
		return result;
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t GetWeakRefCount(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->m_weakRefCount;
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t GetStrongRefCount(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->m_strongRefCount;
	}

	inline uint32_t IncArrayRefCount(void* array)
	{
		RefCountedArrayHeader* header = reinterpret_cast<RefCountedArrayHeader*>(array) - 1;
		return header->incRefCount();
	}

	template<typename Object_t>
	inline uint32_t DecArrayRefCount(Object_t* array)
	{
		RefCountedArrayHeader* header = reinterpret_cast<RefCountedArrayHeader*>(array) - 1;
		uint32_t result = header->decRefCount();
		if (0 == result)
		{
			size_t count = header->m_size;
			for (size_t i = 0; i < count; ++i)
			{
				array[i].~Object_t();
			}
			header->~RefCountedArrayHeader();
			Free(header);
		}
		return result;
	}

	//general for Type::decSharedPtrRefCount
	template<typename RefCountHeader_t, typename Destruct_t>
	inline uint32_t DecStrongRefCount(void* object, Destruct_t destruct)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		uint32_t result = header->decStrongRefCount();
		if (0 == result)
		{
			destruct(object);
			DecWeakRefCount<RefCountHeader_t>(object);
		}
		return result;
	}

	//general for Type::decSharedArrayRefCount
	template<typename Destruct_t>
	inline uint32_t DecArrayRefCount(void* array, Destruct_t destruct)
	{
		RefCountedArrayHeader* header = reinterpret_cast<RefCountedArrayHeader*>(array) - 1;
		uint32_t result = header->decRefCount();
		if (0 == result)
		{
			size_t count = header->m_size;
			destruct(array, count);
			header->~RefCountedArrayHeader();
			Free(header);
		}
		return result;
	}

	template<typename T, typename... Args>
	inline T* New(Args&&... args)
	{
		void* ptr = Malloc(sizeof(T));
		return new (ptr) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline void Delete(T* ptr)
	{
		if (nullptr != ptr)
		{
			ptr->~T();
			Free(ptr);
		}
	}

	//template<typename T>
	//inline T* NewArray(size_t count)
	//{
	//	static_assert(alignof(T) <= sizeof(ArrayHeader), "Object alignment is not compatible with ArrayHeader.");
	//	size_t size = sizeof(ArrayHeader) + sizeof(T) * count;
	//	void* p = Malloc(size);
	//	ArrayHeader* header = static_cast<ArrayHeader*>(p);
	//	new(header)ArrayHeader(count);
	//	T* array = reinterpret_cast<T*>(header + 1);
	//	for (size_t i = 0; i < count; ++i)
	//	{
	//		new (array + i) T;
	//	}
	//	return array;
	//}

	//template<typename T>
	//inline void DeleteArray(T* array)
	//{
	//	if (nullptr != array)
	//	{
	//		ArrayHeader* header = static_cast<ArrayHeader*>(array) - 1;
	//		size_t count = header->m_size;
	//		for (size_t i = 0; i < count; ++i)
	//		{
	//			array[i].~T();
	//		}
	//		Free(header);
	//	}
	//}

	template<typename T, typename... Args>
	inline T* NewRefCounted(Args&&... args)
	{
		if constexpr (is_strc_object_v<T>)
		{
			static_assert(alignof(T) <= sizeof(STRCHeader), "Object alignment is not compatible with RefCountHeader.");
			size_t size = sizeof(STRCHeader) + sizeof(T);
			STRCHeader* header = static_cast<STRCHeader*>(Malloc(size));
			new(header)STRCHeader;
			T* object = reinterpret_cast<T*>(header + 1);
			return new (object) T(std::forward<Args>(args)...);
		}
		else if constexpr (is_mtrc_object_v<T>)
		{
			static_assert(alignof(T) <= sizeof(MTRCHeader), "Object alignment is not compatible with RefCountHeader.");
			size_t size = sizeof(MTRCHeader) + sizeof(T);
			MTRCHeader* header = static_cast<MTRCHeader*>(Malloc(size));
			new(header)MTRCHeader;
			T* object = reinterpret_cast<T*>(header + 1);
			return new (object) T(std::forward<Args>(args)...);
		}
		else
		{
			static_assert(alignof(T) <= sizeof(STRCHeader), "Object alignment is not compatible with RefCountHeader.");
			size_t size = sizeof(STRCHeader) + sizeof(T);
			STRCHeader* header = static_cast<STRCHeader*>(Malloc(size));
			new(header)STRCHeader;
			T* object = reinterpret_cast<T*>(header + 1);
			return new (object) T(std::forward<Args>(args)...);
		}
	}

	template<typename T>
	inline T* NewRefCountedArray(size_t count)
	{
		static_assert(!is_object_v<T> && !is_interface_v<T>, "NewRefCountedArray cannot be used with Object-derived or Interface-derived types.");
		static_assert(alignof(T) <= sizeof(RefCountedArrayHeader), "Object alignment is not compatible with RefCountedArrayHeader.");
		size_t size = sizeof(RefCountedArrayHeader) + sizeof(T) * count;
		RefCountedArrayHeader* header = static_cast<RefCountedArrayHeader*>(Malloc(size));
		new(header)RefCountedArrayHeader(count);
		T* array = reinterpret_cast<T*>(header + 1);
		for (size_t i = 0; i < count; ++i)
		{
			new (array + i) T;
		}
		return array;
	}

	template <typename T2>
	class ObserverPtr;

	template <typename T2>
	class SharedPtr;

	template <typename T2>
	class WeakPtr;

	template <typename T2>
	class ObserverArray;

	template <typename T2>
	class SharedArray;


	template<typename T>
	class ObserverPtr
	{
		friend class Variant;
	public:
		ObserverPtr() = default;

		ObserverPtr(std::nullptr_t)
		{}

		ObserverPtr(T* ptr) : 
			m_ptr(ptr)
		{}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		ObserverPtr(const ObserverPtr<U>& other) : 
			m_ptr(other.get())
		{}

		ObserverPtr& operator=(std::nullptr_t)
		{
			m_ptr = 0;
			return *this;
		}

		ObserverPtr& operator=(T* ptr)
		{
			m_ptr = ptr;
			return *this;
		}

		void assign(T* ptr)
		{
			m_ptr = ptr;
		}

		T* get() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

		operator T*() const
		{
			return m_ptr;
		}
	private:
		T* m_ptr{ nullptr };
	};


	template<>
	class ObserverPtr<void>
	{
	public:
		ObserverPtr() = default;

		ObserverPtr(std::nullptr_t)
		{}

		ObserverPtr(void* ptr) : 
			m_ptr(ptr)
		{}

		ObserverPtr& operator=(std::nullptr_t)
		{
			m_ptr = nullptr;
			return *this;
		}

		ObserverPtr& operator=(void* ptr)
		{
			m_ptr = ptr;
			return *this;
		}

		void* get() const
		{
			return m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

		operator void* () const
		{
			return m_ptr;
		}
	private:
		void* m_ptr{ nullptr };
	};


	template<typename T>
	class ObserverArray
	{
		friend class Variant;
	public:
		ObserverArray() = default;

		ObserverArray(std::nullptr_t)
		{}

		ObserverArray(T* ptr) : 
			m_ptr(ptr)
		{}

		ObserverArray& operator=(std::nullptr_t)
		{
			m_ptr = nullptr;
			return *this;
		}

		ObserverArray& operator=(T* ptr)
		{
			m_ptr = ptr;
			return *this;
		}

		void assign(T* ptr)
		{
			m_ptr = ptr;
		}

		T* get() const
		{
			return m_ptr;
		}

		uint32_t size() const
		{
			PAF_ASSERT(nullptr != m_ptr);
			return RefCountedArraySize(m_ptr);
		}

		T& operator[](size_t index) const
		{
			PAF_ASSERT(nullptr != m_ptr && index < size());
			return m_ptr[index];
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}
	private:
		T* m_ptr{ nullptr };
	};


	//template<typename T>
	//class UniquePtr
	//{
	//public:
	//	static_assert(!is_rc_object_v<T> && !is_interface_v<T>, "UniquePtr cannot own RCObject-derived or Interface-derived types.");

	//	UniquePtr() = default;

	//	UniquePtr(std::nullptr_t)
	//	{}

	//	explicit UniquePtr(T* ptr) : 
	//		m_ptr(ptr)
	//	{}

	//	UniquePtr(const UniquePtr&) = delete;

	//	UniquePtr& operator=(const UniquePtr&) = delete;

	//	UniquePtr(UniquePtr&& other) noexcept : 
	//		m_ptr(other.m_ptr)
	//	{
	//		other.m_ptr = nullptr;
	//	}

	//	UniquePtr& operator=(UniquePtr&& other) noexcept
	//	{
	//		if (this != &other)
	//		{
	//			reset();
	//			m_ptr = other.m_ptr;
	//			other.m_ptr = nullptr;
	//		}
	//		return *this;
	//	}

	//	~UniquePtr()
	//	{
	//		reset();
	//	}

	//	T* get() const
	//	{
	//		return m_ptr;
	//	}

	//	T* release()
	//	{
	//		T* ptr = m_ptr;
	//		m_ptr = 0;
	//		return ptr;
	//	}

	//	void reset(T* ptr = 0)
	//	{
	//		Delete(m_ptr);
	//		m_ptr = ptr;
	//	}

	//	T& operator*() const
	//	{
	//		return *m_ptr;
	//	}

	//	T* operator->() const
	//	{
	//		return m_ptr;
	//	}

	//	explicit operator bool() const
	//	{
	//		return 0 != m_ptr;
	//	}
	//private:
	//	T* m_ptr;
	//};

	//template<typename T, typename... Args>
	//inline UniquePtr<T> MakeUnique(Args&&... args)
	//{
	//	return UniquePtr<T>(New<T>(std::forward<Args>(args)...));
	//}

	//template<typename T>
	//class UniqueArray
	//{
	//public:
	//	static_assert(!is_rc_object_v<T> && !is_interface_v<T>, "UniqueArray cannot own RCObject-derived or Interface-derived types.");

	//	UniqueArray() = default;

	//	UniqueArray(std::nullptr_t)
	//	{}

	//	UniqueArray(T* ptr) : 
	//		m_ptr(ptr)
	//	{}

	//	UniqueArray(const UniqueArray&) = delete;

	//	UniqueArray& operator=(const UniqueArray&) = delete;

	//	UniqueArray(UniqueArray&& other) noexcept : 
	//		m_ptr(other.m_ptr)
	//	{
	//		other.m_ptr = nullptr;
	//	}

	//	UniqueArray& operator=(UniqueArray&& other) noexcept
	//	{
	//		if (this != &other)
	//		{
	//			reset();
	//			m_ptr = other.m_ptr;
	//			other.m_ptr = nullptr;
	//		}
	//		return *this;
	//	}

	//	~UniqueArray()
	//	{
	//		reset();
	//	}

	//	T* get() const
	//	{
	//		return m_ptr;
	//	}

	//	void reset(T* ptr = 0)
	//	{
	//		DeleteArray(m_ptr);
	//		m_ptr = ptr;
	//	}

	//	T& operator[](size_t index) const
	//	{
	//		return m_ptr[index];
	//	}

	//	explicit operator bool() const
	//	{
	//		return 0 != m_ptr;
	//	}
	//private:
	//	T* m_ptr;
	//};

	//template<typename T>
	//inline UniqueArray<T> MakeUniqueArray(size_t count)
	//{
	//	return UniqueArray<T>(NewArray<T>(count));
	//}

	template<typename T>
	class SharedPtr
	{
		friend class Variant;

		template <typename T2>
		friend class SharedPtr;

		template <typename T2>
		friend class WeakPtr;

	public:
		static SharedPtr Retain(T* ptr)
		{
			SharedPtr sharedPtr;
			sharedPtr.m_ptr = ptr;
			sharedPtr.incStrongRefCount();
			return sharedPtr;
		}

		SharedPtr() noexcept = default;

		SharedPtr(std::nullptr_t) noexcept
		{}

		explicit SharedPtr(T* ptr) noexcept :
			m_ptr(ptr)
		{}

		SharedPtr(const SharedPtr& other) noexcept :
			m_ptr(other.m_ptr)
		{
			incStrongRefCount();
		}

		SharedPtr(SharedPtr&& other) noexcept :
			m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template<typename T2>
		SharedPtr(const SharedPtr<T2>& other) noexcept :
			m_ptr(other.m_ptr)
		{
			incStrongRefCount();
		}

		template<typename T2>
		SharedPtr(SharedPtr<T2>&& other) noexcept :
			m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		SharedPtr& operator=(const SharedPtr& other) noexcept
		{
			if (this != &other)
			{
				decStrongRefCount();
				m_ptr = other.m_ptr;
				incStrongRefCount();
			}
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			if (this != &other)
			{
				decStrongRefCount();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		~SharedPtr()
		{
			decStrongRefCount();
		}

		T* get() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

	private:
		void incStrongRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				m_ptr->incStrongRefCount();
			}
			else
			{
				IncStrongRefCount<STRCHeader>(m_ptr);
			}
		}

		void decStrongRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				m_ptr->decStrongRefCount();
			}
			else
			{
				DecStrongRefCount<STRCHeader>(m_ptr);
			}
			//m_ptr = 0;
		}
	private:
		void assign(T* ptr)//for Variant
		{
			if (m_ptr != ptr)
			{
				decStrongRefCount();
				m_ptr = ptr;
				incStrongRefCount();
			}
		}
	private:
		T* m_ptr{ nullptr };
	};

	template<typename T, typename... Args>
	inline SharedPtr<T> MakeShared(Args&&... args)
	{
		return SharedPtr<T>(NewRefCounted<T>(std::forward<Args>(args)...));
	}

	template<typename T>
	class WeakPtr
	{
	public:

		WeakPtr() = default;

		WeakPtr(std::nullptr_t)
		{}

		explicit WeakPtr(T* ptr) : 
			m_ptr(ptr)
		{
			incWeakRefCount();
		}

		WeakPtr(const WeakPtr& other) :
			m_ptr(other.m_ptr)
		{
			incWeakRefCount();
		}

		WeakPtr& operator=(const WeakPtr& other)
		{
			if (this != &other)
			{
				decWeakRefCount();
				m_ptr = other.m_ptr;
				incWeakRefCount();
			}
			return *this;
		}

		WeakPtr(WeakPtr&& other) noexcept :
			m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		WeakPtr& operator=(WeakPtr&& other) noexcept
		{
			if (this != &other)
			{
				decWeakRefCount();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		~WeakPtr()
		{
			decWeakRefCount();
		}

		T* get() const
		{
			return m_ptr;
		}

		bool expired() const
		{
			return 0 == getStrongCount();
		}

		SharedPtr<T> lock() const
		{
			if (incStrongRefCountNotZero())
			{
				return SharedPtr<T>(m_ptr);
			}
			else
			{
				return SharedPtr<T>();
			}
		}

		explicit operator bool() const
		{
			return !expired();
		}

	private:
		void incWeakRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				m_ptr->incWeakRefCount();
			}
			else
			{
				IncWeakRefCount<STRCHeader>(m_ptr);
			}
		}

		void decWeakRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				m_ptr->decWeakRefCount();
			}
			else
			{
				DecWeakRefCount<STRCHeader>(m_ptr);
			}
			//m_ptr = 0;
		}

		bool incStrongRefCountNotZero()
		{
			if (0 == m_ptr)
			{
				return false;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				return m_ptr->incStrongRefCountNotZero();
			}
			else
			{
				return IncStrongRefCountNotZero<STRCHeader>(m_ptr);
			}
		}

		uint32_t getStrongCount() const
		{
			if (0 == m_ptr)
			{
				return 0;
			}
			if constexpr (is_object_v<T> || is_interface_v<T>)
			{
				return m_ptr->getStrongRefCount();
			}
			else
			{
				return GetStrongRefCount<STRCHeader>(m_ptr);
			}
		}
	private:
		T* m_ptr{ nullptr };
	};


	template<typename T>
	class SharedArray
	{
		friend class Variant;
	public:
		static_assert(!is_object_v<T> && !is_interface_v<T>, "SharedArray cannot own Object-derived or Interface-derived types.");

		SharedArray() = default;

		SharedArray(std::nullptr_t)
		{}

		SharedArray(T* ptr) :
			m_ptr(ptr)
		{}

		SharedArray(const SharedArray& other) :
			m_ptr(other.m_ptr)
		{
			incStrongRefCount();
		}

		SharedArray& operator=(const SharedArray& other)
		{
			if (this != &other)
			{
				decStrongRefCount();
				m_ptr = other.m_ptr;
				incStrongRefCount();
			}
			return *this;
		}

		SharedArray(SharedArray&& other) noexcept :
			m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		SharedArray& operator=(SharedArray&& other) noexcept
		{
			if (this != &other)
			{
				decStrongRefCount();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		~SharedArray()
		{
			decStrongRefCount();
		}

		T* get() const
		{
			return m_ptr;
		}

		uint32_t size() const
		{
			PAF_ASSERT(nullptr != m_ptr);
			return RefCountedArraySize(m_ptr);
		}

		T& operator[](size_t index) const
		{
			PAF_ASSERT(nullptr != m_ptr && index < size());
			return m_ptr[index];
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

	private:
		void incStrongRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			IncArrayRefCount(m_ptr);
		}

		void decStrongRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			DecArrayRefCount(m_ptr);
			m_ptr = nullptr;
		}
	private:
		void assign(T* ptr)//for Variant
		{
			if (m_ptr != p)
			{
				decStrongRefCount();
				m_ptr = ptr;
				incStrongRefCount();
			}
		}
	private:
		T* m_ptr{ nullptr };
	};

	template<typename T>
	SharedArray<T> MakeSharedArray(size_t count)
	{
		return SharedArray<T>(NewRefCountedArray<T>(count));
	}

	template<typename T>
	class StlAllocator
	{
	public:
		using value_type = T;

		StlAllocator() noexcept
		{}

		template<typename U>
		StlAllocator(const StlAllocator<U>&) noexcept
		{}

		T* allocate(std::size_t count)
		{
			return static_cast<T*>(Malloc(sizeof(T) * count));
		}

		void deallocate(T* p, std::size_t) noexcept
		{
			Free(p);
		}

		template<typename U>
		bool operator==(const StlAllocator<U>&) const noexcept
		{
			return true;
		}

		template<typename U>
		bool operator!=(const StlAllocator<U>&) const noexcept
		{
			return false;
		}
	};


	template<typename T>
	void ReleaseSharedPtr(T* ptr)
	{
		PAF_ASSERT(ptr != nullptr);
		if constexpr (is_object_v<T> || is_interface_v<T>)
		{
			ptr->decStrongRefCount();
		}
		else
		{
			DecStrongRefCount<STRCHeader>(ptr);
		}
	}

	template<typename T>
	void ReleaseSharedArray(T* ptr)
	{
		PAF_ASSERT(ptr != nullptr);
		DecArrayRefCount(ptr);
	}
}
