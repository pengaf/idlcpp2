#pragma once

#include "utility.h"
#include "string.h"
#include "metadata.h"

namespace pafcore
{
	class Variant;
	class Type;
	class TypeAlias;
	class PrimitiveType;
	class EnumType;
	class ClassType;
	class InstanceField;
	class InstanceProperty;
	class InstanceMethod;
	class StaticField;
	class StaticProperty;
	class StaticMethod;
	class Iterator;


	class PAFCORE_EXPORT Reflection
	{
	public:
		static ::pafcore::ClassType* GetType();

		static String GetTypeFullName(Type const* type);
		static String GetTypeAliasFullName(TypeAlias const* typeAlias);
		static Type* GetTypeFromFullName(string_t fullName);

		static String PrimitiveToString(const Variant& value);
		static void StringToPrimitive(Variant& value, PrimitiveType const* primitiveType, const char* str);
		
		static String EnumToString(const Variant& value);
		static bool StringToEnum(Variant& value, EnumType const* enumType, const char* str);
		
		static ErrorCode ScalarInstanceFieldGetRef(InstanceField const* field, Variant& value, Variant& that);
		static ErrorCode ScalarInstanceFieldSet(InstanceField const* field, Variant& that, Variant const& value);
		static ErrorCode ArrayInstanceFieldGetRef(InstanceField const* field, Variant& value, Variant& that, uint32_t index);
		static ErrorCode ArrayInstanceFieldSet(InstanceField const* field, Variant& that, uint32_t index, Variant const& value);

		static ErrorCode ScalarStaticFieldGetRef(StaticField const* field, Variant& value);
		static ErrorCode ScalarStaticFieldSet(StaticField const* field, Variant const& value);
		static ErrorCode ArrayStaticFieldGetRef(StaticField const* field, Variant& value, uint32_t index);
		static ErrorCode ArrayStaticFieldSet(StaticField const* field, uint32_t index, Variant const& value);

		//instance property
		static ErrorCode InstancePropertyEnumrate(InstanceProperty const* property, Variant& candidates, Variant const& that);
		static ErrorCode ScalarInstancePropertyGet(InstanceProperty const* property, Variant& value, Variant const& that);
		static ErrorCode ScalarInstancePropertySet(InstanceProperty const* property, Variant& that, Variant const& value);
		static ErrorCode CollectionInstancePropertyIterate(InstanceProperty const* property, Variant& iterator, Variant const& that);
		static ErrorCode CollectionInstancePropertyGet(InstanceProperty const* property, Variant& value, Variant const& that, Variant const& iterator);
		static ErrorCode CollectionInstancePropertySet(InstanceProperty const* property, Variant& that, Variant const& iterator, Variant const& value);
		static ErrorCode ArrayInstancePropertySize(InstanceProperty const* property, Variant& size, Variant const& that);
		static ErrorCode ArrayInstancePropertyResize(InstanceProperty const* property, Variant& that, Variant const& size);
		static ErrorCode ListInstancePropertyInsert(InstanceProperty const* property, Variant& that, Variant const& iterator, Variant& value);
		static ErrorCode ListInstancePropertyErase(InstanceProperty const* property, Variant& that, Variant const& iterator);

		static ErrorCode CollectionInstancePropertyGetByIndex(InstanceProperty const* property, Variant& value, Variant const& that, uint32_t index);
		static ErrorCode CollectionInstancePropertySetByIndex(InstanceProperty const* property, Variant& that, uint32_t index, Variant const& value);

		//static property
		static ErrorCode StaticPropertyEnumrate(StaticProperty const* property, Variant& candidates);
		static ErrorCode ScalarStaticPropertyGet(StaticProperty const* property, Variant& value);
		static ErrorCode ScalarStaticPropertySet(StaticProperty const* property, Variant& value);
		static ErrorCode CollectionStaticPropertyIterate(StaticProperty const* property, Variant& iterator);
		static ErrorCode CollectionStaticPropertyGet(StaticProperty const* property, Variant& value, Variant const& iterator);
		static ErrorCode CollectionStaticPropertySet(StaticProperty const* property, Variant const& iterator, Variant const& value);
		static ErrorCode ArrayStaticPropertySize(StaticProperty const* property, Variant& size);
		static ErrorCode ArrayStaticPropertyResize(StaticProperty const* property, Variant const& size);
		static ErrorCode ListStaticPropertyInsert(StaticProperty const* property, Variant const& iterator, Variant& value);
		static ErrorCode ListStaticPropertyErase(StaticProperty const* property, Variant const& iterator);

		static ErrorCode CollectionStaticPropertyGetByIndex(StaticProperty const* property, Variant& value, uint32_t index);
		static ErrorCode CollectionStaticPropertySetByIndex(StaticProperty const* property, uint32_t index, Variant const& value);

		static ErrorCode CallInstanceMethod(InstanceMethod const* method, Variant* results, uint32_t numResults, Variant* that, Variant* arguments, uint32_t numArguments);
		static ErrorCode CallStaticMethod(StaticMethod const* method, Variant* results, uint32_t numResults, Variant* arguments, uint32_t numArguments);
	};

}
