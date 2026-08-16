#include "variant.h"
#include "primitive_type.h"
#include "enum_type.h"
#include "class_type.h"
#include "object.h"
#include "debug.h"
#include "memory.h"
#include <assert.h>
#include <memory>
#include <cstring>

BEGIN_PAFCORE

namespace
{
	ErrorCode CallConstruct(StaticMethod* staticMethod, size_t address, Variant** arguments, uint32_t numArguments)
	{
		if (max_parameter_count < numArguments)
		{
			return ErrorCode::e_invalid_arg_num;
		}
		Variant* args[max_parameter_count + 1];
		Variant addr;
		addr.assignPrimitive(address);
		args[0] = &addr;
		for (uint32_t i = 0; i < numArguments; ++i)
		{
			args[i + 1] = arguments[i];
		}
		FunctionInvoker invoker = staticMethod->invoker();
		uint32_t numResults = 0;
		ErrorCode errorCode = (*invoker)(nullptr, numResults, args, numArguments + 1);
		return errorCode;
	}

	template<typename Type_t>
	ErrorCode CallConstruct(const Type_t* type, size_t address, Variant** arguments, uint32_t numArguments)
	{
		StaticMethod* staticMethod;
		if constexpr (std::is_same_v<Type_t, PrimitiveType>)
		{
			staticMethod = type->findStaticMethod("Construct");
		}
		else if constexpr (std::is_same_v<Type_t, ClassType>)
		{
			staticMethod = type->findStaticMethod("Construct", false);
		}
		else
		{
			static_assert(false);
		}
		if (nullptr == staticMethod)
		{
			return ErrorCode::e_not_implemented;
		}
		return CallConstruct(staticMethod, address, arguments, numArguments);
	}

	template<typename Type_t>
	ErrorCode CallConstructArray(const Type_t* type, size_t address, uint32_t count)
	{
		StaticMethod* staticMethod;
		if constexpr (std::is_same_v<Type_t, PrimitiveType>)
		{
			staticMethod = type->findStaticMethod("ConstructArray");
		}
		else if constexpr (std::is_same_v<Type_t, ClassType>)
		{
			staticMethod = type->findStaticMethod("ConstructArray", false);
		}
		else
		{
			static_assert(false);
		}
		if (nullptr == staticMethod)
		{
			return ErrorCode::e_not_implemented;
		}
		Variant argument;
		argument.assignPrimitive(count);
		Variant* args[1] = { &argument };
		return CallConstruct(staticMethod, address, args, 1);
	}
}

template<typename Type_t>
ErrorCode Variant::newSharedPtr_(const Type_t* type, Variant** arguments, uint32_t numArguments)
{
	clear();
	RefCountPolicy refCountPolicy = type->refCountPolicy();
	if (RefCountPolicy::unknown == refCountPolicy)
	{
		return ErrorCode::e_invalid_object_type;
	}
	void* ptr;
	if (RefCountPolicy::single_thread == refCountPolicy)
	{
		size_t size = sizeof(STRCHeader) + type->size();
		STRCHeader* header = static_cast<STRCHeader*>(Malloc(size));
		ptr = header;
		new(header)STRCHeader;
		m_pointer = header + 1;
	}
	else
	{
		size_t size = sizeof(MTRCHeader) + type->size();
		MTRCHeader* header = static_cast<MTRCHeader*>(Malloc(size));
		ptr = header;
		new(header)MTRCHeader;
		m_pointer = header + 1;
	}
	m_type = const_cast<Type_t*>(type);
	m_typeCompound = TypeCompound::shared_ptr;
	ErrorCode errorCode = CallConstruct(type, size_t(m_pointer), arguments, numArguments);
	if (ErrorCode::s_ok != errorCode)
	{
		::pafcore::Free(ptr);
		m_pointer = nullptr;
		m_type = nullptr;
		m_typeCompound = TypeCompound::none;
	}
	return errorCode;
}

