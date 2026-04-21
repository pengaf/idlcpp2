#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#define PAF_MEMORY_BACKEND_STD 0
#define PAF_MEMORY_BACKEND_MIMALLOC 1
#define PAF_MEMORY_BACKEND_CUSTOM 2

#if !defined(PAF_MEMORY_BACKEND)
#define PAF_MEMORY_BACKEND PAF_MEMORY_BACKEND_STD
#endif

#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
#include <mimalloc.h>
#endif

#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_CUSTOM
#if !defined(PAF_MEMORY_CUSTOM_ALLOC) || !defined(PAF_MEMORY_CUSTOM_FREE)
#error Custom memory backend requires PAF_MEMORY_CUSTOM_ALLOC and PAF_MEMORY_CUSTOM_FREE.
#endif
#endif

namespace pafcore
{
	class Interface;
	class Object;

	struct RefCountHeader
	{
		int32_t strongCount;
		int32_t weakCount;
	};

	inline int32_t AtomicInc(int32_t& value)
	{
		return reinterpret_cast<std::atomic<int32_t>&>(value).fetch_add(1, std::memory_order_relaxed) + 1;
	}

	inline int32_t AtomicDec(int32_t& value)
	{
		return reinterpret_cast<std::atomic<int32_t>&>(value).fetch_sub(1, std::memory_order_acq_rel) - 1;
	}

	inline bool AtomicIncIfPositive(int32_t& value)
	{
		std::atomic<int32_t>& atomicValue = reinterpret_cast<std::atomic<int32_t>&>(value);
		int32_t current = atomicValue.load(std::memory_order_acquire);
		while (current > 0)
		{
			if (atomicValue.compare_exchange_weak(current, current + 1, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				return true;
			}
		}
		return false;
	}

	inline int32_t NonAtomicInc(int32_t& value)
	{
		return ++value;
	}

	inline int32_t NonAtomicDec(int32_t& value)
	{
		return --value;
	}

	inline bool NonAtomicIncIfPositive(int32_t& value)
	{
		if (value <= 0)
		{
			return false;
		}
		++value;
		return true;
	}

#if defined(PAFCORE_EXPORT)
#define PAFCORE_MEMORY_API PAFCORE_EXPORT
#else
#define PAFCORE_MEMORY_API
#endif

	PAFCORE_MEMORY_API void IncSharedInterfaceStrong(void* rootAddress);
	PAFCORE_MEMORY_API void ReleaseSharedInterfaceStrong(void* rootAddress);

#undef PAFCORE_MEMORY_API

	inline RefCountHeader* GetRefCountFromObjectAddress(void* objectAddress)
	{
		return reinterpret_cast<RefCountHeader*>(objectAddress) - 1;
	}

	inline const RefCountHeader* GetRefCountFromObjectAddress(const void* objectAddress)
	{
		return reinterpret_cast<const RefCountHeader*>(objectAddress) - 1;
	}

	inline void* GetObjectAddressFromRefCount(RefCountHeader* refCount)
	{
		return reinterpret_cast<void*>(refCount + 1);
	}

	inline const void* GetObjectAddressFromRefCount(const RefCountHeader* refCount)
	{
		return reinterpret_cast<const void*>(refCount + 1);
	}

	template<typename T>
	struct is_interface : std::is_base_of<Interface, T> {};

	template<typename T>
	struct is_object : std::is_base_of<Object, T> {};

	template<typename T>
	inline constexpr bool is_interface_v = is_interface<T>::value;

	template<typename T>
	inline constexpr bool is_object_v = is_object<T>::value;

	template<typename T>
	struct is_atomic_object : std::false_type {};

	template<typename T>
	inline constexpr bool is_atomic_object_v = is_atomic_object<T>::value;

	template<typename T>
	inline void* SharedRootAddress(T* ptr)
	{
		if constexpr (is_object_v<T>)
		{
			return ptr;
		}
		else
		{
			static_assert(is_interface_v<T>, "Shared pointer target must derive from Object or Interface.");
			return reinterpret_cast<void*>(ptr->getAddress());
		}
	}

	inline size_t AlignUp(size_t value, size_t alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	inline size_t NormalizeAlignment(size_t alignment)
	{
		return alignment > alignof(std::max_align_t) ? alignment : alignof(std::max_align_t);
	}

	inline void* BackendAllocate(size_t size, size_t alignment)
	{
		if (0 == size)
		{
			size = 1;
		}

#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
		return mi_malloc_aligned(size, alignment);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_CUSTOM
		return PAF_MEMORY_CUSTOM_ALLOC(size, alignment);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_STD
		if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			return ::operator new(size, std::align_val_t(alignment));
		}
		return ::operator new(size);
#else
#error Unsupported PAF_MEMORY_BACKEND value.
#endif
	}

	inline void BackendDeallocate(void* p, size_t alignment) noexcept
	{
		if (0 == p)
		{
			return;
		}

#if PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_MIMALLOC
		mi_free(p);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_CUSTOM
		PAF_MEMORY_CUSTOM_FREE(p, alignment);
#elif PAF_MEMORY_BACKEND == PAF_MEMORY_BACKEND_STD
		if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			::operator delete(p, std::align_val_t(alignment));
			return;
		}
		::operator delete(p);
#else
#error Unsupported PAF_MEMORY_BACKEND value.
#endif
	}

