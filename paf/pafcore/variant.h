#pragma once

#include "utility.h"
#include "metadata.h"
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
	~Variant();
	Variant(Variant&& other);
	Variant& operator = (Variant&& other);
	Variant(const Variant&) = delete;
	Variant& operator = (const Variant&) = delete;
public:
	TypeCompound typeCompound() const;
	Type* type() const;
	bool isNull();
	bool byValue();
	bool byRef();
	//bool isArray();
	void clear();
	void move(Variant& var);
	bool subscript(Variant& var, size_t index);

	void assignPrimitive(Type* type, const void* pointer);
	void assignEnum(Type* type, const void* pointer);
	void assignClass(Type* type, const void* pointer);

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

	//template<typename T>
	//void assignRawPtr(const T*& dst)
	//{
	//	assignRawPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void*)dst);
	//}

	template<typename T>
	void assignRawPtr(T*& dst)
	{
		assignRawPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void*)dst);
	}

	template<typename T>
	void assignSharedPtr(const SharedPtr<T>& dst)
	{
		dst.incStrongRefCount();
		assignSharedPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
	}

	template<typename T>
	void assignSharedPtr(SharedPtr<T>&& dst)
	{
		assignSharedPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
		dst.m_ptr = nullptr;
	}

	template<typename T>
	void assignObserverPtr(const ObserverPtr<T>& dst)
	{
		assignObserverPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
	}

	template<typename T>
	void assignSharedArray(const SharedArray<T>& dst)
	{
		dst.incStrongRefCount();
		assignSharedArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
	}

	template<typename T>
	void assignSharedArray(SharedArray<T>&& dst)
	{
		assignSharedArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
		dst.m_ptr = nullptr;
	}

	template<typename T>
	void assignObserverArray(const ObserverArray<T>& dst)
	{
		assignObserverArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), dst.m_ptr);
	}

	bool castToPrimitive(Type* dstType, void* dst) const;
	bool castToEnum(Type* dstType, void* dst) const;
	bool castToClass(Type* dstType, void* dst) const;

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
		return castToPrimitive(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	template<typename T>
	bool castToClass(T& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClass(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}
	
	bool castToPrimitivePtr(Type* dstType, void** dst) const;
	bool castToEnumPtr(Type* dstType, void** dst) const;
	bool castToClassPtr(Type* dstType, void** dst) const;
	bool castToClassPtrStrict(Type* dstType, void** dst) const;

	template<typename T>
	bool castToPrimitivePtr(T*& dst) const
	{
		static_assert(is_primitive_v<T>);
		return castToPrimitivePtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	template<typename T>
	bool castToEnumPtr(T*& dst) const
	{
		static_assert(is_enum_v<T>);
		return castToEnumPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	template<typename T>
	bool castToClassPtr(T*& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClassPtr(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	template<typename T>
	bool castToClassPtrStrict(T*& dst) const
	{
		static_assert(is_class_v<T>);
		return castToClassPtrStrict(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &dst);
	}

	//bool castToPrimitivePtr(Type* dstType, void** dst) const;
	//bool castToEnumPtr(Type* dstType, void** dst) const;
	//bool castToClassPtr(Type* dstType, void** dst) const;


	//bool castToVoidPtrAllowNull(void** dst) const;
	//bool castToPrimitivePtrAllowNull(Type* dstType, void** dst) const;
	//bool castToEnumPtrAllowNull(Type* dstType, void** dst) const;
	//bool castToValuePtrAllowNull(Type* dstType, void** dst) const;
	//bool castToRcPtrAllowNull(Type* dstType, void** dst) const;

	//bool castToObjectPtr(Type* dstType, void** dst) const;
	//bool castToObject(Type* dstType, void* dst) const;

	//void reinterpretCastToPtr(Variant& var, Type* dstType) const;

	//void setTemporary();
	//bool isTemporary() const;

	//void setSubClassProxy();
	//bool isSubClassProxy() const;
	template<typename T>
	bool castToRawPtr(T*& ptr)
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
	bool castToSharedPtr(SharedPtr<T>& dst)
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
	bool castToObserverPtr(ObserverPtr<T>& dst)
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
	bool castToSharedArray(SharedPtr<T>& dst)
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
	bool castToObserverArray(ObserverPtr<T>& dst)
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
private:
	void assignRawPtr(Type* type, void* src);
	void assignObserverPtr(Type* type, void* src);
	void assignObserverArray(Type* type, void* src);
	void assignSharedPtr(Type* type, void* src);
	void assignSharedArray(Type* type, void* src);
private:
	bool destroyStorage();
public:
	Type* m_type = nullptr;
	void* m_pointer = nullptr;
	byte_t m_embeddedValue[max_embedded_value_size];
	TypeCompound m_typeCompound = TypeCompound::none;
};

//------------------------------------------------------------------------------

inline bool Variant::isNull()
{
	return nullptr == m_pointer;
}

END_PAFCORE

