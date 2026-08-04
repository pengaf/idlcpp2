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


Variant::~Variant()
{
	switch (m_typeCompound)
	{
		clear();
	}
}

void Variant::clear()
{
	if (m_pointer)
	{
		switch (m_typeCompound)
		{
		case TypeCompound::none:
			if (m_embeddedValue == m_pointer)
			{
				assert(nullptr != m_type && m_type->size() <= max_embedded_value_size);
				m_type->destruct(m_pointer, 1);
			}
			else
			{
			}
			break;
		}
	}
	m_type = nullptr;
	m_pointer = nullptr;
}

Variant::Variant(Variant&& other)
{
	m_type = other.m_type;
	if (other.m_pointer == &other.m_embeddedValue)
	{
		assert(TypeCompound::none == other.m_typeCompound && nullptr != other.m_type && other.m_type->size() <= max_embedded_value_size);
		m_pointer = m_embeddedValue;
		m_type->assign(m_pointer, other.m_pointer);
	}
	else
	{
		m_pointer = other.m_pointer;
	}
	m_typeCompound = other.m_typeCompound;
	other.clear();
}

Variant& Variant::operator = (Variant&& other)
{
	clear();
	m_type = other.m_type;
	if (other.m_pointer == &other.m_embeddedValue)
	{
		assert(nullptr != m_type && m_type->size() <= max_embedded_value_size);
		m_pointer = m_embeddedValue;
		memcpy(m_pointer, other.m_pointer, m_type->size());
	}
	else
	{
		m_pointer = other.m_pointer;
	}
	m_typeCompound = other.m_typeCompound;

	other.m_type = nullptr;
	other.m_pointer = nullptr;
	other.m_typeCompound = TypeCompound::none;
	return *this;
}

bool Variant::subscript(Variant& var, size_t index)
{
	assert(false);
	return false;
}

void Variant::assignPrimitive(Type* type, const void* pointer)
{
	assert(type->isPrimitive());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	m_typeCompound = TypeCompound::none;
	memcpy(m_pointer, pointer, type->size());
}

void Variant::assignEnum(Type* type, const void* pointer)
{
	assert(type->isEnum());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	m_typeCompound = TypeCompound::none;
	memcpy(m_pointer, pointer, type->size());
}

void Variant::assignClass(Type* type, const void* pointer)
{
	assert(type->isClass());
	clear();
	m_type = type;
	size_t size = type->size();
	if(size <= max_embedded_value_size)
	{
		m_pointer = m_embeddedValue;
	}
	else
	{
		m_pointer = Malloc(size);
	}
	m_typeCompound = TypeCompound::none;
	//type->copyConstruct(m_pointer, pointer);
	assert(false);
}

void Variant::assignRawPtr(Type* type, void* src)
{
	clear();
	m_type = type;
	m_pointer = src;
	m_typeCompound = TypeCompound::raw_ptr;
}

void Variant::assignSharedPtr(Type* type, void* src)
{
	clear();
	m_type = type;
	m_pointer = src;
	m_typeCompound = TypeCompound::shared_ptr;
}

void Variant::assignObserverPtr(Type* type, void* src)
{
	clear();
	m_type = type;
	m_pointer = src;
	m_typeCompound = TypeCompound::observer_ptr;
}

void Variant::assignSharedArray(Type* type, void* src)
{
	clear();
	m_type = type;
	m_pointer = src;
	m_typeCompound = TypeCompound::shared_array;
}

void Variant::assignObserverArray(Type* type, void* src)
{
	clear();
	m_type = type;
	m_pointer = src;
	m_typeCompound = TypeCompound::observer_array;
}

bool Variant::castToPrimitive(Type* dstType, void* dst) const
{
	assert(dstType->isPrimitive());
	if(m_type->isPrimitive())
	{
		return static_cast<PrimitiveType*>(m_type)->castTo(dst, dstType, m_pointer);
	}
	if(m_type->isEnum())
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
			assert(false);
		}
	}
	return false;
}

bool Variant::castToEnum(Type* dstType, void* dst) const
{
	assert(dstType->isEnum());
	int tmp;
	if(m_type->isPrimitive())
	{
		static_cast<PrimitiveType*>(m_type)->castTo(&tmp, IntType::GetSingleton(), m_pointer);
	}
	else if(m_type->isEnum())
	{
		switch (m_type->size())
		{
		case 1:
			CharType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
			break;
		case 2:
			ShortType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
			break;
		case 4:
			IntType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
			break;
		case 8:
			LongLongType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
			break;
		default:
			assert(false);
		}
	}
	else
	{
		return false;
	}
	switch (dstType->size())
	{
	case 1:
		IntType::GetSingleton()->castTo(dst, CharType::GetSingleton(), &tmp);
		return true;
	case 2:
		IntType::GetSingleton()->castTo(dst, ShortType::GetSingleton(), &tmp);
		return true;
	case 4:
		IntType::GetSingleton()->castTo(dst, IntType::GetSingleton(), &tmp);
		return true;
	default:
		assert(false);
	}
	return false;
}

bool Variant::castToClass(Type* dstType, void* dst) const
{
	assert(dstType->isClass());
	void* ptr;
	if(castToClassPtr(dstType, &ptr) && ptr)
	{
		static_cast<ClassType*>(dstType)->assign(dst, ptr);
		return true;
	}
	return false;
}

bool Variant::castToPrimitivePtr(Type* dstType, void** dst) const
{
	assert(dstType->isPrimitive());
	if(m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToEnumPtr(Type* dstType, void** dst) const
{
	assert(dstType->isEnum());
	if(m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToClassPtr(Type* dstType, void** dst) const
{
	assert(dstType->isClass());
	if (m_pointer)
	{
		size_t offset;
		if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
		{
			*dst = (void*)((size_t)m_pointer + offset);
			return true;
		}
	}
	return false;
}

bool Variant::castToClassPtrStrict(Type* dstType, void** dst) const
{
	assert(dstType->isClass());
	if (m_type == dstType && m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}


END_PAFCORE


