#include "reflection.h"
#include "variant.h"
#include "primitive_type.h"
#include "enum_type.h"
#include "class_type.h"
#include "enum_member.h"
#include "type_alias.h"
#include "name_space.h"
#include "instance_field.h"
#include "instance_property.h"
#include "instance_method.h"
#include "static_field.h"
#include "static_property.h"
#include "static_method.h"
#include "iterator.mh"
#include "std_string.h"
#include <cstring>

BEGIN_PAFCORE

namespace
{
	template<typename Field_t>
	ErrorCode FieldGetRef(Field_t const* field, size_t fieldAddress, Variant& value)
	{
		PAF_ASSERT(nullptr != field);
		TypeCompound typeCompound = field->typeCompound();
		switch (typeCompound)
		{
		case TypeCompound::none:
			value.assignRef(field->type(), (void*)fieldAddress);
			break;
		case TypeCompound::raw_ptr:
		case TypeCompound::shared_ptr:
		case TypeCompound::observer_ptr:
			value.assignRawPtr(field->type(), *(void**)fieldAddress);
			break;
		case TypeCompound::shared_array:
		case TypeCompound::observer_array:
			value.assignRawArray(field->type(), *(void**)fieldAddress, RefCountedArraySize(*(void**)fieldAddress));
			break;
		default:
			PAF_ASSERT(false);
			return ErrorCode::e_invalid_type;
		}
		return ErrorCode::s_ok;
	}

	template<typename Field_t>
	ErrorCode FieldSet(Field_t const* field, size_t fieldAddress, Variant const& value)
	{
		PAF_ASSERT(nullptr != field);
		TypeCompound typeCompound = field->typeCompound();
		switch (typeCompound)
		{
		case TypeCompound::none:
			if (value.castToValue(field->type(), (void*)fieldAddress))
			{
				return ErrorCode::s_ok;
			}
			break;
		case TypeCompound::raw_ptr:
			if (TypeCompound::none != value.typeCompound())
			{
				if (value.castToRawPtr(field->type(), (void**)fieldAddress))
				{
					return ErrorCode::s_ok;
				}
			}
			break;
		case TypeCompound::shared_ptr:
			if (TypeCompound::shared_ptr == value.typeCompound() || TypeCompound::observer_ptr == value.typeCompound())
			{
				if (value.castToSharedPtr(field->type(), (void**)fieldAddress))
				{
					return ErrorCode::s_ok;
				}
			}
			break;
		case TypeCompound::observer_ptr:
			if (TypeCompound::shared_ptr == value.typeCompound() || TypeCompound::observer_ptr == value.typeCompound())
			{
				if (value.castToRawPtr(field->type(), (void**)fieldAddress))
				{
					return ErrorCode::s_ok;
				}
			}
			break;
		case TypeCompound::shared_array:
			if (TypeCompound::shared_array == value.typeCompound() || TypeCompound::observer_array == value.typeCompound())
			{
				if (value.castToSharedArray(field->type(), (void**)fieldAddress))
				{
					return ErrorCode::s_ok;
				}
			}
			break;
		case TypeCompound::observer_array:
			if (TypeCompound::shared_array == value.typeCompound() || TypeCompound::observer_array == value.typeCompound())
			{
				if (value.castToRawPtr(field->type(), (void**)fieldAddress))
				{
					return ErrorCode::s_ok;
				}
			}
			break;
		}
		return ErrorCode::e_invalid_field_type;
	}
}

String Reflection::GetTypeFullName(Type const* type)
{
	const char* localName = type->_name_();
	size_t totalLength = strlen(localName) + 1;
	const char* scopeNames[64];
	size_t scopeCount = 0;

	Metadata* scope = type->enclosing();
	while (scope)
	{
		Metadata* nextScope = 0;
		MetadataKind kind = scope->_kind_();
		if (MetadataKind::name_space == kind)
		{
			nextScope = static_cast<NameSpace*>(scope)->enclosing();
			if (NameSpace::GetGlobalNameSpace() == scope)
			{
				break;
			}
		}
		else
		{
			PAF_ASSERT(MetadataKind::class_type == kind);
			nextScope = static_cast<ClassType*>(scope)->enclosing();
		}
		const char* scopeName = scope->_name_();
		scopeNames[scopeCount++] = scopeName;
		totalLength += strlen(scopeName) + 1;
		scope = nextScope;
	}
	pafcore::string name;
	name.reserve(totalLength);
	for (size_t i = 0; i < scopeCount; ++i)
	{
		name.append(scopeNames[scopeCount - 1 - i]);
		name.append(".");
	}
	name.append(localName);
	return String(name.c_str());
}

