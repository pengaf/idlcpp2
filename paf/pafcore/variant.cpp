#include "variant.h"
#include "void_type.h"
#include "primitive_type.h"
#include "enum_type.h"
#include "class_type.h"
#include "object.h"
#include "debug.h"
#include "utility.h"
#include <assert.h>
#include <memory>
#include <cstring>

#if defined(_WIN32)
#include <Windows.h>
#endif

BEGIN_PAFCORE


#ifdef _DEBUG

class VariantLeakReporter
{
public:
	~VariantLeakReporter()
	{
		m_liveObjects.lock();
		for (auto& item : m_liveObjects.m_objects)
		{
			Variant* variant = item.first;
			size_t serialNumber = item.second;
			char buf[1024];
			sprintf_s(buf, "pafcore Warning: Live Variant at 0x%p, SerialNumber:%zu\n",
				variant, serialNumber);
			OutputDebugStringA(buf);
		}
		m_liveObjects.unlock();
	}
public:
	void onVariantConstruct(Variant* variant)
	{
		m_liveObjects.addPtr(variant);
	}
	void onVariantDestruct(Variant* variant)
	{
		m_liveObjects.removePtr(variant);
	}
public:
	LiveObjects<Variant> m_liveObjects;
public:
	static VariantLeakReporter* GetInstance()
	{
		static VariantLeakReporter s_instance;
		return &s_instance;
	}
};

#endif//_DEBUG

void Variant::SemanticToState(Semantic semantic, Metadata::Passing& passing, Metadata::TypeCompound& typeCompound)
{
	passing = Metadata::by_value;
	typeCompound = Metadata::tc_none;

	switch (semantic)
	{
	case by_value:
		break;
	case by_ref:
		passing = Metadata::by_ref;
		break;
	case by_ptr:
		typeCompound = Metadata::tc_observer_ptr;
		break;
	case by_array:
		typeCompound = Metadata::tc_observer_array;
		break;
	default:
		assert(false);
		break;
	}
}

Variant::Semantic Variant::StateToSemantic(Metadata::Passing passing, Metadata::TypeCompound typeCompound)
{
	if (Metadata::by_ref == passing)
	{
		return by_ref;
	}
	if (Metadata::tc_observer_array == typeCompound)
	{
		return by_array;
	}
	if (Metadata::tc_none != typeCompound)
	{
		return by_ptr;
	}
	return by_value;
}

void Variant::setState(bool constant, Semantic semantic)
{
	Metadata::Passing passing;
	Metadata::TypeCompound typeCompound;
	SemanticToState(semantic, passing, typeCompound);
	m_passing = static_cast<byte_t>(passing);
	m_typeCompound = static_cast<byte_t>(typeCompound);
	m_constant = constant;
}

void Variant::resetState()
{
	m_passing = Metadata::by_value;
	m_typeCompound = Metadata::tc_none;
	m_storageKind = storage_empty;
	m_constant = false;
	m_temporary = false;
	m_subClassProxy = false;
}

bool Variant::ownsMemory() const
{
	return storage_owning_instance == m_storageKind
		|| storage_shared_instance == m_storageKind
		|| storage_owning_array == m_storageKind
		|| storage_embedded == m_storageKind;
}

bool Variant::destroyStorage()
{
	if (0 == m_pointer)
	{
		return false;
	}

	switch (static_cast<StorageKind>(m_storageKind))
	{
	case storage_empty:
	case storage_alias:
		return false;
	case storage_embedded:
		if (m_type->isValue() || m_type->isRcObject())
		{
			if (m_subClassProxy)
			{
				PAF_ASSERT(m_type->isValue() || m_type->isRcObject());
				static_cast<ClassType*>(m_type)->destroySubclassProxy(m_pointer);
			}
			else
			{
				m_type->destroyInstance(m_pointer);
			}
		}
		return true;
	case storage_owning_instance:
		if (m_subClassProxy)
		{
			PAF_ASSERT(m_type->isValue() || m_type->isRcObject());
			static_cast<ClassType*>(m_type)->destroySubclassProxy(m_pointer);
		}
		else
		{
			m_type->destroyInstance(m_pointer);
		}
		return true;
	case storage_shared_instance:
		ReleaseSharedInterfaceStrong(m_pointer);
		return true;
	case storage_owning_array:
		m_type->destroyArray(m_pointer);
		return true;
	default:
		assert(false);
		return false;
	}
}

