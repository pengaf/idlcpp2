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
#endif


namespace pafcore
{
	class Interface;
	class Object;
	class RCObject;
	class STRCObject;
	class MTRCObject;

	template<typename Base, typename Derived>
	struct is_base_or_same
	{
		static constexpr bool value =
			std::is_base_of<Base, Derived>::value || 
			std::is_same<Base, Derived>::value;
	};

	template<typename T>
	struct is_interface : is_base_or_same<Interface, T> {};

	template<typename T>
	struct is_object : is_base_or_same<Object, T> {};

	template<typename T>
	struct is_rc_object : is_base_or_same<RCObject, T> {};

	template<typename T>
	struct is_strc_object : is_base_or_same<STRCObject, T> {};

	template<typename T>
	struct is_mtrc_object : is_base_or_same<MTRCObject, T> {};

	template<typename T>
	inline constexpr bool is_interface_v = is_interface<T>::value;

	template<typename T>
	inline constexpr bool is_object_v = is_object<T>::value;

	template<typename T>
	inline constexpr bool is_rc_object_v = is_rc_object<T>::value;

	template<typename T>
	inline constexpr bool is_strc_object_v = is_strc_object<T>::value;

	template<typename T>
	inline constexpr bool is_mtrc_object_v = is_mtrc_object<T>::value;

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