String Reflection::GetTypeAliasFullName(TypeAlias const* typeAlias)
{
	const char* localName = typeAlias->_name_();
	size_t totalLength = strlen(localName) + 1;
	const char* scopeNames[64];
	size_t scopeCount = 0;

	Metadata* scope = typeAlias->m_enclosing;
	while (scope)
	{
		Metadata* nextScope = 0;
		MetadataKind kind = scope->_kind_();
		if (MetadataKind::name_space == kind)
		{
			nextScope = static_cast<NameSpace*>(scope)->enclosing();
			if (NameSpace::GetGlobalNameSpace() == scope)
			{
				break;
			}
		}
		else
		{
			PAF_ASSERT(MetadataKind::class_type == kind);
			nextScope = static_cast<ClassType const*>(scope)->enclosing();
		}
		const char* scopeName = scope->_name_();
		scopeNames[scopeCount++] = scopeName;
		totalLength += strlen(scopeName) + 1;
		scope = nextScope;
	}
	pafcore::string name;
	name.reserve(totalLength);
	for (size_t i = 0; i < scopeCount; ++i)
	{
		name.append(scopeNames[scopeCount - 1 - i]);
		name.append(".");
	}
	name.append(localName);
	return String(name.c_str());
}

Type* Reflection::GetTypeFromFullName(string_t fullName)
{
	pafcore::string name;
	const char* nameBegin = fullName;
	Metadata* metadata = NameSpace::GetGlobalNameSpace();
	while (metadata && nameBegin)
	{
		const char* dot = strchr(nameBegin, '.');
		if (dot)
		{
			name.assign(nameBegin, dot);
			nameBegin = dot + 1;
		}
		else
		{
			name.assign(nameBegin);
			nameBegin = 0;
		}
		MetadataKind kind = metadata->_kind_();
		if (MetadataKind::name_space == kind)
		{
			metadata = static_cast<NameSpace*>(metadata)->findMember(name.c_str());
		}
		else if (MetadataKind::class_type == kind)
		{
			metadata = static_cast<ClassType*>(metadata)->findNestedType(name.c_str(), true, true);
		}
		else
		{
			PAF_ASSERT(MetadataKind::primitive_type == kind || MetadataKind::enum_type == kind);
		}
	}
	if (metadata)
	{
		MetadataKind kind = metadata->_kind_();
		if (MetadataKind::name_space != kind)
		{
			PAF_ASSERT(MetadataKind::primitive_type == kind || MetadataKind::enum_type == kind || MetadataKind::class_type == kind);
			return static_cast<Type*>(metadata);
		}
	}
	return nullptr;
}