Variant::Variant()
{
	m_type = VoidType::GetSingleton();
	m_pointer = 0;
	m_arraySize = 0;
	resetState();

#ifdef _DEBUG
	VariantLeakReporter::GetInstance()->onVariantConstruct(this);
#endif//_DEBUG
}

Variant::~Variant()
{
	destroyStorage();

#ifdef _DEBUG
	VariantLeakReporter::GetInstance()->onVariantDestruct(this);
#endif//_DEBUG
}

void Variant::clear()
{
	destroyStorage();
	m_type = VoidType::GetSingleton();
	m_pointer = 0;
	m_arraySize = 0;
	resetState();
}

bool Variant::unhold()
{
	if (storage_owning_instance == m_storageKind)
	{
		m_storageKind = storage_alias;
		return true;
	}
	else if (storage_shared_instance == m_storageKind)
	{
		m_storageKind = storage_alias;
		return true;
	}
	else if (storage_owning_array == m_storageKind)
	{
		m_storageKind = storage_alias;
		return true;
	}
	return false;
}

void Variant::move(Variant& var)
{
	clear();
	memcpy(this, &var, sizeof(Variant));
	if(var.m_pointer == var.m_embeddedValue)
	{
		m_pointer = m_embeddedValue;
	}
	var.m_pointer = 0;
	var.m_type = VoidType::GetSingleton();
	var.m_arraySize = 0;
	var.resetState();
}

bool Variant::subscript(Variant& var, size_t index)
{
	if (by_value != getSemantic() && (0 == index || index < m_arraySize))
	{
		var.assignPtr(m_type, (void*)((size_t)m_pointer + m_type->m_size * index), m_constant, by_ref);
		//var.assignArray(m_type, (void*)((size_t)m_pointer + m_type->m_size * index), m_arraySize - index, m_constant, by_ref);
		return true;
	}
	return false;
}

void Variant::assignPrimitive(Type* type, const void* pointer)
{
	assert(type->isPrimitive());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	memcpy(m_pointer, pointer, type->m_size);
	m_storageKind = storage_embedded;
}

void Variant::assignEnum(Type* type, const void* pointer)
{
	assert(type->isEnum());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	memcpy(m_pointer, pointer, type->m_size);
	m_storageKind = storage_embedded;
}

void Variant::assignVoidPtr(const void* pointer, bool constant)
{
	clear();
	m_pointer = (void*)pointer;
	m_type = VoidType::GetSingleton();
	m_storageKind = (0 != pointer) ? storage_alias : storage_empty;
	setState(constant, by_ptr);
}

void Variant::assignPrimitivePtr(Type* type, const void* pointer, bool constant, Semantic semantic)
{
	assert(type->isPrimitive());
	clear();
	m_type = type;
	if (by_value == semantic)
	{
		m_pointer = m_embeddedValue;
		memcpy(m_pointer, pointer, type->m_size);
		m_storageKind = storage_embedded;
		setState(constant, semantic);
		return;
	}
	m_pointer = (void*)pointer;
	m_storageKind = storage_alias;
	setState(constant, semantic);
}

void Variant::assignEnumPtr(Type* type, const void* pointer, bool constant, Semantic semantic)
{
	assert(type->isEnum());
	clear();
	m_type = type;
	if (by_value == semantic)
	{
		m_pointer = m_embeddedValue;
		memcpy(m_pointer, pointer, type->m_size);
		m_storageKind = storage_embedded;
		setState(constant, semantic);
		return;
	}
	m_pointer = (void*)pointer;
	m_storageKind = storage_alias;
	setState(constant, semantic);
}

void Variant::assignValuePtr(Type* type, const void* pointer, bool constant, Semantic semantic)
{
	assert(type->isValue());
	clear();
	m_type = type;
	if (by_value == semantic)
	{
		m_pointer = (void*)pointer;
		m_storageKind = (0 != pointer) ? storage_alias : storage_empty;
		setState(constant, semantic);
		return;
	}
	m_pointer = (void*)pointer;
	m_storageKind = storage_alias;
	setState(constant, semantic);
}

void Variant::assignRcPtr(Type* type, const void* pointer, bool constant, Semantic semantic)
{
	assert(type->isRcObject());
	clear();
	if(0 != pointer)
	{
		Object* object = (Object*)pointer;
		m_type = object->getType();
		m_pointer = (void*)object;
	}
	else
	{
		m_type = type;
		m_pointer = 0;
	}
	m_storageKind = (0 != m_pointer) ? storage_alias : storage_empty;
	setState(constant, semantic);
}