template<typename Type_t>
ErrorCode Variant::newSharedArray_(const Type_t* type, uint32_t count)
{
	clear();
	size_t size = sizeof(RefCountedArrayHeader) + type->size() * count;
	RefCountedArrayHeader* header = static_cast<RefCountedArrayHeader*>(Malloc(size));
	new(header)RefCountedArrayHeader(count);
	m_pointer = header + 1;
	m_type = const_cast<Type_t*>(type);
	m_typeCompound = TypeCompound::shared_array;
	m_arraySize = count;
	ErrorCode errorCode = CallConstructArray(type, size_t(m_pointer), count);
	if (ErrorCode::s_ok != errorCode)
	{
		::pafcore::Free(header);
		m_pointer = nullptr;
		m_type = nullptr;
		m_typeCompound = TypeCompound::none;
		m_arraySize = 0;
	}
	return errorCode;
}

Variant::~Variant()
{
	clear();
}

Variant::Variant(Variant&& other)
{
	Variant::move(std::move(other));
}

Variant& Variant::operator = (Variant&& other)
{
	clear();
	move(std::move(other));
	return *this;
}

void Variant::clear()
{
	if (m_pointer)
	{
		switch (m_typeCompound)
		{
		case TypeCompound::none:
			m_type->destruct(m_pointer, 1);
			if (m_embeddedValue != m_pointer)
			{
				::pafcore::Free(m_pointer);
			}
			break;
		case TypeCompound::shared_ptr:
			m_type->decSharedPtrRefCount(m_pointer);
			break;
		case TypeCompound::shared_array:
			m_type->decSharedArrayRefCount(m_pointer);
			break;
		}
		m_pointer = nullptr;
		m_type = nullptr;
		m_typeCompound = TypeCompound::none;
		m_arraySize = 0;
	}
}

void Variant::move(Variant& other)
{
	PAF_ASSERT(nullptr == m_pointer && nullptr == m_type);
	m_pointer = other.m_pointer;
	m_type = other.m_type;
	m_typeCompound = other.m_typeCompound;
	m_arraySize = other.m_arraySize;
	if (other.m_embeddedValue == other.m_pointer)
	{
		PAF_ASSERT(TypeCompound::none == other.m_typeCompound && 0 == other.m_arraySize && (other.m_type->size() <= max_embedded_value_size));
		m_pointer = m_embeddedValue;
		if (!other.m_type->moveConstruct(m_embeddedValue, other.m_embeddedValue, 1))
		{
			other.m_type->copyConstruct(m_embeddedValue, other.m_embeddedValue, 1);
			other.m_type->destruct(other.m_embeddedValue, 1);
		}
	}
	other.m_pointer = nullptr;
}

bool Variant::subscript(Variant& var, size_t index) const
{
	if (index < m_arraySize)
	{
		PAF_ASSERT(TypeCompound::raw_array == m_typeCompound ||
			TypeCompound::shared_array == m_typeCompound ||
			TypeCompound::observer_array == m_typeCompound);
		var.clear();
		var.m_type = m_type;
		var.m_typeCompound = TypeCompound::raw_ptr;
		var.m_pointer = (byte_t*)m_pointer + index * m_type->size();
		return true;
	}
	return false;
}

ErrorCode Variant::constructPrimitive(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type && type->isPrimitive());
	clear();
	PAF_ASSERT(type->size() <= max_embedded_value_size);
	m_pointer = m_embeddedValue;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::none;
	ErrorCode errorCode = CallConstruct(static_cast<const PrimitiveType*>(type), size_t(m_pointer), arguments, numArguments);
	if (ErrorCode::s_ok != errorCode)
	{
		m_pointer = nullptr;
		m_type = nullptr;
	}
	return errorCode;
}

ErrorCode Variant::constructClass(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type && type->isClass());
	clear();
	size_t size = type->size();
	if (size <= max_embedded_value_size)
	{
		m_pointer = m_embeddedValue;
	}
	else
	{
		m_pointer = ::pafcore::Malloc(size);
	}
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::none;
	ErrorCode errorCode = CallConstruct(static_cast<const ClassType*>(type), size_t(m_pointer), arguments, numArguments);
	if (ErrorCode::s_ok != errorCode)
	{
		if (m_embeddedValue != m_pointer)
		{
			::pafcore::Free(m_pointer);
		}
		m_pointer = nullptr;
		m_type = nullptr;
	}
	return errorCode;
}

ErrorCode Variant::construct(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type);
	if (type->isPrimitive())
	{
		return constructPrimitive(type, arguments, numArguments);
	}
	else if (type->isClass())
	{
		return constructClass(type, arguments, numArguments);
	}
	else
	{
		return ErrorCode::e_not_implemented;
	}
}