String Reflection::PrimitiveToString(const Variant& value)
{
	String res;
	char buf[64];
	PAF_ASSERT(value.type()->isPrimitive());
	PrimitiveType const* primitiveType = static_cast<PrimitiveType const*>(value.type());
	switch (primitiveType->primitiveKind())
	{
	case bool_type:
		res.assign(*reinterpret_cast<const bool*>(value.pointer()) ? "true" : "false");
		break;
	case char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const char*>(value.pointer()));
		res.assign(buf);
		break;
	case signed_char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const signed char*>(value.pointer()));
		res.assign(buf);
		break;
	case unsigned_char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const unsigned char*>(value.pointer()));
		res.assign(buf);
		break;
	case wchar_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const wchar_t*>(value.pointer()));
		res.assign(buf);
		break;
	case short_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const short*>(value.pointer()));
		res.assign(buf);
		break;
	case unsigned_short_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const unsigned short*>(value.pointer()));
		res.assign(buf);
		break;
	case int_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const int*>(value.pointer()));
		res.assign(buf);
		break;
	case unsigned_int_type:
		sprintf_s(buf, "%u", *reinterpret_cast<const unsigned int*>(value.pointer()));
		res.assign(buf);
		break;
	case long_type:
		sprintf_s(buf, "%ld", *reinterpret_cast<const long*>(value.pointer()));
		res.assign(buf);
		break;
	case unsigned_long_type:
		sprintf_s(buf, "%lu", *reinterpret_cast<const unsigned long*>(value.pointer()));
		res.assign(buf);
		break;
	case long_long_type:
		sprintf_s(buf, "%lld", *reinterpret_cast<const long long*>(value.pointer()));
		res.assign(buf);
		break;
	case unsigned_long_long_type:
		sprintf_s(buf, "%llud", *reinterpret_cast<const unsigned long long*>(value.pointer()));
		res.assign(buf);
		break;
	case float_type:
		sprintf_s(buf, "%.8g", *reinterpret_cast<const float*>(value.pointer()));
		res.assign(buf);
		break;
	case double_type:
		sprintf_s(buf, "%.16g", *reinterpret_cast<const double*>(value.pointer()));
		res.assign(buf);
		break;
	case long_double_type:
		sprintf_s(buf, "%.16g", *reinterpret_cast<const long double*>(value.pointer()));
		res.assign(buf);
		break;
	case string_type:
		res.assign(*reinterpret_cast<::string_t*>(value.pointer()));
		break;		
	}
	return res;
}

