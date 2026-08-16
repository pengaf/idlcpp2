#pragma once

#include "utility.h"
#include "metadata.h"
#include "class_type.h"
#include "memory.h"

BEGIN_PAFCORE

const size_t max_embedded_value_size = 64;
const size_t max_primitive_type_size = max_embedded_value_size;

template <typename T>
constexpr bool is_primitive_v = std::is_arithmetic_v<T> || std::is_same_v<T, string_t>;

template <typename T>
constexpr bool is_class_v = std::is_class_v<T> && !is_primitive_v<T>;

template <typename T>
constexpr bool is_enum_v = std::is_enum_v<T>;

class Type;

class PAFCORE_EXPORT Variant
{
public:
	Variant() = default;
	~Variant();
	Variant(Variant&& other);
	Variant& operator = (Variant&& other);
	Variant(const Variant&) = delete;
	Variant& operator = (const Variant&) = delete;
public:
	void* pointer() const;
	Type* type() const;
	TypeCompound typeCompound() const;
	uint32_t arraySize() const;

	bool isNull() const;
	void setNull();
	void move(Variant& var);
	bool subscript(Variant& var, size_t index) const;

	//TypeCompound::none
	ErrorCode constructPrimitive(const Type* type, Variant** arguments, uint32_t numArguments);
	ErrorCode constructClass(const Type* type, Variant** arguments, uint32_t numArguments);
	ErrorCode construct(const Type* type, Variant** arguments, uint32_t numArguments);