	struct ArrayHeader
	{
		uint64_t m_count;
		ArrayHeader(uint64_t count) : 
			m_count(count)
		{}
	};

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
		//std::atomic<uint32_t> m_refCount{ 1 };
		uint32_t m_refCount{ 1 };
		uint32_t m_count;
		RefCountedArrayHeader(uint32_t count) :
			m_count(count)
		{}
		uint32_t incRefCount() noexcept
		{
			return ++m_refCount;
		}
		uint32_t decRefCount() noexcept
		{
			return --m_refCount;
		}

	};


	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t IncWeakRefCount(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incWeakRefCount();
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t IncStrongRefCount(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incStrongRefCount();
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t IncStrongRefCountNotZero(Object_t* object)
	{
		RefCountHeader_t* header = reinterpret_cast<RefCountHeader_t*>(object) - 1;
		return header->incStrongRefCountNotZero();
	}

	template<typename RefCountHeader_t, typename Object_t>
	inline uint32_t DecWeakRefCount(Object_t* object)
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
			DecObjectWeakRefCount<Object_t, RefCountHeader_t>(object);
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

	template<typename Object_t>
	inline uint32_t IncArrayRefCount(Object_t* array)
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
			size_t count = header->m_count;
			for (size_t i = 0; i < count; ++i)
			{
				array[i].~Object_t();
			}
			header->~RefCountedArrayHeader();
			Free(header);
		}
		return result;
	}

	template<typename T, typename... Args>
	inline T* New(Args&&... args)
	{
		void* p = Malloc(sizeof(T));
		return new (p) T(std::forward<Args>(args)...);
	}

	template<typename T>
	inline void Delete(T* p)
	{
		if (nullptr != p)
		{
			p->~T();
			Free(p);
		}
	}

	template<typename T>
	inline T* NewArray(size_t count)
	{
		static_assert(alignof(T) <= sizeof(ArrayHeader), "Object alignment is not compatible with ArrayHeader.");
		size_t size = sizeof(ArrayHeader) + sizeof(T) * count;
		void* p = Malloc(size);
		ArrayHeader* header = static_cast<ArrayHeader*>(p);
		new(header)ArrayHeader(count);
		T* array = reinterpret_cast<T*>(header + 1);
		for (size_t i = 0; i < count; ++i)
		{
			new (array + i) T;
		}
		return array;
	}

	template<typename T>
	inline void DeleteArray(T* array)
	{
		if (nullptr != array)
		{
			ArrayHeader* header = static_cast<ArrayHeader*>(array) - 1;
			size_t count = header->m_count;
			for (size_t i = 0; i < count; ++i)
			{
				array[i].~T();
			}
			Free(header);
		}
	}

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

	class GenericSmartPtr
	{
		friend class Variant;
	public:
		GenericSmartPtr() = default;
		GenericSmartPtr(void* ptr) :
			m_ptr(ptr)
		{}

		GenericSmartPtr& operator=(const GenericSmartPtr& other) noexcept
		{
			m_ptr = other.m_ptr;
			return *this;
		}

		GenericSmartPtr& operator=(GenericSmartPtr&& other) noexcept
		{
			if (this != &other)
			{
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		void* get() const
		{
			return m_ptr;
		}
	protected:
		void* m_ptr = nullptr;
	};

	template<typename T>
	class ObserverPtr : public GenericSmartPtr
	{
	public:
		ObserverPtr() = default;

		ObserverPtr(std::nullptr_t)
		{}

		ObserverPtr(T* ptr) : 
			GenericSmartPtr(ptr)
		{}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		ObserverPtr(const ObserverPtr<U>& other) : 
			GenericSmartPtr(other.get())
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

		T* get() const
		{
			return (T*)m_ptr;
		}

		T& operator*() const
		{
			return *(T*)m_ptr;
		}

		T* operator->() const
		{
			return (T*)m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

		operator T*() const
		{
			return (T*)m_ptr;
		}
	};


	template<>
	class ObserverPtr<void> : public GenericSmartPtr
	{
	public:
		ObserverPtr() = default;

		ObserverPtr(std::nullptr_t)
		{}

		ObserverPtr(void* ptr) : 
			GenericSmartPtr(ptr)
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
	};


	template<typename T>
	class ObserverArray : public GenericSmartPtr
	{
	public:
		ObserverArray() = default;

		ObserverArray(std::nullptr_t)
		{}

		ObserverArray(T* ptr) : 
			GenericSmartPtr(ptr)
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

		T* get() const
		{
			return (T*)m_ptr;
		}

		T& operator[](size_t index) const
		{
			return ((T*)m_ptr)[index];
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}
	};


	template<typename T>
	class UniquePtr : public GenericSmartPtr
	{
	public:
		static_assert(!is_rc_object_v<T> && !is_interface_v<T>, "UniquePtr cannot own RCObject-derived or Interface-derived types.");

		UniquePtr() = default;

		UniquePtr(std::nullptr_t)
		{}

		explicit UniquePtr(T* ptr) : 
			GenericSmartPtr(ptr)
		{}

		UniquePtr(const UniquePtr&) = delete;

		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept : 
			GenericSmartPtr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		~UniquePtr()
		{
			reset();
		}

		T* get() const
		{
			return (T*)m_ptr;
		}

		T* release()
		{
			T* ptr = m_ptr;
			m_ptr = 0;
			return (T*)ptr;
		}

		void reset(T* ptr = 0)
		{
			Delete((T*)m_ptr);
			m_ptr = ptr;
		}

		T& operator*() const
		{
			return *(T*)m_ptr;
		}

		T* operator->() const
		{
			return (T*)m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}
	};

	template<typename T, typename... Args>
	inline UniquePtr<T> MakeUnique(Args&&... args)
	{
		return UniquePtr<T>(New<T>(std::forward<Args>(args)...));
	}

	template<typename T>
	class UniqueArray : public GenericSmartPtr
	{
	public:
		static_assert(!is_rc_object_v<T> && !is_interface_v<T>, "UniqueArray cannot own RCObject-derived or Interface-derived types.");

		UniqueArray() = default;

		UniqueArray(std::nullptr_t)
		{}

		UniqueArray(T* ptr) : 
			GenericSmartPtr(ptr)
		{}

		UniqueArray(const UniqueArray&) = delete;

		UniqueArray& operator=(const UniqueArray&) = delete;

		UniqueArray(UniqueArray&& other) noexcept : 
			GenericSmartPtr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		UniqueArray& operator=(UniqueArray&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		~UniqueArray()
		{
			reset();
		}

		T* get() const
		{
			return (T*)m_ptr;
		}

		void reset(T* ptr = 0)
		{
			DeleteArray((T*)m_ptr);
			m_ptr = ptr;
		}

		T& operator[](size_t index) const
		{
			return ((T*)m_ptr)[index];
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}
	};

	template<typename T>
	inline UniqueArray<T> MakeUniqueArray(size_t count)
	{
		return UniqueArray<T>(NewArray<T>(count), count);
	}

	template<typename T>
	class SharedPtr : public GenericSmartPtr
	{
	public:
		static SharedPtr Retain(T* ptr)
		{
			SharedPtr sharedPtr;
			sharedPtr.m_ptr = ptr;
			sharedPtr.incStrongRefCount();
			return sharedPtr;
		}

		SharedPtr() = default;

		SharedPtr(std::nullptr_t)
		{}

		explicit SharedPtr(T* ptr) : 
			GenericSmartPtr(ptr)
		{}

		SharedPtr(const SharedPtr& other) : 
			GenericSmartPtr(other.m_ptr)
		{
			incStrongRefCount();
		}

		SharedPtr& operator=(const SharedPtr& other)
		{
			if (this != &other)
			{
				decStrongRefCount();
				m_ptr = other.m_ptr;
				incStrongRefCount();
			}
			return *this;
		}

		SharedPtr(SharedPtr&& other) noexcept : 
			GenericSmartPtr(other.m_ptr)
		{
			other.m_ptr = nullptr;
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
			return (T*)m_ptr;
		}

		T& operator*() const
		{
			return *(T*)m_ptr;
		}

		T* operator->() const
		{
			return (T*)m_ptr;
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
				((T*)m_ptr)->incStrongRefCount();
			}
			else
			{
				IncStrongRefCount<STRCHeader>((T*)m_ptr);
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
				((T*)m_ptr)->decStrongRefCount();
			}
			else
			{
				DecStrongRefCount<STRCHeader>((T*)m_ptr);
			}
			//m_ptr = 0;
		}
	};

	template<typename T, typename... Args>
	inline SharedPtr<T> MakeShared(Args&&... args)
	{
		return SharedPtr<T>(NewRefCounted<T>(std::forward<Args>(args)...));
	}

	template<typename T>
	class WeakPtr : public GenericSmartPtr
	{
	public:

		WeakPtr() = default;

		WeakPtr(std::nullptr_t)
		{}

		explicit WeakPtr(T* ptr) : 
			GenericSmartPtr(ptr)
		{
			incWeakRefCount();
		}

		WeakPtr(const WeakPtr& other) :
			GenericSmartPtr(other.m_ptr)
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
			GenericSmartPtr(other.m_ptr)
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
			return (T*)m_ptr;
		}

		bool expired() const
		{
			return 0 == getStrongCount();
		}

		SharedPtr<T> lock() const
		{
			if (incStrongRefIfPositive())
			{
				return SharedPtr<T>((T*)m_ptr);
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
				((T*)m_ptr)->incWeakRefCount();
			}
			else
			{
				IncWeakRefCount<STRCHeader>((T*)m_ptr);
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
				((T*)m_ptr)->decWeakRefCount();
			}
			else
			{
				DecWeakRefCount<STRCHeader>((T*)m_ptr);
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
				return ((T*)m_ptr)->incStrongRefCountNotZero();
			}
			else
			{
				return IncStrongRefCountNotZero<STRCHeader>((T*)m_ptr);
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
				return ((T*)m_ptr)->getStrongRefCount();
			}
			else
			{
				return GetStrongRefCount<STRCHeader>((T*)m_ptr);
			}
		}
	};


	template<typename T>
	class SharedArray : public GenericSmartPtr
	{
	public:
		static_assert(!is_rc_object_v<T> && !is_interface_v<T>, "SharedArray cannot own RCObject-derived or Interface-derived types.");

		SharedArray() = default;

		SharedArray(std::nullptr_t)
		{}

		SharedArray(T* ptr) :
			GenericSmartPtr(ptr)
		{}

		SharedArray(const SharedArray& other) :
			GenericSmartPtr(other.m_ptr)
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
			return (T*)m_ptr;
		}

		T& operator[](size_t index) const
		{
			return ((T*)m_ptr)[index];
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
			IncArrayRefCount((T*)m_ptr);
		}

		void decStrongRefCount()
		{
			if (0 == m_ptr)
			{
				return;
			}
			DecArrayRefCount((T*)m_ptr);
			m_ptr = nullptr;
		}
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
			return static_cast<T*>(Malloc(sizeof(T) * count, alignof(T)));
		}

		void deallocate(T* p, std::size_t) noexcept
		{
			Free(p, alignof(T));
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

}