void Reflection::StringToPrimitive(Variant& value, PrimitiveType const* primitiveType, const char* str)
{
	switch (primitiveType->primitiveKind())
	{
	case bool_type: {
		bool val = (0 == strcmp(str, "true")) ? true : false;
		value.assignPrimitive(primitiveType, &val);
		break; }

	case char_type: {
		char val = (char)strtol(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case signed_char_type: {
		signed char val = (signed char)strtol(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case unsigned_char_type: {
		unsigned char val = (unsigned char)strtoul(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case wchar_type: {
		wchar_t val = (wchar_t)strtoul(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case short_type: {
		short val = (short)strtol(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case unsigned_short_type: {
		unsigned short val = (unsigned short)strtoul(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case int_type: {
		int val = strtol(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case unsigned_int_type: {
		unsigned int val = strtoul(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case long_type: {
		long val = strtol(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case unsigned_long_type: {
		unsigned long val = strtoul(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case long_long_type: {
		long long val = strtoll(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case unsigned_long_long_type: {
		unsigned long long val = strtoull(str, 0, 0);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case float_type: {
		float val = (float)atof(str);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case double_type: {
		double val = atof(str);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case long_double_type: {
		long double val = atof(str);
		value.assignPrimitive(primitiveType, &val);
		break; }

	case string_type: {
		::string_t s(str);
		value.assignPrimitive(primitiveType, &s);
		break; }
	}
}

String Reflection::EnumToString(const Variant& value)
{
	PAF_ASSERT(value.type()->isEnum());
	EnumType const* enumType = static_cast<EnumType const*>(value.type());
	int enumValue = 0;
	value.castToEnum(enumType, &enumValue);
	EnumMember* enumMember = enumType->_getEnumeratorByValue_(enumValue);
	if (enumMember)
	{
		return String(enumMember->_name_());
	}
	return String();
}

bool Reflection::StringToEnum(Variant& value, EnumType const* enumType, const char* str)
{
	EnumMember* enumMember = enumType->findEnumerator(str);
	if (enumMember)
	{
		value.assignEnum(enumType, &enumMember->m_value);
		return true;
	}
	else
	{
		int n = atoi(str);
		value.assignEnum(enumType, &n);
		return true;
	}
	return false;
}
//
//ErrorCode Reflection::NewPrimitive(Variant& result, PrimitiveType const* type, Variant** arguments, uint32_t numArguments)
//{
//	PAF_ASSERT(0 != type);
//	StaticMethod* staticMethod = type->findStaticMethod("New");
//	if (staticMethod)
//	{
//		Variant* results[1] = { &result };
//		FunctionInvoker invoker = staticMethod->invoker();
//		uint32_t numResults = 1;
//		ErrorCode errorCode = (*invoker)(results, numResults, arguments, numArguments);
//		return errorCode;
//	}
//	return ErrorCode::e_member_not_found;
//}
//
//ErrorCode Reflection::NewClass(Variant& result, ClassType const* type, Variant** arguments, uint32_t numArguments)
//{
//	PAF_ASSERT(0 != type);
//	StaticMethod* staticMethod = type->findStaticMethod("New", false);
//	if (staticMethod)
//	{
//		Variant* results[1] = { &result };
//		FunctionInvoker invoker = staticMethod->invoker();
//		uint32_t numResults = 1;
//		ErrorCode errorCode = (*invoker)(results, numResults, arguments, numArguments);
//		return errorCode;
//	}
//	return ErrorCode::e_member_not_found;
//}
//
//ErrorCode Reflection::NewPrimitiveArray(Variant& result, PrimitiveType const* type, size_t count)
//{
//	PAF_ASSERT(0 != type);
//	Variant argument;
//	argument.assignPrimitive(count);
//
//	StaticMethod* staticMethod = type->findStaticMethod("NewArray");
//	if (staticMethod)
//	{
//		Variant* results[1] = { &result };
//		Variant* arguments[1] = { &argument };
//		FunctionInvoker invoker = staticMethod->invoker();
//		uint32_t numResults = 1;
//		ErrorCode errorCode = (*invoker)(results, numResults, arguments, 1);
//		return errorCode;
//	}
//	return ErrorCode::e_member_not_found;
//}
//
//ErrorCode Reflection::NewClassArray(Variant& result, ClassType const* type, size_t count)
//{
//	PAF_ASSERT(0 != type);
//	Variant argument;
//	argument.assignPrimitive(count);
//
//	StaticMethod* staticMethod = type->findStaticMethod("NewArray", false);
//	if (staticMethod)
//	{
//		Variant* results[1] = { &result };
//		Variant* arguments[1] = { &argument };
//		FunctionInvoker invoker = staticMethod->invoker();
//		uint32_t numResults = 1;
//		ErrorCode errorCode = (*invoker)(results, numResults, arguments, 1);
//		return errorCode;
//	}
//	return ErrorCode::e_member_not_found;
//}
//
//
//ErrorCode Reflection::New(Variant& result, Type const* type, Variant** arguments, uint32_t numArguments)
//{
//	PAF_ASSERT(0 != type);
//	if (type->isPrimitive())
//	{
//		return NewPrimitive(result, static_cast<PrimitiveType const*>(type), arguments, numArguments);
//	}
//	else if (type->isClass())
//	{
//		return NewClass(result, static_cast<ClassType const*>(type), arguments, numArguments);
//	}
//	return ErrorCode::e_invalid_type;
//}
//
//ErrorCode Reflection::NewArray(Variant& result, Type const* type, size_t count)
//{
//	PAF_ASSERT(0 != type);
//	if (type->isPrimitive())
//	{
//		return NewPrimitiveArray(result, static_cast<PrimitiveType const*>(type), count);
//	}
//	else if (type->isClass())
//	{
//		return NewClassArray(result, static_cast<ClassType const*>(type), count);
//	}
//	return ErrorCode::e_invalid_type;
//}

ErrorCode Reflection::ScalarInstanceFieldGetRef(InstanceField const* field, Variant& value, Variant& that)
{
	PAF_ASSERT(nullptr != field);
	PAF_ASSERT(that.type() && that.type()->isClass());
	if (!field->isScalar())
	{
		return ErrorCode::e_field_is_not_scalar;
	}
	size_t baseOffset;
	if (!static_cast<ClassType const*>(that.type())->getClassOffset(baseOffset, field->objectType()))
	{
		return ErrorCode::e_invalid_type;
	}
	size_t fieldAddress = (size_t)that.pointer() + baseOffset + field->offset();
	return FieldGetRef(field, fieldAddress, value);
}

ErrorCode Reflection::ArrayInstanceFieldGetRef(InstanceField const* field, Variant& value, Variant& that, uint32_t index)
{
	PAF_ASSERT(nullptr != field);
	PAF_ASSERT(that.type() && that.type()->isClass());
	if (!field->isArray())
	{
		return ErrorCode::e_field_is_not_array;
	}
	if(field->arraySize() <= index)
	{
		return ErrorCode::e_index_out_of_range;
	}
	size_t baseOffset;
	if (!static_cast<ClassType const*>(that.type())->getClassOffset(baseOffset, field->objectType()))
	{
		return ErrorCode::e_invalid_type;
	}
	size_t fieldAddress = (size_t)that.pointer() + baseOffset + field->offset() + 
		(TypeCompound::none == field->typeCompound() ? index * field->type()->size() : index * sizeof(void*));
	return FieldGetRef(field, fieldAddress, value);
}

ErrorCode Reflection::ScalarInstanceFieldSet(InstanceField const* field, Variant& that, Variant const& value)
{
	PAF_ASSERT(nullptr != field);
	PAF_ASSERT(that.type() && that.type()->isClass());

	if (!field->isScalar())
	{
		return ErrorCode::e_field_is_not_scalar;
	}
	size_t baseOffset;
	if (!static_cast<ClassType const*>(that.type())->getClassOffset(baseOffset, field->objectType()))
	{
		return ErrorCode::e_invalid_object_type;
	}
	size_t fieldAddress = (size_t)that.pointer() + baseOffset + field->offset();
	return FieldSet(field, fieldAddress, value);
}

ErrorCode Reflection::ArrayInstanceFieldSet(InstanceField const* field, Variant& that, uint32_t index, Variant const& value)
{
	PAF_ASSERT(nullptr != field);
	PAF_ASSERT(that.type() && that.type()->isClass());

	if (!field->isArray())
	{
		return ErrorCode::e_field_is_not_array;
	}
	if (field->arraySize() <= index)
	{
		return ErrorCode::e_index_out_of_range;
	}
	size_t baseOffset;
	if (!static_cast<ClassType const*>(that.type())->getClassOffset(baseOffset, field->objectType()))
	{
		return ErrorCode::e_invalid_object_type;
	}
	size_t fieldAddress = (size_t)that.pointer() + baseOffset + field->offset() +
		(TypeCompound::none == field->typeCompound() ? index * field->type()->size() : index * sizeof(void*));
	return FieldSet(field, fieldAddress, value);
}

ErrorCode Reflection::ScalarStaticFieldGetRef(StaticField const* field, Variant& value)
{
	PAF_ASSERT(nullptr != field);
	if (!field->isScalar())
	{
		return ErrorCode::e_field_is_not_scalar;
	}
	size_t fieldAddress = field->address();
	return FieldGetRef(field, fieldAddress, value);
}

ErrorCode Reflection::ScalarStaticFieldSet(StaticField const* field, Variant const& value)
{
	PAF_ASSERT(nullptr != field);
	if (!field->isScalar())
	{
		return ErrorCode::e_field_is_not_scalar;
	}
	size_t fieldAddress = field->address();
	return FieldSet(field, fieldAddress, value);
}


ErrorCode Reflection::ArrayStaticFieldGetRef(StaticField const* field, Variant& value, uint32_t index)
{
	PAF_ASSERT(nullptr != field);
	if (!field->isArray())
	{
		return ErrorCode::e_field_is_not_scalar;
	}
	if (field->arraySize() <= index)
	{
		return ErrorCode::e_index_out_of_range;
	}
	size_t fieldAddress = field->address() +
		(TypeCompound::none == field->typeCompound() ? index * field->type()->size() : index * sizeof(void*));
	return FieldGetRef(field, fieldAddress, value);
}

ErrorCode Reflection::ArrayStaticFieldSet(StaticField const* field, uint32_t index, Variant const& value)
{
	PAF_ASSERT(nullptr != field);
	if (!field->isArray())
	{
		return ErrorCode::e_field_is_not_array;
	}
	if (field->arraySize() <= index)
	{
		return ErrorCode::e_index_out_of_range;
	}
	size_t fieldAddress = field->address() +
		(TypeCompound::none == field->typeCompound() ? index * field->type()->size() : index * sizeof(void*));
	return FieldSet(field, fieldAddress, value);
}

ErrorCode Reflection::InstancePropertyEnumrate(InstanceProperty const* property, Variant& candidates, Variant const& that)
{
	InstancePropertyEnumerate enumerate = property->enumerate();
	if (nullptr == enumerate)
	{
		return ErrorCode::e_property_is_not_enumerable;
	}
	ErrorCode errorCode = (*enumerate)(that, candidates);
	return errorCode;
}

ErrorCode Reflection::ScalarInstancePropertyGet(InstanceProperty const* property, Variant& value, Variant const& that)
{
	if (!property->isScalar())
	{
		return ErrorCode::e_is_not_scalar_property;
	}
	InstancePropertyScalarGet scalarGet = property->scalarGet();
	if (nullptr == scalarGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (*scalarGet)(that, value);
	return errorCode;
}

ErrorCode Reflection::ScalarInstancePropertySet(InstanceProperty const* property, Variant& that, Variant const& value)
{
	if (!property->isScalar())
	{
		return ErrorCode::e_is_not_scalar_property;
	}
	InstancePropertyScalarSet scalarSet = property->scalarSet();
	if (nullptr == scalarSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (*scalarSet)(that, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertyIterate(InstanceProperty const* property, Variant& iterator, Variant const& that)
{
	InstancePropertyCollectionIterate iterate = property->collectionIterate();
	if (nullptr == iterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	ErrorCode errorCode = (iterate)(that, iterator);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertyGet(InstanceProperty const* property, Variant& value, Variant const& that, Variant const& iterator)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	InstancePropertyCollectionGet collectionGet = property->collectionGet();
	if (nullptr == collectionGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (collectionGet)(that, iterator, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertySet(InstanceProperty const* property, Variant& that, Variant const& iterator, Variant const& value)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	InstancePropertyCollectionSet collectionSet = property->collectionSet();
	if (nullptr == collectionSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (collectionSet)(that, iterator, value);
	return errorCode;
}

ErrorCode Reflection::ArrayInstancePropertySize(InstanceProperty const* property, Variant& size, Variant const& that)
{
	if (!(property->isFixedArray() || property->isDynamicArray()))
	{
		return ErrorCode::e_is_not_array_property;
	}
	InstancePropertyArraySize arraySize = property->arraySize();
	if (nullptr == arraySize)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (arraySize)(that, size);
	return errorCode;
}

ErrorCode Reflection::ArrayInstancePropertyResize(InstanceProperty const* property, Variant& that, Variant const& size)
{
	if (!property->isDynamicArray())
	{
		return ErrorCode::e_is_not_dynamic_array_property;
	}
	InstancePropertyArrayResize arrayResize = property->arrayResize();
	if(nullptr == arrayResize)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (*arrayResize)(that, size);
	return errorCode;
}

ErrorCode Reflection::ListInstancePropertyInsert(InstanceProperty const* property, Variant& that, Variant const& iterator, Variant& value)
{
	if (!property->isList())
	{
		return ErrorCode::e_is_not_list_property;
	}
	InstancePropertyListInsert listInsert = property->listInsert();
	if (nullptr == listInsert)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (listInsert)(that, iterator, value);
	return errorCode;
}


ErrorCode Reflection::ListInstancePropertyErase(InstanceProperty const* property, Variant& that, Variant const& iterator)
{
	if (!property->isList())
	{
		return ErrorCode::e_is_not_list_property;
	}
	InstancePropertyListErase listErase = property->listErase();
	if (nullptr == listErase)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (listErase)(that, iterator);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertyGetByIndex(InstanceProperty const* property, Variant& value, Variant const& that, uint32_t index)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	InstancePropertyCollectionIterate collectionIterate = property->collectionIterate();
	if (nullptr == collectionIterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	InstancePropertyCollectionGet collectionGet = property->collectionGet();
	if (nullptr == collectionGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	Variant iterator;
	ErrorCode errorCode = (collectionIterate)(that, iterator);
	if(ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	Iterator* it;
	if(!iterator.castToClassPtr(it))
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	it->advance(index);
	if (it->isEnd())
	{
		return ErrorCode::e_index_out_of_range;
	}
	errorCode = (collectionGet)(that, iterator, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertySetByIndex(InstanceProperty const* property, Variant& that, uint32_t index, Variant const& value)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	InstancePropertyCollectionIterate collectionIterate = property->collectionIterate();
	if (nullptr == collectionIterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	InstancePropertyCollectionSet collectionSet = property->collectionSet();
	if (nullptr == collectionSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	Variant iterator;
	ErrorCode errorCode = (collectionIterate)(that, iterator);
	if (ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	Iterator* it;
	if (!iterator.castToClassPtr(it))
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	it->advance(index);
	if (it->isEnd())
	{
		return ErrorCode::e_index_out_of_range;
	}
	errorCode = (collectionSet)(that, iterator, value);
	return errorCode;
}


ErrorCode Reflection::StaticPropertyEnumrate(StaticProperty const* property, Variant& candidates)
{
	StaticPropertyEnumerate enumerate = property->enumerate();
	if (nullptr == enumerate)
	{
		return ErrorCode::e_property_is_not_enumerable;
	}
	ErrorCode errorCode = (*enumerate)(candidates);
	return errorCode;
}

ErrorCode Reflection::ScalarStaticPropertyGet(StaticProperty const* property, Variant& value)
{
	if (!property->isScalar())
	{
		return ErrorCode::e_is_not_scalar_property;
	}
	StaticPropertyScalarGet scalarGet = property->scalarGet();
	if (nullptr == scalarGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (*scalarGet)(value);
	return errorCode;
}

ErrorCode Reflection::ScalarStaticPropertySet(StaticProperty const* property, Variant& value)
{
	if (!property->isScalar())
	{
		return ErrorCode::e_is_not_scalar_property;
	}
	StaticPropertyScalarSet scalarSet = property->scalarSet();
	if (nullptr == scalarSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (*scalarSet)(value);
	return errorCode;
}

ErrorCode Reflection::CollectionStaticPropertyIterate(StaticProperty const* property, Variant& iterator)
{
	StaticPropertyCollectionIterate iterate = property->collectionIterate();
	if (nullptr == iterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	ErrorCode errorCode = (iterate)(iterator);
	return errorCode;
}

ErrorCode Reflection::CollectionStaticPropertyGet(StaticProperty const* property, Variant& value, Variant const& iterator)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	StaticPropertyCollectionGet collectionGet = property->collectionGet();
	if (nullptr == collectionGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (collectionGet)(iterator, value);
	return errorCode;
}

ErrorCode Reflection::CollectionStaticPropertySet(StaticProperty const* property, Variant const& iterator, Variant const& value)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	StaticPropertyCollectionSet collectionSet = property->collectionSet();
	if (nullptr == collectionSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (collectionSet)(iterator, value);
	return errorCode;
}

ErrorCode Reflection::ArrayStaticPropertySize(StaticProperty const* property, Variant& size)
{
	if (!(property->isFixedArray() || property->isDynamicArray()))
	{
		return ErrorCode::e_is_not_array_property;
	}
	StaticPropertyArraySize arraySize = property->arraySize();
	if (nullptr == arraySize)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	ErrorCode errorCode = (arraySize)(size);
	return errorCode;
}

ErrorCode Reflection::ArrayStaticPropertyResize(StaticProperty const* property, Variant const& size)
{
	if (!property->isDynamicArray())
	{
		return ErrorCode::e_is_not_dynamic_array_property;
	}
	StaticPropertyArrayResize arrayResize = property->arrayResize();
	if (nullptr == arrayResize)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (*arrayResize)(size);
	return errorCode;
}

ErrorCode Reflection::ListStaticPropertyInsert(StaticProperty const* property, Variant const& iterator, Variant& value)
{
	if (!property->isList())
	{
		return ErrorCode::e_is_not_list_property;
	}
	StaticPropertyListInsert listInsert = property->listInsert();
	if (nullptr == listInsert)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (listInsert)(iterator, value);
	return errorCode;
}

ErrorCode Reflection::ListStaticPropertyErase(StaticProperty const* property, Variant const& iterator)
{
	if (!property->isList())
	{
		return ErrorCode::e_is_not_list_property;
	}
	StaticPropertyListErase listErase = property->listErase();
	if (nullptr == listErase)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	ErrorCode errorCode = (listErase)(iterator);
	return errorCode;
}

ErrorCode Reflection::CollectionStaticPropertyGetByIndex(StaticProperty const* property, Variant& value, uint32_t index)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	StaticPropertyCollectionIterate collectionIterate = property->collectionIterate();
	if (nullptr == collectionIterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	StaticPropertyCollectionGet collectionGet = property->collectionGet();
	if (nullptr == collectionGet)
	{
		return ErrorCode::e_property_is_not_readable;
	}
	Variant iterator;
	ErrorCode errorCode = (collectionIterate)(iterator);
	if (ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	Iterator* it;
	if (!iterator.castToClassPtr(it))
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	it->advance(index);
	if (it->isEnd())
	{
		return ErrorCode::e_index_out_of_range;
	}
	errorCode = (collectionGet)(iterator, value);
	return errorCode;
}

ErrorCode Reflection::CollectionStaticPropertySetByIndex(StaticProperty const* property, uint32_t index, Variant const& value)
{
	if (property->isScalar())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	StaticPropertyCollectionIterate collectionIterate = property->collectionIterate();
	if (nullptr == collectionIterate)
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	StaticPropertyCollectionSet collectionSet = property->collectionSet();
	if (nullptr == collectionSet)
	{
		return ErrorCode::e_property_is_not_writable;
	}
	Variant iterator;
	ErrorCode errorCode = (collectionIterate)(iterator);
	if (ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	Iterator* it;
	if (!iterator.castToClassPtr(it))
	{
		return ErrorCode::e_property_is_not_iterable;
	}
	it->advance(index);
	if (it->isEnd())
	{
		return ErrorCode::e_index_out_of_range;
	}
	errorCode = (collectionSet)(iterator, value);
	return errorCode;
}


ErrorCode Reflection::CallInstanceMethod(InstanceMethod const* method, Variant* results, uint32_t numResults, Variant* that, Variant* arguments, uint32_t numArguments)
{
	const size_t max_param_count = 20;
	const size_t max_result_count = 20;
	Variant* ress[max_result_count + 1];
	Variant* args[max_param_count + 1];
	if (numResults > max_result_count)
	{
		numResults = max_result_count;
	}
	for (int_t i = 0; i < numResults; ++i)
	{
		ress[i] = &results[i];
	}
	if (numArguments > max_param_count)
	{
		numArguments = max_param_count;
	}
	args[0] = that;
	for (int_t i = 0; i < numArguments; ++i)
	{
		args[i + 1] = &arguments[i];
	}
	FunctionInvoker invoker = method->invoker();
	ErrorCode errorCode = (*invoker)(ress, numResults, args, numArguments + 1);
	return errorCode;
}

ErrorCode Reflection::CallStaticMethod(StaticMethod const* method, Variant* results, uint32_t numResults, Variant* arguments, uint32_t numArguments)
{
	const size_t max_param_count = 20;
	const size_t max_result_count = 20;
	Variant* ress[max_result_count + 1];
	Variant* args[max_param_count + 1];
	if (numResults > max_result_count)
	{
		numResults = max_result_count;
	}
	for (int_t i = 0; i < numResults; ++i)
	{
		ress[i] = &results[i];
	}
	if (numArguments > max_param_count)
	{
		numArguments = max_param_count;
	}
	for (int_t i = 0; i < numArguments; ++i)
	{
		args[i] = &arguments[i];
	}
	FunctionInvoker invoker = method->invoker();
	ErrorCode errorCode = (*invoker)(ress, numResults, args, numArguments + 1);
	return errorCode;
}


END_PAFCORE