ErrorCode Variant::newPrimitiveSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type && type->isPrimitive() && RefCountPolicy::single_thread == type->refCountPolicy());
	ErrorCode errorCode = newSharedPtr_(static_cast<const PrimitiveType*>(type), arguments, numArguments);
	return errorCode;
}

ErrorCode Variant::newClassSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type && type->isClass());
	ErrorCode errorCode = newSharedPtr_(static_cast<const ClassType*>(type), arguments, numArguments);
	return errorCode;
}

ErrorCode Variant::newSharedPtr(const Type* type, Variant** arguments, uint32_t numArguments)
{
	PAF_ASSERT(nullptr != type);
	if (type->isPrimitive())
	{
		return newPrimitiveSharedPtr(type, arguments, numArguments);
	}
	else if (type->isClass())
	{
		return newClassSharedPtr(type, arguments, numArguments);
	}
	else
	{
		return ErrorCode::e_not_implemented;
	}
}

ErrorCode Variant::newPrimitiveSharedArray(const Type* type, uint32_t count)
{
	PAF_ASSERT(nullptr != type && type->isPrimitive());
	ErrorCode errorCode = newSharedArray_(static_cast<const PrimitiveType*>(type), count);
	return errorCode;
}

ErrorCode Variant::newClassSharedArray(const Type* type, uint32_t count)
{
	PAF_ASSERT(nullptr != type && type->isClass());
	ErrorCode errorCode = newSharedArray_(static_cast<const ClassType*>(type), count);
	return errorCode;
}

ErrorCode Variant::newSharedArray(const Type* type, uint32_t count)
{
	PAF_ASSERT(nullptr != type);
	if (type->isPrimitive())
	{
		return newPrimitiveSharedArray(type, count);
	}
	else if (type->isClass())
	{
		return newClassSharedArray(type, count);
	}
	else
	{
		return ErrorCode::e_not_implemented;
	}
}

void Variant::assignPrimitive(const Type* type, const void* pointer)
{
	PAF_ASSERT(type->isPrimitive() && type->size() <= max_embedded_value_size);
	clear();
	m_pointer = m_embeddedValue;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::none;
	memcpy(m_pointer, pointer, type->size());
}

void Variant::assignEnum(const Type* type, const void* pointer)
{
	PAF_ASSERT(type->isEnum() && type->size() <= max_embedded_value_size);
	clear();
	m_pointer = m_embeddedValue;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::none;
	memcpy(m_pointer, pointer, type->size());
}

void Variant::assignClass(const Type* type, const void* pointer)
{
	PAF_ASSERT(type->isClass());
	clear();
	m_type = const_cast<Type*>(type);
	size_t size = type->size();
	if(size <= max_embedded_value_size)
	{
		m_pointer = m_embeddedValue;
	}
	else
	{
		m_pointer = ::pafcore::Malloc(size);
	}
	type->copyConstruct(m_pointer, pointer, 1);
	m_typeCompound = TypeCompound::none;
}

void Variant::assignValue(const Type* type, const void* pointer)
{
	if (type->isPrimitive())
	{
		assignPrimitive(type, pointer);
	}
	else if (type->isEnum())
	{
		assignEnum(type, pointer);
	}
	else if (type->isClass())
	{
		assignClass(type, pointer);
	}
	else
	{
		PAF_ASSERT(false);
	}
}

void Variant::assignRef(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::ref;
}

void Variant::assignRawPtr(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::raw_ptr;
}

void Variant::assignRawArray(const Type* type, void* src, uint32_t arraySize)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::raw_array;
	m_arraySize = arraySize;
}

void Variant::assignSharedPtr(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::shared_ptr;
}

void Variant::assignObserverPtr(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::observer_ptr;
}

void Variant::assignSharedArray(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::shared_array;
	m_arraySize = RefCountedArraySize(src);
}

void Variant::assignObserverArray(const Type* type, void* src)
{
	clear();
	m_pointer = src;
	m_type = const_cast<Type*>(type);
	m_typeCompound = TypeCompound::observer_array;
	m_arraySize = RefCountedArraySize(src);
}