void Variant::assignRcPtr(Object* pointer, bool constant, Semantic semantic)
{
	clear();
	if (0 != pointer)
	{
		m_type = pointer->getType();
		m_pointer = (void*)pointer;
	}
	else
	{
		m_type = VoidType::GetSingleton();
		m_pointer = 0;
	}
	m_storageKind = (0 != m_pointer) ? storage_alias : storage_empty;
	setState(constant, semantic);
}

void Variant::assignOwningValuePtr(Type* type, const void* pointer, bool constant)
{
	assert(type->isValue());
	clear();
	m_type = type;
	m_pointer = (void*)pointer;
	m_storageKind = (0 != pointer) ? storage_owning_instance : storage_empty;
	setState(constant, by_ptr);
}

void Variant::assignOwningRcPtr(Type* type, const void* pointer, bool constant)
{
	assert(type->isRcObject());
	clear();
	if (0 != pointer)
	{
		Object* object = (Object*)pointer;
		m_type = object->getType();
		m_pointer = (void*)object;
		m_storageKind = storage_owning_instance;
	}
	else
	{
		m_type = type;
		m_pointer = 0;
		m_storageKind = storage_empty;
	}
	setState(constant, by_ptr);
}

void Variant::assignSharedPtr(Type* type, const void* pointer, bool constant)
{
	assert(0 != type);
	assert(type->isRcObject());
	clear();
	if (0 != pointer)
	{
		Object* object = (Object*)pointer;
		m_type = object->getType();
		m_pointer = (void*)object;
		IncSharedInterfaceStrong(m_pointer);
		m_storageKind = storage_shared_instance;
	}
	else
	{
		m_type = type;
		m_pointer = 0;
		m_storageKind = storage_empty;
	}
	setState(constant, by_ptr);
}


void Variant::assignNullPrimitive(Type* type)
{
	assert(type->isPrimitive());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	std::uninitialized_fill_n(m_embeddedValue, sizeof(m_embeddedValue), 0);
	m_storageKind = storage_embedded;
}

void Variant::assignNullEnum(Type* type)
{
	assert(type->isEnum());
	clear();
	m_type = type;
	m_pointer = m_embeddedValue;
	std::uninitialized_fill_n(m_embeddedValue, sizeof(m_embeddedValue), 0);
	m_storageKind = storage_embedded;
}

void Variant::assignNullPtr()
{
	clear();
	setState(false, by_ptr);
}

void Variant::assignNullPtr(Type* type)
{
	clear();
	m_type = type;
	m_pointer = 0;
	m_storageKind = storage_empty;
	setState(false, by_ptr);
}

void Variant::assignPtr(Type* type, const void* pointer, bool constant, Semantic semantic)
{
	switch(type->m_category)
	{
	case void_object:
		assignVoidPtr(pointer, constant);
		break;
	case primitive_object:
		assignPrimitivePtr(type, pointer, constant, semantic);
		break;
	case enum_object:
		assignEnumPtr(type, pointer, constant, semantic);
		break;
	case value_object:
		assignValuePtr(type, pointer, constant, semantic);
		break;
	default:
		assignRcPtr(type, pointer, constant, semantic);
	}
}

void Variant::assignArray(Type* type, const void* pointer, size_t arraySize, bool constant, Semantic semantic)
{
	clear();
	m_type = type;
	m_pointer = (void*)pointer;
	m_arraySize = arraySize;
	m_storageKind = (0 != pointer ? storage_alias : storage_empty);
	setState(constant, semantic);
}

void Variant::assignOwningArray(Type* type, const void* pointer, size_t arraySize, bool constant)
{
	clear();
	m_type = type;
	m_pointer = (void*)pointer;
	m_arraySize = arraySize;
	m_storageKind = (0 != pointer ? storage_owning_array : storage_empty);
	setState(constant, by_array);
}

//void Variant::assignObject(Type* type, const void* pointer, bool constant, bool hold)
//{
//	switch(type->m_category)
//	{
//	case primitive_object:
//		assignPrimitive(type, pointer);
//		break;
//	case enum_object:
//		assignEnum(type, pointer);
//		break;
//	case value_object:
//		assignValuePtr(type, pointer, constant, hold);
//		break;
//	default:
//		assignRcPtr(type, pointer, constant, hold);
//	}
//}

bool Variant::castToPrimitive(Type* dstType, void* dst) const
{
	assert(dstType->isPrimitive());
	if(m_type->isPrimitive())
	{
		return static_cast<PrimitiveType*>(m_type)->castTo(dst, dstType, m_pointer);
	}
	if(m_type->isEnum())
	{
		switch (m_type->m_size)
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
		switch (m_type->m_size)
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
		default:
			assert(false);
		}
	}
	else
	{
		return false;
	}
	switch (dstType->m_size)
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

