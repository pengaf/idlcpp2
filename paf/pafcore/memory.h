#pragma once

#include <cstddef>
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
}