	//TypeCompound::shared_ptr
	ErrorCode newPrimitiveSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments);
	ErrorCode newClassSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments);
	ErrorCode newSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments);

	//TypeCompound::shared_array
	ErrorCode newPrimitiveSharedArray(const Type* type, uint32_t count);
	ErrorCode newClassSharedArray(const Type* type, uint32_t count);
	ErrorCode newSharedArray(const Type* type, uint32_t count);

	void assignPrimitive(const Type* type, const void* pointer);
	void assignEnum(const Type* type, const void* pointer);
	void assignClass(const Type* type, const void* pointer);
	void assignValue(const Type* type, const void* pointer);

	void assignRef(const Type* type, void* src);
	void assignRawPtr(const Type* type, void* src);
	void assignObserverPtr(const Type* type, void* src);
	void assignRawArray(const Type* type, void* src, uint32_t arraySize);
	void assignObserverArray(const Type* type, void* src);

	bool castToPrimitive(const Type* dstType, void* dst) const;
	bool castToEnum(const Type* dstType, void* dst) const;
	bool castToClass(const Type* dstType, void* dst) const;
	bool castToValue(const Type* dstType, void* dst) const;

	bool castToPrimitivePtr(const Type* dstType, void** dst) const;
	bool castToEnumPtr(const Type* dstType, void** dst) const;
	bool castToClassPtr(const Type* dstType, void** dst) const;
	bool castToClassPtrStrict(const Type* dstType, void** dst) const;
	bool castToRawPtr(const Type* dstType, void** dst) const;
	bool castToSharedPtr(const Type* dstType, void** dst) const;
	bool castToSharedArray(const Type* dstType, void** dst) const;

	template<typename T>
	void assignPrimitive(const T& src)
	{
		static_assert(is_primitive_v<T>);
		assignPrimitive(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &src);
	}

	template<typename T>
	void assignEnum(const T& src)
	{
		static_assert(is_enum_v<T>);
		assignEnum(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &src);
	}

	template<typename T>
	void assignClass(const T& src)
	{
		static_assert(is_class_v<T>);
		assignClass(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &src);
	}

	template<typename T>
	void assignValue(const T& src)
	{
		if constexpr (is_primitive_v<T>)
		{
			assignPrimitive(src);
		}
		else if constexpr (is_enum_v<T>)
		{
			assignEnum(src);
		}
		else if constexpr (is_class_v<T>)
		{
			assignClass(src);
		}
		else
		{
			static_assert(false, "Unsupported type for assignValue.");
		}
	}

	template<typename T>
	void assignRawPtr(T* src)
	{	
		if constexpr (is_object_v<T>)
		{
			PAF_ASSERT(paf_base_offset_of(T, Object) == 0);
			assignRawPtr(src->getType(), (void*)src);
		}
		else if constexpr (is_interface_v<T>)
		{
			assignRawPtr(src->getType(), (void*)src->getAddress());
		}
		else
		{
			assignRawPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void*)src);
		}
	}

	template<typename T>
	void assignSharedPtr(const SharedPtr<T>& src)
	{
		const_cast<SharedPtr<T>&>(src).incStrongRefCount();
		if constexpr (is_object_v<T>)
		{
			PAF_ASSERT(paf_base_offset_of(T, Object) == 0);
			assignSharedPtr(src->getType(), (void*)src.m_ptr);
		}
		else if constexpr (is_interface_v<T>)
		{
			assignSharedPtr(src->getType(), (void*)src->getAddress());
		}
		else
		{
			assignSharedPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
		}
	}

	template<typename T>
	void assignSharedPtr(SharedPtr<T>&& src)
	{
		if constexpr (is_object_v<T>)
		{
			PAF_ASSERT(paf_base_offset_of(T, Object) == 0);
			assignSharedPtr(src->getType(), (void*)src.m_ptr);
		}
		else if constexpr (is_interface_v<T>)
		{
			assignSharedPtr(src->getType(), (void*)src->getAddress());
		}
		else
		{
			assignSharedPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
		}
		src.m_ptr = nullptr;
	}

	template<typename T>
	void assignObserverPtr(const ObserverPtr<T>& src)
	{
		if constexpr (is_object_v<T>)
		{
			PAF_ASSERT(paf_base_offset_of(T, Object) == 0);
			assignObserverPtr(src->getType(), (void*)src.m_ptr);
		}
		else if constexpr (is_interface_v<T>)
		{
			assignObserverPtr(src->getType(), (void*)src->getAddress());
		}
		else
		{
			assignObserverPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
		}
	}

	template<typename T>
	void assignSharedArray(const SharedArray<T>& src)
	{
		if constexpr (is_interface_v<T>)
		{
			PAF_ASSERT(RuntimeTypeOf<T>::RuntimeType::GetSingleton() == src->getType());
		}
		src.incStrongRefCount();
		assignSharedArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
	}

	template<typename T>
	void assignSharedArray(SharedArray<T>&& src)
	{
		if constexpr (is_interface_v<T>)
		{
			PAF_ASSERT(RuntimeTypeOf<T>::RuntimeType::GetSingleton() == src->getType());
		}
		assignSharedArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
		src.m_ptr = nullptr;
	}

	template<typename T>
	void assignObserverArray(const ObserverArray<T>& src)
	{
		if constexpr (is_interface_v<T>)
		{
			PAF_ASSERT(RuntimeTypeOf<T>::RuntimeType::GetSingleton() == src->getType());
		}
		assignObserverArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), src.m_ptr);
	}

	template<typename T>
	bool castToPrimitive(T& dst) const
	{
		static_assert(is_primitive_v<T>);
		return castToPrimitive(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}
	
	template<typename T>
	bool castToEnum(T& dst) const
	{
		static_assert(is_enum_v<T>);
		return castToEnum(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	template<typename T>
	bool castToClass(T& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClass(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}
	
	template<typename T>
	bool castToPrimitivePtr(T*& dst) const
	{
		static_assert(is_primitive_v<T>);
		return castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&dst);
	}

	template<typename T>
	bool castToEnumPtr(T*& dst) const
	{
		static_assert(is_enum_v<T>);
		return castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&dst);
	}

	template<typename T>
	bool castToClassPtr(T*& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClassPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&dst);
	}

	template<typename T>
	bool castToClassPtrStrict(T*& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClassPtrStrict(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&dst);
	}

	template<typename T>
	bool castToRawPtr(T*& ptr) const
	{
		if constexpr (is_primitive_v<T>)
		{
			return castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr);
		}
		else if constexpr (is_enum_v<T>)
		{
			return castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr);
		}
		else if constexpr (is_class_v<T>)
		{
			return castToClassPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr);	
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	bool castToSharedPtr(SharedPtr<T>& dst) const
	{
		if (TypeCompound::observer_ptr != m_typeCompound && TypeCompound::shared_ptr != m_typeCompound)
		{
			return false;
		}
		T* ptr;
		if constexpr (is_primitive_v<T>)
		{
			if (castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_enum_v<T>)
		{
			if (castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_class_v<T>)
		{
			if (castToClassPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool castToObserverPtr(ObserverPtr<T>& dst) const
	{
		if (TypeCompound::none == m_typeCompound)
		{
			return false;
		}
		T* ptr;
		if constexpr (is_primitive_v<T>)
		{
			if (castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_enum_v<T>)
		{
			if (castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_class_v<T>)
		{
			if (castToClassPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool castToSharedArray(SharedArray<T>& dst) const
	{
		if (TypeCompound::observer_array != m_typeCompound && TypeCompound::shared_array != m_typeCompound)
		{
			return false;
		}
		T* ptr;
		if constexpr (is_primitive_v<T>)
		{
			if (castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_enum_v<T>)
		{
			if (castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_class_v<T>)
		{
			if (castToClassPtrStrict(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool castToObserverArray(ObserverArray<T>& dst) const
	{
		if (TypeCompound::observer_array != m_typeCompound && TypeCompound::shared_array != m_typeCompound)
		{
			return false;
		}
		T* ptr;
		if constexpr (is_primitive_v<T>)
		{
			if (castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_enum_v<T>)
		{
			if (castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		else if constexpr (is_class_v<T>)
		{
			if (castToClassPtrStrict(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&ptr))
			{
				dst.assign(ptr);
				return true;
			}
		}
		return false;
	}

protected:
	void clear();
	void assignSharedPtr(const Type* type, void* src);
	void assignSharedArray(const Type* type, void* src);
private:
	template<typename Type_t>
	ErrorCode newSharedPtr_(const Type_t* type, Variant** arguments, uint32_t numArguments);
	template<typename Type_t>
	ErrorCode newSharedArray_(const Type_t* type, uint32_t count);	
protected:
	void* m_pointer{ nullptr };
	Type* m_type{ nullptr };
	TypeCompound m_typeCompound{ TypeCompound::none };
	uint32_t m_arraySize{ 0 };
	byte_t m_embeddedValue[max_embedded_value_size];//align 8 bytes
};

//------------------------------------------------------------------------------

inline void* Variant::pointer() const
{
	return m_pointer;
}

inline Type* Variant::type() const
{
	return m_type;
}

inline TypeCompound Variant::typeCompound() const
{
	return m_typeCompound;
}

inline uint32_t Variant::arraySize() const
{
	return m_arraySize;
}

inline bool Variant::isNull() const
{
	return nullptr == m_pointer;
}

inline void Variant::setNull()
{
	clear();
}

END_PAFCORE