bool Variant::castToPrimitive(const Type* dstType, void* dst) const
{
	PAF_ASSERT(dstType->isPrimitive());
	if(m_type->isPrimitive())
	{
		return static_cast<PrimitiveType*>(m_type)->castTo(dst, dstType, m_pointer);
	}
	else if(m_type->isEnum())
	{
		switch (m_type->size())
		{
		case 1:
			CharType::GetSingleton()->castTo(dst, dstType, m_pointer);
			return true;
		case 2:
			ShortType::GetSingleton()->castTo(dst, dstType, m_pointer);
			return true;
		case 4:
			IntType::GetSingleton()->castTo(dst, dstType, m_pointer);
			return true;
		case 8:
			LongLongType::GetSingleton()->castTo(dst, dstType, m_pointer);
			return true;
		default:
			PAF_ASSERT(false);
		}
	}
	return false;
}

bool Variant::castToEnum(const Type* dstType, void* dst) const
{
	PAF_ASSERT(dstType->isEnum());
	if (m_type->isPrimitive())
	{
		switch (dstType->size())
		{
		case 1:
			static_cast<PrimitiveType*>(m_type)->castTo(dst, CharType::GetSingleton(), m_pointer);
			return true;
		case 2:
			static_cast<PrimitiveType*>(m_type)->castTo(dst, ShortType::GetSingleton(), m_pointer);
			return true;
		case 4:
			static_cast<PrimitiveType*>(m_type)->castTo(dst, IntType::GetSingleton(), m_pointer);
			return true;
		case 8:
			static_cast<PrimitiveType*>(m_type)->castTo(dst, LongLongType::GetSingleton(), m_pointer);
			return true;
		}
	}
	else if (m_type->isEnum())
	{
		if (dstType == m_type)
		{
			memcpy(dst, m_pointer, dstType->size());
			return true;
		}
	}
	return false;
}

bool Variant::castToClass(const Type* dstType, void* dst) const
{
	PAF_ASSERT(dstType->isClass());
	void* ptr;
	if(castToClassPtr(dstType, &ptr) && ptr)
	{
		return static_cast<const ClassType*>(dstType)->copyAssign(dst, ptr, 1);
	}
	return false;
}

bool Variant::castToValue(const Type* dstType, void* dst) const
{
	if (dstType->isPrimitive())
	{
		return castToPrimitive(dstType, dst);
	}
	else if (dstType->isEnum())
	{
		return castToEnum(dstType, dst);
	}
	else if (dstType->isClass())
	{
		return castToClass(dstType, dst);
	}
	return false;
}

bool Variant::castToPrimitivePtr(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	PAF_ASSERT(dstType->isPrimitive());
	if(m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToEnumPtr(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	PAF_ASSERT(dstType->isEnum());
	if(m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToClassPtr(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	PAF_ASSERT(dstType->isClass());
	if (m_type->isClass() && m_pointer)
	{
		size_t offset;
		if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<const ClassType*>(dstType)))
		{
			*dst = (void*)((size_t)m_pointer + offset);
			return true;
		}
	}
	return false;
}


bool Variant::castToClassPtrStrict(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	PAF_ASSERT(dstType->isClass());
	if (m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToRawPtr(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	if (dstType->isPrimitive())
	{
		return castToPrimitivePtr(dstType, dst);
	}
	else if (dstType->isEnum())
	{
		return castToEnumPtr(dstType, dst);
	}
	else if (dstType->isClass())
	{
		return castToClassPtr(dstType, dst);
	}
	return false;
}

bool Variant::castToSharedPtr(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	if (TypeCompound::observer_ptr != m_typeCompound && TypeCompound::shared_ptr != m_typeCompound)
	{
		return false;
	}
	void* ptr;
	if (!castToRawPtr(dstType, &ptr))
	{
		return false;
	}
	dstType->incSharedPtrRefCount(ptr);
	*dst = ptr;
	return true;
}

bool Variant::castToSharedArray(const Type* dstType, void** dst) const
{
	PAF_ASSERT(nullptr != dstType && nullptr != dst);
	if (TypeCompound::observer_array != m_typeCompound && TypeCompound::shared_array != m_typeCompound)
	{
		return false;
	}
	void* ptr;
	if (!castToRawPtr(dstType, &ptr))
	{
		return false;
	}
	dstType->incSharedArrayRefCount(ptr);
	*dst = ptr;
	return true;
}

END_PAFCORE