bool Variant::castToValue(Type* dstType, void* dst) const
{
	assert(dstType->isValue());
	void* ptr;
	if(castToValuePtr(dstType, &ptr) && ptr)
	{
		return dstType->assign(dst, ptr);
	}
	return false;
}

bool Variant::castToRcObject(Type* dstType, void* dst) const
{
	assert(dstType->isRcObject());
	void* ptr;
	if(castToRcPtr(dstType, &ptr) && ptr)
	{
		return dstType->assign(dst, ptr);
	}
	return false;
}

bool Variant::castToVoidPtr(void** dst) const
{
	if (0 != m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToPrimitivePtr(Type* dstType, void** dst) const
{
	assert(dstType->isPrimitive());
	if(m_type == dstType && 0 != m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToEnumPtr(Type* dstType, void** dst) const
{
	assert(dstType->isEnum());
	if(m_type == dstType && 0 != m_pointer)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToValuePtr(Type* dstType, void** dst) const
{
	assert(dstType->isValue());
	if(0 == m_pointer)
	{
		return false;
	}
	size_t offset;
	if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
	{
		*dst = (void*)((size_t)m_pointer + offset);
		return true;
	}
	return false;
}

bool Variant::castToRcPtr(Type* dstType, void** dst) const
{
	assert(dstType->isRcObject());
	if(0 == m_pointer)
	{
		return false;
	}
	if(m_type->isRcObject())
	{
		size_t offset;
		if(static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
		{
			*(size_t*)dst = (size_t)m_pointer + offset;
			return true;
		}
	}
	return false;
}


bool Variant::castToVoidPtrAllowNull(void** dst) const
{
	*dst = m_pointer;
	return true;
}

bool Variant::castToPrimitivePtrAllowNull(Type* dstType, void** dst) const
{
	if (0 == m_pointer)
	{
		*dst = 0;
		return true;
	}
	assert(dstType->isPrimitive());
	if (m_type == dstType)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToEnumPtrAllowNull(Type* dstType, void** dst) const
{
	if (0 == m_pointer)
	{
		*dst = 0;
		return true;
	}
	assert(dstType->isEnum());
	if (m_type == dstType)
	{
		*dst = m_pointer;
		return true;
	}
	return false;
}

bool Variant::castToValuePtrAllowNull(Type* dstType, void** dst) const
{
	if (0 == m_pointer)
	{
		*dst = 0;
		return true;
	}
	assert(dstType->isValue());
	if (m_type->isValue())
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

bool Variant::castToRcPtrAllowNull(Type* dstType, void** dst) const
{
	if (0 == m_pointer)
	{
		*dst = 0;
		return true;
	}
	assert(dstType->isRcObject());
	if (m_type->isRcObject())
	{
		size_t offset;
		if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
		{
			*(size_t*)dst = (size_t)m_pointer + offset;
			return true;
		}
	}
	return false;
}

bool Variant::castToObjectPtr(Type* dstType, void** dst) const
{
	if (dstType->isRcObject())
	{
		return castToRcPtr(dstType, dst);
	}
	switch (dstType->m_category)
	{
	case void_object:
		return castToVoidPtr(dst);
	case primitive_object:
		return castToPrimitivePtr(dstType, dst);
	case enum_object:
		return castToEnumPtr(dstType, dst);
	case value_object:
		return castToValuePtr(dstType, dst);
	}
	return false;
}

bool Variant::castToObject(Type* dstType, void* dst) const
{
	if (dstType->isRcObject())
	{
		return castToRcObject(dstType, dst);
	}
	switch(dstType->m_category)
	{
	case primitive_object:
		return castToPrimitive(dstType, dst);
	case enum_object:
		return castToEnum(dstType, dst);
	case value_object:
		return castToValue(dstType, dst);
	}
	return false;
}

void Variant::reinterpretCastToPtr(Variant& var, Type* dstType) const
{
	assert(var.isNull() && 0 == var.m_arraySize);
	assert(dstType && dstType->m_size);
	var.m_type = dstType;
	var.m_pointer = m_pointer;
	size_t size = m_type->m_size *(0 == m_arraySize ? 1 : m_arraySize);
	var.m_arraySize = size / dstType->m_size;
	var.setState(m_constant, by_ptr);
}

END_PAFCORE