	inline void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t))
	{
		return BackendAllocate(size, NormalizeAlignment(alignment));
	}

	inline void Deallocate(void* p, size_t alignment = alignof(std::max_align_t)) noexcept
	{
		BackendDeallocate(p, NormalizeAlignment(alignment));
	}

	template<typename T>
	inline size_t ObjectBlockAlignment()
	{
		return NormalizeAlignment(alignof(T) > alignof(RefCountHeader) ? alignof(T) : alignof(RefCountHeader));
	}

	template<typename T>
	inline size_t ObjectOffset()
	{
		static_assert(0 == sizeof(RefCountHeader) % alignof(T), "Object alignment is not compatible with RefCountHeader.");
		return AlignUp(sizeof(RefCountHeader), alignof(T));
	}

	template<typename T>
	inline RefCountHeader* GetRefCountFromTypedObject(T* p)
	{
		return reinterpret_cast<RefCountHeader*>(reinterpret_cast<char*>(p) - ObjectOffset<T>());
	}

	template<typename T>
	inline RefCountHeader* GetRefCountFromSharedPtr(T* p)
	{
		return GetRefCountFromObjectAddress(SharedRootAddress(p));
	}

	template<typename T>
	inline void IncStrong(T* p)
	{
		RefCountHeader* rc = GetRefCountFromSharedPtr(p);
		if constexpr (is_atomic_object_v<T>)
		{
			AtomicInc(rc->strongCount);
		}
		else
		{
			NonAtomicInc(rc->strongCount);
		}
	}

	template<typename T>
	inline bool IncStrongIfPositive(T* p)
	{
		RefCountHeader* rc = GetRefCountFromSharedPtr(p);
		if constexpr (is_atomic_object_v<T>)
		{
			return AtomicIncIfPositive(rc->strongCount);
		}
		else
		{
			return NonAtomicIncIfPositive(rc->strongCount);
		}
	}

	template<typename T>
	inline int32_t DecStrong(T* p)
	{
		RefCountHeader* rc = GetRefCountFromSharedPtr(p);
		if constexpr (is_atomic_object_v<T>)
		{
			return AtomicDec(rc->strongCount);
		}
		else
		{
			return NonAtomicDec(rc->strongCount);
		}
	}

	template<typename T>
	inline void IncWeak(T* p)
	{
		RefCountHeader* rc = GetRefCountFromSharedPtr(p);
		if constexpr (is_atomic_object_v<T>)
		{
			AtomicInc(rc->weakCount);
		}
		else
		{
			NonAtomicInc(rc->weakCount);
		}
	}

	template<typename T>
	inline int32_t DecWeak(T* p)
	{
		RefCountHeader* rc = GetRefCountFromSharedPtr(p);
		if constexpr (is_atomic_object_v<T>)
		{
			return AtomicDec(rc->weakCount);
		}
		else
		{
			return NonAtomicDec(rc->weakCount);
		}
	}

	template<typename T>
	inline void* GetObjectBlockAddress(T* p)
	{
		return reinterpret_cast<void*>(GetRefCountFromTypedObject(p));
	}

	template<typename T, typename... Args>
	inline T* CreateObject(Args&&... args)
	{
		const size_t alignment = ObjectBlockAlignment<T>();
		const size_t objectOffset = ObjectOffset<T>();
		const size_t bytes = objectOffset + sizeof(T);
		RefCountHeader* header = static_cast<RefCountHeader*>(Allocate(bytes, alignment));
		header->strongCount = 1;
		header->weakCount = 0;
		T* object = reinterpret_cast<T*>(reinterpret_cast<char*>(header) + objectOffset);
		try
		{
			return new (object) T(std::forward<Args>(args)...);
		}
		catch (...)
		{
			Deallocate(header, alignment);
			throw;
		}
	}

	template<typename T>
	inline void DeleteObject(T* p)
	{
		if (0 == p)
		{
			return;
		}

		RefCountHeader* header = GetRefCountFromTypedObject(p);
		const size_t alignment = ObjectBlockAlignment<T>();
		p->~T();
		if (0 == header->weakCount)
		{
			Deallocate(header, alignment);
		}
	}

	template<typename T>
	inline void DeleteSharedObject(T* p)
	{
		if constexpr (is_object_v<T>)
		{
			DeleteObject(p);
		}
		else
		{
			static_assert(is_interface_v<T>, "Shared pointer target must derive from Object or Interface.");
			ReleaseSharedInterfaceStrong(SharedRootAddress(p));
		}
	}

	template<typename T>
	inline T* CreateObjectArray(size_t count)
	{
		return CreateArray<T>(count);
	}

	template<typename T>
	inline void DeleteObjectArray(T* p, size_t)
	{
		DestroyArray(p);
	}

	template<typename T>
	inline size_t SharedArrayBlockAlignment()
	{
		return NormalizeAlignment(alignof(T) > alignof(RefCountHeader) ? alignof(T) : alignof(RefCountHeader));
	}

	template<typename T>
	inline size_t SharedArrayDataOffset()
	{
		return AlignUp(sizeof(RefCountHeader), alignof(T));
	}

	template<typename T>
	inline RefCountHeader* GetRefCountFromSharedArray(T* p)
	{
		return reinterpret_cast<RefCountHeader*>(reinterpret_cast<char*>(p) - SharedArrayDataOffset<T>());
	}

	template<typename T>
	inline T* CreateSharedArray(size_t count)
	{
		if (0 == count)
		{
			return 0;
		}

		const size_t alignment = SharedArrayBlockAlignment<T>();
		const size_t dataOffset = SharedArrayDataOffset<T>();
		const size_t bytes = dataOffset + sizeof(T) * count;
		RefCountHeader* header = static_cast<RefCountHeader*>(Allocate(bytes, alignment));
		header->strongCount = 1;
		header->weakCount = 0;
		T* items = reinterpret_cast<T*>(reinterpret_cast<char*>(header) + dataOffset);

		size_t constructed = 0;
		try
		{
			for (; constructed < count; ++constructed)
			{
				new (items + constructed) T;
			}
		}
		catch (...)
		{
			while (constructed > 0)
			{
				--constructed;
				items[constructed].~T();
			}
			Deallocate(header, alignment);
			throw;
		}

		return items;
	}

	template<typename T>
	inline void DeleteSharedArray(T* p, size_t count)
	{
		if (0 == p)
		{
			return;
		}

		RefCountHeader* header = GetRefCountFromSharedArray(p);
		const size_t alignment = SharedArrayBlockAlignment<T>();
		while (count > 0)
		{
			--count;
			p[count].~T();
		}
		if (0 == header->weakCount)
		{
			Deallocate(header, alignment);
		}
	}

	template<typename T>
	class SharedArray;

	template<typename T>
	inline SharedArray<T> MakeSharedArray(size_t count)
	{
		return SharedArray<T>(CreateSharedArray<T>(count), count);
	}

	struct ArrayHeader
	{
		size_t count;
		size_t alignment;
	};

	template<typename T>
	inline size_t ArrayDataOffset()
	{
		return AlignUp(sizeof(ArrayHeader), alignof(T));
	}

	template<typename T, typename... Args>
	inline T* Create(Args&&... args)
	{
		void* p = Allocate(sizeof(T), alignof(T));
		try
		{
			return new (p) T(std::forward<Args>(args)...);
		}
		catch (...)
		{
			Deallocate(p, alignof(T));
			throw;
		}
	}

	template<typename T>
	inline void Destroy(T* p)
	{
		if (0 != p)
		{
			p->~T();
			Deallocate(p, alignof(T));
		}
	}

	template<typename T>
	inline T* CreateArray(size_t count)
	{
		if (0 == count)
		{
			return 0;
		}

		const size_t alignment = NormalizeAlignment(alignof(T) > alignof(ArrayHeader) ? alignof(T) : alignof(ArrayHeader));
		const size_t dataOffset = ArrayDataOffset<T>();
		const size_t bytes = dataOffset + sizeof(T) * count;
		ArrayHeader* header = static_cast<ArrayHeader*>(Allocate(bytes, alignment));
		header->count = count;
		header->alignment = alignment;
		T* items = reinterpret_cast<T*>(reinterpret_cast<char*>(header) + dataOffset);

		size_t constructed = 0;
		try
		{
			for (; constructed < count; ++constructed)
			{
				new (items + constructed) T;
			}
		}
		catch (...)
		{
			while (constructed > 0)
			{
				--constructed;
				items[constructed].~T();
			}
			Deallocate(header, alignment);
			throw;
		}

		return items;
	}

	template<typename T>
	inline ArrayHeader* GetArrayHeader(T* p)
	{
		return reinterpret_cast<ArrayHeader*>(reinterpret_cast<char*>(p) - ArrayDataOffset<T>());
	}

	template<typename T>
	inline size_t ArraySizeOf(T* p)
	{
		return 0 != p ? GetArrayHeader(p)->count : 0;
	}

	template<typename T>
	inline void DestroyArray(T* p)
	{
		if (0 == p)
		{
			return;
		}

		ArrayHeader* header = GetArrayHeader(p);
		size_t count = header->count;
		while (count > 0)
		{
			--count;
			p[count].~T();
		}
		Deallocate(header, header->alignment);
	}

	template<typename T>
	class StlAllocator
	{
	public:
		using value_type = T;

		StlAllocator() noexcept
		{
		}

		template<typename U>
		StlAllocator(const StlAllocator<U>&) noexcept
		{
		}

		T* allocate(std::size_t count)
		{
			return static_cast<T*>(Allocate(sizeof(T) * count, alignof(T)));
		}

		void deallocate(T* p, std::size_t) noexcept
		{
			Deallocate(p, alignof(T));
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
	class ObserverPtr
	{
	public:
		ObserverPtr() = default;
		ObserverPtr(std::nullptr_t)
		{
		}
		ObserverPtr(T* ptr) : m_ptr(ptr)
		{
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		ObserverPtr(const ObserverPtr<U>& other) : m_ptr(other.get())
		{
		}

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
		T* m_ptr = 0;
	};

	template<>
	class ObserverPtr<void>
	{
	public:
		ObserverPtr() = default;
		ObserverPtr(std::nullptr_t)
		{
		}
		ObserverPtr(void* ptr) : m_ptr(ptr)
		{
		}

		ObserverPtr& operator=(std::nullptr_t)
		{
			m_ptr = 0;
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

		operator void*() const
		{
			return m_ptr;
		}

	private:
		void* m_ptr = 0;
	};

	template<>
	class ObserverPtr<const void>
	{
	public:
		ObserverPtr() = default;
		ObserverPtr(std::nullptr_t)
		{
		}
		ObserverPtr(const void* ptr) : m_ptr(ptr)
		{
		}

		ObserverPtr& operator=(std::nullptr_t)
		{
			m_ptr = 0;
			return *this;
		}

		ObserverPtr& operator=(const void* ptr)
		{
			m_ptr = ptr;
			return *this;
		}

		const void* get() const
		{
			return m_ptr;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

		operator const void*() const
		{
			return m_ptr;
		}

	private:
		const void* m_ptr = 0;
	};

	template<typename T>
	class ObserverArray
	{
	public:
		ObserverArray() = default;
		ObserverArray(std::nullptr_t)
		{
		}
		ObserverArray(T* ptr, size_t size) : m_ptr(ptr), m_size(size)
		{
		}

		ObserverArray& operator=(std::nullptr_t)
		{
			m_ptr = 0;
			m_size = 0;
			return *this;
		}

		T* get() const
		{
			return m_ptr;
		}

		size_t size() const
		{
			return m_size;
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
		T* m_ptr = 0;
		size_t m_size = 0;
	};

	template<typename T>
	class UniquePtr
	{
	public:
		UniquePtr() = default;
		UniquePtr(std::nullptr_t)
		{
		}
		explicit UniquePtr(T* ptr) : m_ptr(ptr)
		{
		}

		UniquePtr(const UniquePtr&) = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept : m_ptr(other.m_ptr)
		{
			other.m_ptr = 0;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				m_ptr = other.m_ptr;
				other.m_ptr = 0;
			}
			return *this;
		}

		~UniquePtr()
		{
			reset();
		}

		T* get() const
		{
			return m_ptr;
		}

		T* release()
		{
			T* ptr = m_ptr;
			m_ptr = 0;
			return ptr;
		}

		void reset(T* ptr = 0)
		{
			if (m_ptr)
			{
				if constexpr (is_object_v<T>)
				{
					static_assert(!is_object_v<T>, "UniquePtr cannot own Object-derived types.");
				}
				else
				{
					Destroy(m_ptr);
				}
			}
			m_ptr = ptr;
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
		T* m_ptr = 0;
	};

	template<typename T>
	class UniqueArray
	{
	public:
		UniqueArray() = default;
		UniqueArray(std::nullptr_t)
		{
		}
		UniqueArray(T* ptr, size_t size) : m_ptr(ptr), m_size(size)
		{
		}

		UniqueArray(const UniqueArray&) = delete;
		UniqueArray& operator=(const UniqueArray&) = delete;

		UniqueArray(UniqueArray&& other) noexcept : m_ptr(other.m_ptr), m_size(other.m_size)
		{
			other.m_ptr = 0;
			other.m_size = 0;
		}

		UniqueArray& operator=(UniqueArray&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				m_ptr = other.m_ptr;
				m_size = other.m_size;
				other.m_ptr = 0;
				other.m_size = 0;
			}
			return *this;
		}

		~UniqueArray()
		{
			reset();
		}

		T* get() const
		{
			return m_ptr;
		}

		size_t size() const
		{
			return m_size;
		}

		void reset(T* ptr = 0, size_t size = 0)
		{
			if (m_ptr)
			{
				if constexpr (is_object_v<T>)
				{
					static_assert(!is_object_v<T>, "UniqueArray cannot own Object-derived types.");
				}
				else
				{
					DestroyArray(m_ptr);
				}
			}
			m_ptr = ptr;
			m_size = size;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

	private:
		T* m_ptr = 0;
		size_t m_size = 0;
	};

	template<typename T>
	class SharedPtr
	{
	public:
		static_assert(is_object_v<T> || is_interface_v<T>, "SharedPtr target must derive from Object or Interface.");

		static SharedPtr Retain(T* ptr)
		{
			SharedPtr sharedPtr;
			sharedPtr.m_ptr = ptr;
			sharedPtr.incStrong();
			return sharedPtr;
		}

		SharedPtr() = default;
		SharedPtr(std::nullptr_t)
		{
		}
		explicit SharedPtr(T* ptr) : m_ptr(ptr)
		{
		}

		SharedPtr(const SharedPtr& other) : m_ptr(other.m_ptr)
		{
			incStrong();
		}

		SharedPtr& operator=(const SharedPtr& other)
		{
			if (this != &other)
			{
				decStrong();
				m_ptr = other.m_ptr;
				incStrong();
			}
			return *this;
		}

		SharedPtr(SharedPtr&& other) noexcept : m_ptr(other.m_ptr)
		{
			other.m_ptr = 0;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			if (this != &other)
			{
				decStrong();
				m_ptr = other.m_ptr;
				other.m_ptr = 0;
			}
			return *this;
		}

		~SharedPtr()
		{
			decStrong();
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
		void incStrong()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T>)
			{
				IncStrong(m_ptr);
			}
			else
			{
				IncSharedInterfaceStrong(SharedRootAddress(m_ptr));
			}
		}

		void decStrong()
		{
			if (0 == m_ptr)
			{
				return;
			}
			if constexpr (is_object_v<T>)
			{
				int32_t strongCount = DecStrong(m_ptr);
				if (0 == strongCount)
				{
					T::Delete(m_ptr);
				}
			}
			else
			{
				ReleaseSharedInterfaceStrong(SharedRootAddress(m_ptr));
			}
			m_ptr = 0;
		}

	private:
		T* m_ptr = 0;
	};

	template<typename T>
	class SharedArray
	{
	public:
		static_assert(!is_object_v<T>, "SharedArray cannot own Object-derived types.");

		SharedArray() = default;
		SharedArray(std::nullptr_t)
		{
		}
		SharedArray(T* ptr, size_t size) : m_ptr(ptr), m_size(size)
		{
		}

		SharedArray(const SharedArray& other) : m_ptr(other.m_ptr), m_size(other.m_size)
		{
			incStrong();
		}

		SharedArray& operator=(const SharedArray& other)
		{
			if (this != &other)
			{
				decStrong();
				m_ptr = other.m_ptr;
				m_size = other.m_size;
				incStrong();
			}
			return *this;
		}

		SharedArray(SharedArray&& other) noexcept : m_ptr(other.m_ptr), m_size(other.m_size)
		{
			other.m_ptr = 0;
			other.m_size = 0;
		}

		SharedArray& operator=(SharedArray&& other) noexcept
		{
			if (this != &other)
			{
				decStrong();
				m_ptr = other.m_ptr;
				m_size = other.m_size;
				other.m_ptr = 0;
				other.m_size = 0;
			}
			return *this;
		}

		~SharedArray()
		{
			decStrong();
		}

		T* get() const
		{
			return m_ptr;
		}

		size_t size() const
		{
			return m_size;
		}

		explicit operator bool() const
		{
			return 0 != m_ptr;
		}

	private:
		void incStrong()
		{
			if (0 == m_ptr)
			{
				return;
			}
			RefCountHeader* rc = GetRefCountFromSharedArray(m_ptr);
			if constexpr (is_atomic_object_v<T>)
			{
				AtomicInc(rc->strongCount);
			}
			else
			{
				NonAtomicInc(rc->strongCount);
			}
		}

		void decStrong()
		{
			if (0 == m_ptr)
			{
				return;
			}
			RefCountHeader* rc = GetRefCountFromSharedArray(m_ptr);
			int32_t strongCount;
			if constexpr (is_atomic_object_v<T>)
			{
				strongCount = AtomicDec(rc->strongCount);
			}
			else
			{
				strongCount = NonAtomicDec(rc->strongCount);
			}
			if (0 == strongCount)
			{
				DeleteSharedArray(m_ptr, m_size);
			}
			m_ptr = 0;
			m_size = 0;
		}

	private:
		T* m_ptr = 0;
		size_t m_size = 0;
	};

	template<typename T>
	class WeakPtr
	{
	public:
		static_assert(is_object_v<T>, "WeakPtr currently supports Object-derived types only.");

		WeakPtr() = default;
		WeakPtr(std::nullptr_t)
		{
		}
		explicit WeakPtr(T* ptr) :
			m_ptr(ptr),
			m_refCount(ptr ? GetRefCountFromSharedPtr(ptr) : 0)
		{
			incWeak();
		}

		WeakPtr(const WeakPtr& other) :
			m_ptr(other.m_ptr),
			m_refCount(other.m_refCount)
		{
			incWeak();
		}

		WeakPtr& operator=(const WeakPtr& other)
		{
			if (this != &other)
			{
				decWeak();
				m_ptr = other.m_ptr;
				m_refCount = other.m_refCount;
				incWeak();
			}
			return *this;
		}

		WeakPtr(WeakPtr&& other) noexcept :
			m_ptr(other.m_ptr),
			m_refCount(other.m_refCount)
		{
			other.m_ptr = 0;
			other.m_refCount = 0;
		}

		WeakPtr& operator=(WeakPtr&& other) noexcept
		{
			if (this != &other)
			{
				decWeak();
				m_ptr = other.m_ptr;
				m_refCount = other.m_refCount;
				other.m_ptr = 0;
				other.m_refCount = 0;
			}
			return *this;
		}

		~WeakPtr()
		{
			decWeak();
		}

		T* get() const
		{
			return m_ptr;
		}

		bool expired() const
		{
			return 0 == m_refCount || m_refCount->strongCount <= 0;
		}

		SharedPtr<T> lock() const
		{
			if (0 == m_refCount)
			{
				return SharedPtr<T>();
			}
			bool locked;
			if constexpr (is_atomic_object_v<T>)
			{
				locked = AtomicIncIfPositive(m_refCount->strongCount);
			}
			else
			{
				locked = NonAtomicIncIfPositive(m_refCount->strongCount);
			}
			return locked ? SharedPtr<T>(m_ptr) : SharedPtr<T>();
		}

		explicit operator bool() const
		{
			return !expired();
		}

	private:
		void incWeak()
		{
			if (0 == m_refCount)
			{
				return;
			}
			if constexpr (is_atomic_object_v<T>)
			{
				AtomicInc(m_refCount->weakCount);
			}
			else
			{
				NonAtomicInc(m_refCount->weakCount);
			}
		}

		void decWeak()
		{
			if (0 == m_refCount)
			{
				return;
			}
			int32_t weakCount;
			if constexpr (is_atomic_object_v<T>)
			{
				weakCount = AtomicDec(m_refCount->weakCount);
			}
			else
			{
				weakCount = NonAtomicDec(m_refCount->weakCount);
			}
			if (0 == weakCount && 0 == m_refCount->strongCount)
			{
				Deallocate(m_refCount, ObjectBlockAlignment<T>());
			}
			m_ptr = 0;
			m_refCount = 0;
		}

	private:
		T* m_ptr = 0;
		RefCountHeader* m_refCount = 0;
	};
}
