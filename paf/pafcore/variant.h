#pragma once

#include "utility.h"
#include "metadata.h"

BEGIN_PAFCORE

class Type;
class Object;
class Value;

const size_t max_embedded_value_size = 64;
const size_t max_primitive_type_size = max_embedded_value_size;
const size_t unknown_array_size = ((size_t)-1) >> 1;

class PAFCORE_EXPORT Variant
{
public:
	enum Semantic
	{
		by_value,
		by_ref,
		by_ptr,
		by_array,
	};
	enum StorageKind
	{
		storage_empty,
		storage_alias,
		storage_embedded,
		storage_owning_instance,
		storage_shared_instance,
		storage_owning_array,
	};
public:
	Variant();
	~Variant();
private:
	Variant(const Variant& var) = delete;
	Variant& operator = (const Variant& var) = delete;
private:
	static void SemanticToState(Semantic semantic, Metadata::Passing& passing, Metadata::TypeCompound& typeCompound);
	static Semantic StateToSemantic(Metadata::Passing passing, Metadata::TypeCompound typeCompound);
	bool destroyStorage();
	void setState(bool constant, Semantic semantic);
	void resetState();
	bool ownsMemory() const;
public:
	Semantic getSemantic();
	bool isNull();
	bool byValue();
	bool byRef();
	//bool isArray();
	bool isConstant();
	void clear();
	bool unhold();
	void move(Variant& var);
	bool subscript(Variant& var, size_t index);

	void assignPrimitive(Type* type, const void* pointer);
	void assignEnum(Type* type, const void* pointer);
	
	void assignVoidPtr(const void* pointer, bool constant);
	void assignPrimitivePtr(Type* type, const void* pointer, bool constant, Semantic semantic);
	void assignEnumPtr(Type* type, const void* pointer, bool constant, Semantic semantic);
	void assignValuePtr(Type* type, const void* pointer, bool constant, Semantic semantic);
	void assignRcPtr(Type* type, const void* pointer, bool constant, Semantic semantic);
	void assignRcPtr(Object* pointer, bool constant, Semantic semantic);
	void assignOwningValuePtr(Type* type, const void* pointer, bool constant);
	void assignOwningRcPtr(Type* type, const void* pointer, bool constant);
	void assignSharedPtr(Type* type, const void* pointer, bool constant);

	void assignNullPrimitive(Type* type);
	void assignNullEnum(Type* type);
	void assignNullPtr();
	void assignNullPtr(Type* type);
	
	void assignPtr(Type* type, const void* pointer, bool constant, Semantic semantic);
	void assignArray(Type* type, const void* pointer, size_t arraySize, bool constant, Semantic semantic);
	void assignOwningArray(Type* type, const void* pointer, size_t arraySize, bool constant);

	bool castToPrimitive(Type* dstType, void* dst) const;
	bool castToEnum(Type* dstType, void* dst) const;
	bool castToValue(Type* dstType, void* dst) const;
	bool castToRcObject(Type* dstType, void* dst) const;

	bool castToVoidPtr(void** dst) const;
	bool castToPrimitivePtr(Type* dstType, void** dst) const;
	bool castToEnumPtr(Type* dstType, void** dst) const;
	bool castToValuePtr(Type* dstType, void** dst) const;
	bool castToRcPtr(Type* dstType, void** dst) const;

	bool castToVoidPtrAllowNull(void** dst) const;
	bool castToPrimitivePtrAllowNull(Type* dstType, void** dst) const;
	bool castToEnumPtrAllowNull(Type* dstType, void** dst) const;
	bool castToValuePtrAllowNull(Type* dstType, void** dst) const;
	bool castToRcPtrAllowNull(Type* dstType, void** dst) const;

	bool castToObjectPtr(Type* dstType, void** dst) const;
	bool castToObject(Type* dstType, void* dst) const;

	void reinterpretCastToPtr(Variant& var, Type* dstType) const;

	void setTemporary();
	bool isTemporary() const;

	void setSubClassProxy();
	bool isSubClassProxy() const;
public:
	Type* m_type;
	void* m_pointer;
	alignas(std::max_align_t) byte_t m_embeddedValue[max_embedded_value_size];
	size_t m_arraySize;
	byte_t m_passing;
	byte_t m_typeCompound;
	byte_t m_storageKind;
	bool m_constant;
	bool m_temporary;
	bool m_subClassProxy;
};

//------------------------------------------------------------------------------
inline bool Variant::isNull()
{
	return 0 == m_pointer;
}

inline bool Variant::byValue()
{
	return (by_value == getSemantic());
}

inline bool Variant::byRef()
{
	return (by_ref == getSemantic());
}
//
inline bool Variant::isConstant()
{
	return m_constant;
}

inline Variant::Semantic Variant::getSemantic()
{
	return StateToSemantic(static_cast<Metadata::Passing>(m_passing), static_cast<Metadata::TypeCompound>(m_typeCompound));
}

inline void Variant::setTemporary()
{
	m_temporary = true;
}

inline bool Variant::isTemporary() const
{
	return m_temporary;
}

inline void Variant::setSubClassProxy()
{
	m_subClassProxy = true;
}

inline bool Variant::isSubClassProxy() const
{
	return m_subClassProxy;
}

END_PAFCORE

