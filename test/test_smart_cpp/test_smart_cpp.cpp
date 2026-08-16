#include "../../paf/pafcore/class_type.h"
#include "../../paf/pafcore/enum_member.h"
#include "../../paf/pafcore/enum_type.h"
#include "../../paf/pafcore/instance_field.h"
#include "../../paf/pafcore/instance_method.h"
#include "../../paf/pafcore/instance_property.h"
#include "../../paf/pafcore/metadata.h"
#include "../../paf/pafcore/name_space.h"
#include "../../paf/pafcore/parameter.h"
#include "../../paf/pafcore/primitive_type.h"
#include "../../paf/pafcore/reflection.h"
#include "../../paf/pafcore/result.h"
#include "../../paf/pafcore/static_field.h"
#include "../../paf/pafcore/static_method.h"
#include "../../paf/pafcore/static_property.h"
#include "../../paf/pafcore/type_alias.h"
#include "../../paf/pafcore/variant.h"
#include "../test_smart_types/test_smart_types.h"
#include "../test_smart_types/test_smart_types.mh"

#include <cassert>
#include <cstring>
#include <cstdio>

namespace
{
	using namespace pafcore;
	using namespace testsmart;

	void CheckMethodSignature()
	{
		ClassType* objectType = TestObject::GetType();
		ClassType* holderType = TestHolder::GetType();
		InstanceMethod* acceptEnum = holderType->findInstanceMethod("acceptEnum", false);
		InstanceMethod* acceptObserver = holderType->findInstanceMethod("acceptObserver", false);
		InstanceMethod* maybeObserver = holderType->findInstanceMethod("maybeObserver", false);
		InstanceMethod* observe = holderType->findInstanceMethod("observe", false);
		InstanceMethod* splitPrimitive = holderType->findInstanceMethod("splitPrimitive", false);
		assert(objectType->_hasAttribute_("category"));
		assert(0 == std::strcmp(objectType->_getAttributeContentByName_("category").c_str(), "object"));
		assert(0 != acceptEnum);
		assert(acceptEnum->getResult(0, 0)->typeCompound() == TypeCompound::none);
		assert(acceptEnum->getParameter(0, 0)->typeCompound() == TypeCompound::none);
		assert(0 != acceptObserver);
		assert(acceptObserver->getResult(0, 0)->typeCompound() == TypeCompound::raw_ptr);
		assert(acceptObserver->getParameter(0, 0)->typeCompound() == TypeCompound::raw_ptr);
		assert(0 != maybeObserver);
		assert(maybeObserver->getParameter(0, 0)->allowNull());
		assert(0 != observe);
		assert(observe->getResult(0, 0)->typeCompound() == TypeCompound::observer_ptr);
		assert(observe->getParameter(0, 0)->typeCompound() == TypeCompound::shared_ptr);
		assert(0 != splitPrimitive);
		assert(splitPrimitive->maxNumResults() == 2);
		assert(splitPrimitive->getResultCount(0) == 2);
		assert(splitPrimitive->getResult(0, 0)->typeCompound() == TypeCompound::none);
		assert(splitPrimitive->getResult(0, 1)->typeCompound() == TypeCompound::none);
	}

	void CheckFieldMetadata()
	{
		ClassType* holderType = TestHolder::GetType();
		InstanceField* primitiveField = holderType->findInstanceField("primitiveField", false);
		InstanceField* signedCharField = holderType->findInstanceField("signedCharField", false);
		InstanceField* unsignedIntField = holderType->findInstanceField("unsignedIntField", false);
		InstanceField* unsignedLongLongField = holderType->findInstanceField("unsignedLongLongField", false);
		InstanceField* longDoubleField = holderType->findInstanceField("longDoubleField", false);
		InstanceField* stringField = holderType->findInstanceField("stringField", false);
		InstanceField* valueField = holderType->findInstanceField("valueField", false);
		InstanceField* largeValueField = holderType->findInstanceField("largeValueField", false);
		InstanceField* enumField = holderType->findInstanceField("enumField", false);
		InstanceField* plainEnumField = holderType->findInstanceField("plainEnumField", false);
		InstanceField* nestedField = holderType->findInstanceField("nestedField", false);
		InstanceField* nestedEnumField = holderType->findInstanceField("nestedEnumField", false);
		InstanceField* attributedField = holderType->findInstanceField("attributedField", false);
		InstanceField* u8AttributedField = holderType->findInstanceField("u8AttributedField", false);
		InstanceField* nativeField = holderType->findInstanceField("nativeField", false);
		InstanceField* noMetaField = holderType->findInstanceField("noMetaField", false);
		InstanceField* noCodeField = holderType->findInstanceField("noCodeField", false);
		InstanceField* observerField = holderType->findInstanceField("observerField", false);
		InstanceField* fixedValues = holderType->findInstanceField("fixedValues", false);
		StaticField* staticFixedValues = holderType->findStaticField("staticFixedValues", false);
		StaticField* staticPrimitiveField = holderType->findStaticField("staticPrimitiveField", false);
		InstanceProperty* fixedProperty = holderType->findInstanceProperty("fixedProperty", false);
		InstanceProperty* dynamicProperty = holderType->findInstanceProperty("dynamicProperty", false);
		InstanceProperty* listProperty = holderType->findInstanceProperty("listProperty", false);
		StaticProperty* staticRefProperty = holderType->findStaticProperty("staticRefProperty", false);

		assert(primitiveField && primitiveField->typeCompound() == TypeCompound::none);
		assert(signedCharField && signedCharField->type() == RuntimeTypeOf<signed char>::RuntimeType::GetSingleton());
		assert(unsignedIntField && unsignedIntField->type() == RuntimeTypeOf<unsigned int>::RuntimeType::GetSingleton());
		assert(unsignedLongLongField && unsignedLongLongField->type() == RuntimeTypeOf<unsigned long long>::RuntimeType::GetSingleton());
		assert(longDoubleField && longDoubleField->type() == RuntimeTypeOf<long double>::RuntimeType::GetSingleton());
		assert(stringField && stringField->type() == RuntimeTypeOf<string_t>::RuntimeType::GetSingleton());
		assert(valueField && valueField->typeCompound() == TypeCompound::none);
		assert(largeValueField && largeValueField->typeCompound() == TypeCompound::none);
		assert(enumField && enumField->typeCompound() == TypeCompound::none);
		assert(enumField->type() == RuntimeTypeOf<TestEnum>::RuntimeType::GetSingleton());
		assert(plainEnumField && plainEnumField->type() == RuntimeTypeOf<PlainEnum>::RuntimeType::GetSingleton());
		assert(nestedField && nestedField->type() == RuntimeTypeOf<TestHolder::NestedValue>::RuntimeType::GetSingleton());
		assert(nestedEnumField && nestedEnumField->type() == RuntimeTypeOf<TestHolder::NestedEnum>::RuntimeType::GetSingleton());
		assert(attributedField && attributedField->_hasAttribute_("flag"));
		assert(0 == std::strcmp(attributedField->_getAttributeContentByName_("note").c_str(), "field"));
		assert(u8AttributedField && u8AttributedField->_hasAttribute_("utf8_note"));
		assert(0 == std::strcmp(u8AttributedField->_getAttributeContentByName_("utf8_note").c_str(), "field"));
		assert(nativeField && nativeField->typeCompound() == TypeCompound::none);
		assert(0 == noMetaField);
		assert(noCodeField && noCodeField->typeCompound() == TypeCompound::none);
		assert(observerField && observerField->typeCompound() == TypeCompound::raw_ptr);
		assert(fixedValues && fixedValues->typeCompound() == TypeCompound::none);
		assert(fixedValues->isArray());
		assert(fixedValues->arraySize() == 3);
		assert(staticFixedValues && staticFixedValues->isArray());
		assert(staticFixedValues->arraySize() == 3);
		assert(staticPrimitiveField && staticPrimitiveField->typeCompound() == TypeCompound::none);
		assert(staticPrimitiveField->isScalar());
		assert(fixedProperty && fixedProperty->isFixedArray());
		assert(dynamicProperty && dynamicProperty->isDynamicArray());
		assert(listProperty && listProperty->isList());
		assert(staticRefProperty && staticRefProperty->typeCompound() == TypeCompound::none);
	}

	void CheckPropertyMetadata()
	{
		ClassType* holderType = TestHolder::GetType();
		InstanceProperty* primitiveProperty = holderType->findInstanceProperty("primitiveProperty", false);
		InstanceProperty* nativeProperty = holderType->findInstanceProperty("nativeProperty", false);
		InstanceProperty* valueProperty = holderType->findInstanceProperty("valueProperty", false);
		InstanceProperty* largeValueProperty = holderType->findInstanceProperty("largeValueProperty", false);
		InstanceProperty* enumProperty = holderType->findInstanceProperty("enumProperty", false);
		InstanceProperty* plainEnumProperty = holderType->findInstanceProperty("plainEnumProperty", false);
		InstanceProperty* observerProperty = holderType->findInstanceProperty("observerProperty", false);
		StaticProperty* staticFixedProperty = holderType->findStaticProperty("staticFixedProperty", false);
		StaticProperty* staticDynamicProperty = holderType->findStaticProperty("staticDynamicProperty", false);
		StaticProperty* staticListProperty = holderType->findStaticProperty("staticListProperty", false);
		StaticProperty* staticPrimitiveProperty = holderType->findStaticProperty("staticPrimitiveProperty", false);

		assert(primitiveProperty && primitiveProperty->typeCompound() == TypeCompound::none);
		assert(nativeProperty && nativeProperty->typeCompound() == TypeCompound::none);
		assert(valueProperty && valueProperty->typeCompound() == TypeCompound::none);
		assert(largeValueProperty && largeValueProperty->typeCompound() == TypeCompound::none);
		assert(enumProperty && enumProperty->typeCompound() == TypeCompound::none);
		assert(enumProperty->type() == RuntimeTypeOf<TestEnum>::RuntimeType::GetSingleton());
		assert(plainEnumProperty && plainEnumProperty->type() == RuntimeTypeOf<PlainEnum>::RuntimeType::GetSingleton());
		assert(observerProperty && observerProperty->typeCompound() == TypeCompound::raw_ptr);
		assert(staticFixedProperty && staticFixedProperty->isFixedArray());
		assert(staticDynamicProperty && staticDynamicProperty->isDynamicArray());
		assert(staticListProperty && staticListProperty->isList());
		assert(staticPrimitiveProperty && staticPrimitiveProperty->typeCompound() == TypeCompound::none);
		assert(staticPrimitiveProperty->isScalar());
	}

	void CheckEnumAndNestedMetadata()
	{
		ClassType* holderType = TestHolder::GetType();
		EnumType* plainEnumType = RuntimeTypeOf<PlainEnum>::RuntimeType::GetSingleton();
		EnumMember* plainZero = plainEnumType->findEnumerator("plain_zero");
		EnumMember* plainOne = plainEnumType->findEnumerator("plain_one");
		Type* nestedValue = holderType->findNestedType("NestedValue", false, true);
		Type* nestedEnum = holderType->findNestedType("NestedEnum", false, true);
		TypeAlias* nestedAlias = holderType->findNestedTypeAlias("NestedAlias", false);

		assert(plainEnumType->_hasAttribute_("label"));
		assert(0 == std::strcmp(plainEnumType->_getAttributeContentByName_("label").c_str(), "plain"));
		assert(plainEnumType->_getEnumeratorCount_() == 3);
		assert(plainZero && plainZero->_value_() == static_cast<int>(plain_zero));
		assert(plainZero->_hasAttribute_("meaning"));
		assert(0 == std::strcmp(plainZero->_getAttributeContentByName_("meaning").c_str(), "first"));
		assert(plainOne && plainEnumType->_getEnumeratorByValue_(static_cast<int>(plain_one)) == plainOne);
		assert(nestedValue == RuntimeTypeOf<TestHolder::NestedValue>::RuntimeType::GetSingleton());
		assert(nestedEnum == RuntimeTypeOf<TestHolder::NestedEnum>::RuntimeType::GetSingleton());
		assert(nestedAlias && nestedAlias->type() == RuntimeTypeOf<int>::RuntimeType::GetSingleton());
	}

	void CheckTypeAliasMetadata()
	{
		idlcpp::__testsmart__TestIntAlias_Type::GetSingleton();
		idlcpp::__testsmart__TestUnsignedLongLongAlias_Type::GetSingleton();
		idlcpp::__testsmart__TestIntBox_Type::GetSingleton();
		idlcpp::__testsmart__TestFloatBox_Type::GetSingleton();
		NameSpace* testsmartNamespace = NameSpace::GetGlobalNameSpace()->getNameSpace("testsmart");
		Metadata* member = testsmartNamespace->_findMember_("TestIntAlias");
		assert(member && member->_kind_() == MetadataKind::type_alias);
		TypeAlias* alias = static_cast<TypeAlias*>(member);
		assert(alias->type() == RuntimeTypeOf<int>::RuntimeType::GetSingleton());
		assert(0 == std::strcmp(Reflection::GetTypeAliasFullName(alias).c_str(), "testsmart.TestIntAlias"));

		Metadata* unsignedMember = testsmartNamespace->_findMember_("TestUnsignedLongLongAlias");
		assert(unsignedMember && unsignedMember->_kind_() == MetadataKind::type_alias);
		TypeAlias* unsignedAlias = static_cast<TypeAlias*>(unsignedMember);
		assert(unsignedAlias->type() == RuntimeTypeOf<unsigned long long>::RuntimeType::GetSingleton());

		Metadata* intBoxAlias = testsmartNamespace->_findMember_("TestIntBox");
		Metadata* floatBoxAlias = testsmartNamespace->_findMember_("TestFloatBox");
		assert(intBoxAlias && intBoxAlias->_kind_() == MetadataKind::type_alias);
		assert(floatBoxAlias && floatBoxAlias->_kind_() == MetadataKind::type_alias);
	}

	void CheckTemplateTypes()
	{
		using IntBox = TestBox<int>;
		using FloatBox = TestBox<float>;

		ClassType* intBoxType = RuntimeTypeOf<IntBox>::RuntimeType::GetSingleton();
		ClassType* floatBoxType = RuntimeTypeOf<FloatBox>::RuntimeType::GetSingleton();
		assert(intBoxType && floatBoxType);
		assert(0 == std::strcmp(intBoxType->_name_().c_str(), "TestBox<int>"));
		assert(0 == std::strcmp(floatBoxType->_name_().c_str(), "TestBox<float>"));

		InstanceField* intValueField = intBoxType->findInstanceField("value", false);
		InstanceProperty* intValueProperty = intBoxType->findInstanceProperty("valueProperty", false);
		InstanceMethod* intAdd = intBoxType->findInstanceMethod("add", false);
		StaticMethod* intStaticAdd = intBoxType->findStaticMethod("staticAdd", false);
		assert(intValueField && intValueField->type() == RuntimeTypeOf<int>::RuntimeType::GetSingleton());
		assert(intValueProperty && intValueProperty->type() == RuntimeTypeOf<int>::RuntimeType::GetSingleton());
		assert(intAdd && intStaticAdd);

		IntBox intBox{};
		Variant that;
		that.assignRawPtr(&intBox);

		Variant valueInput;
		valueInput.assignPrimitive(12);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(intValueField, that, valueInput));

		Variant valueRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(intValueField, valueRef, that));
		int intValue = 0;
		assert(valueRef.castToPrimitive(intValue));
		assert(intValue == 12);

		Variant propertyInput;
		propertyInput.assignPrimitive(34);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(intValueProperty, that, propertyInput));

		Variant propertyResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(intValueProperty, propertyResult, that));
		assert(propertyResult.castToPrimitive(intValue));
		assert(intValue == 34);

		Variant addArgs[1];
		addArgs[0].assignPrimitive(5);
		Variant addResult;
		assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(intAdd, &addResult, 1, &that, addArgs, 1));
		assert(addResult.castToPrimitive(intValue));
		assert(intValue == 17);

		Variant staticArgs[2];
		staticArgs[0].assignPrimitive(6);
		staticArgs[1].assignPrimitive(7);
		Variant staticResult;
		assert(ErrorCode::s_ok == Reflection::CallStaticMethod(intStaticAdd, &staticResult, 1, staticArgs, 2));
		assert(staticResult.castToPrimitive(intValue));
		assert(intValue == 13);

		FloatBox floatBox{};
		Variant floatThat;
		floatThat.assignRawPtr(&floatBox);
		InstanceField* floatValueField = floatBoxType->findInstanceField("value", false);
		InstanceProperty* floatValueProperty = floatBoxType->findInstanceProperty("valueProperty", false);
		InstanceMethod* floatAdd = floatBoxType->findInstanceMethod("add", false);
		StaticMethod* floatStaticAdd = floatBoxType->findStaticMethod("staticAdd", false);
		assert(floatValueField && floatValueField->type() == RuntimeTypeOf<float>::RuntimeType::GetSingleton());
		assert(floatValueProperty && floatValueProperty->type() == RuntimeTypeOf<float>::RuntimeType::GetSingleton());
		assert(floatAdd && floatStaticAdd);

		Variant floatInput;
		floatInput.assignPrimitive(1.5f);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(floatValueField, floatThat, floatInput));
		Variant floatAddArgs[1];
		floatAddArgs[0].assignPrimitive(2.5f);
		Variant floatAddResult;
		assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(floatAdd, &floatAddResult, 1, &floatThat, floatAddArgs, 1));
		float floatValue = 0.0f;
		assert(floatAddResult.castToPrimitive(floatValue));
		assert(floatValue == 4.0f);
	}

	void CheckVariantConstruction()
	{
		ClassType* lifeType = TestLifeCycle::GetType();
		assert(lifeType);
		assert(0 != lifeType->findStaticMethod("Construct", false));
		assert(0 != lifeType->findStaticMethod("ConstructArray", false));
		assert(0 == lifeType->findStaticMethod("New", false));
		assert(0 == lifeType->findStaticMethod("NewArray", false));

		TestLifeCycle::resetCounters();
		{
			Variant idArg;
			idArg.assignPrimitive(11);
			Variant* args[1] = { &idArg };
			Variant value;
			assert(ErrorCode::s_ok == value.construct(lifeType, args, 1));
			assert(value.type() == lifeType);
			assert(value.typeCompound() == TypeCompound::none);
			assert(TestLifeCycle::getLiveCount() == 1);
			assert(TestLifeCycle::getCreatedCount() == 1);
			assert(TestLifeCycle::getDestroyedCount() == 0);
			TestLifeCycle* object = 0;
			assert(value.castToRawPtr(object));
			assert(object->id == 11);
			assert(object->serial == 1);
		}
		assert(TestLifeCycle::getLiveCount() == 0);
		assert(TestLifeCycle::getCreatedCount() == 1);
		assert(TestLifeCycle::getDestroyedCount() == 1);

		TestLifeCycle::resetCounters();
		{
			Variant idArg;
			idArg.assignPrimitive(22);
			Variant* args[1] = { &idArg };
			Variant value;
			assert(ErrorCode::s_ok == value.newSharedPtr(lifeType, args, 1));
			assert(value.type() == lifeType);
			assert(value.typeCompound() == TypeCompound::shared_ptr);
			assert(TestLifeCycle::getLiveCount() == 1);
			assert(TestLifeCycle::getCreatedCount() == 1);
			assert(TestLifeCycle::getDestroyedCount() == 0);
			pafcore::SharedPtr<TestLifeCycle> object;
			assert(value.castToSharedPtr(object));
			assert(object->id == 22);
			assert(object->serial == 1);
			value.setNull();
			assert(TestLifeCycle::getLiveCount() == 1);
			object = nullptr;
			assert(TestLifeCycle::getLiveCount() == 0);
			assert(TestLifeCycle::getDestroyedCount() == 1);
		}
		assert(TestLifeCycle::getLiveCount() == 0);
		assert(TestLifeCycle::getCreatedCount() == 1);
		assert(TestLifeCycle::getDestroyedCount() == 1);

		TestLifeCycle::resetCounters();
		{
			Variant array;
			assert(ErrorCode::s_ok == array.newSharedArray(lifeType, 3));
			assert(array.type() == lifeType);
			assert(array.typeCompound() == TypeCompound::shared_array);
			assert(array.arraySize() == 3);
			assert(TestLifeCycle::getLiveCount() == 3);
			assert(TestLifeCycle::getCreatedCount() == 3);
			assert(TestLifeCycle::getDestroyedCount() == 0);

			Variant item;
			assert(array.subscript(item, 0));
			TestLifeCycle* first = 0;
			assert(item.castToRawPtr(first));
			assert(first->id == -1);
			assert(first->serial == 1);
			assert(array.subscript(item, 2));
			TestLifeCycle* third = 0;
			assert(item.castToRawPtr(third));
			assert(third->id == -1);
			assert(third->serial == 3);
		}
		assert(TestLifeCycle::getLiveCount() == 0);
		assert(TestLifeCycle::getCreatedCount() == 3);
		assert(TestLifeCycle::getDestroyedCount() == 3);
	}

	void CheckFields()
	{
		TestHolder* holder = TestHolder::create(0);
		assert(holder);

		ClassType* holderType = TestHolder::GetType();
		InstanceField* primitiveField = holderType->findInstanceField("primitiveField", false);
		InstanceField* valueField = holderType->findInstanceField("valueField", false);
		InstanceField* enumField = holderType->findInstanceField("enumField", false);
		InstanceField* plainEnumField = holderType->findInstanceField("plainEnumField", false);
		InstanceField* nestedField = holderType->findInstanceField("nestedField", false);
		InstanceField* nativeField = holderType->findInstanceField("nativeField", false);
		InstanceField* noCodeField = holderType->findInstanceField("noCodeField", false);
		InstanceField* observerField = holderType->findInstanceField("observerField", false);
		InstanceField* fixedValues = holderType->findInstanceField("fixedValues", false);
		InstanceProperty* fixedProperty = holderType->findInstanceProperty("fixedProperty", false);
		InstanceProperty* dynamicProperty = holderType->findInstanceProperty("dynamicProperty", false);
		InstanceProperty* listProperty = holderType->findInstanceProperty("listProperty", false);

		Variant that;
		that.assignRawPtr(holder);

		Variant primitiveValue;
		primitiveValue.assignPrimitive(123);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(primitiveField, that, primitiveValue));

		Variant primitiveRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(primitiveField, primitiveRef, that));
		assert(primitiveRef.typeCompound() == TypeCompound::ref);
		int primitiveRead = 0;
		assert(primitiveRef.castToPrimitive(primitiveRead));
		assert(primitiveRead == 123);

		TestValueSmall inputValue;
		inputValue.x = 8;
		inputValue.y = 9;
		Variant valueInput;
		valueInput.assignClass(inputValue);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(valueField, that, valueInput));

		Variant valueRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(valueField, valueRef, that));
		assert(valueRef.typeCompound() == TypeCompound::ref);
		TestValueSmall valueRead;
		assert(valueRef.castToClass(valueRead));
		assert(valueRead.x == 8);
		assert(valueRead.y == 9);

		Variant enumInput;
		enumInput.assignEnum(TestEnum::two);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(enumField, that, enumInput));

		Variant enumRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(enumField, enumRef, that));
		assert(enumRef.typeCompound() == TypeCompound::ref);
		TestEnum enumRead = TestEnum::zero;
		assert(enumRef.castToEnum(enumRead));
		assert(enumRead == TestEnum::two);

		Variant plainEnumInput;
		plainEnumInput.assignEnum(plain_two);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(plainEnumField, that, plainEnumInput));

		Variant plainEnumRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(plainEnumField, plainEnumRef, that));
		PlainEnum plainEnumRead = plain_zero;
		assert(plainEnumRef.castToEnum(plainEnumRead));
		assert(plainEnumRead == plain_two);

		TestHolder::NestedValue nestedValue;
		nestedValue.nestedNumber = 606;
		Variant nestedInput;
		nestedInput.assignClass(nestedValue);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(nestedField, that, nestedInput));

		Variant nestedRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(nestedField, nestedRef, that));
		TestHolder::NestedValue nestedRead;
		assert(nestedRef.castToClass(nestedRead));
		assert(nestedRead.nestedNumber == 606);

		Variant nativeValue;
		nativeValue.assignPrimitive(707);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(nativeField, that, nativeValue));
		Variant nativeRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(nativeField, nativeRef, that));
		int nativeRead = 0;
		assert(nativeRef.castToPrimitive(nativeRead));
		assert(nativeRead == 707);

		Variant noCodeValue;
		noCodeValue.assignPrimitive(808);
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldSet(noCodeField, that, noCodeValue));
		Variant noCodeRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(noCodeField, noCodeRef, that));
		int noCodeRead = 0;
		assert(noCodeRef.castToPrimitive(noCodeRead));
		assert(noCodeRead == 808);

		TestObject* object = TestObject::create(55);
		holder->observerField = object;

		Variant observerRef;
		assert(ErrorCode::s_ok == Reflection::ScalarInstanceFieldGetRef(observerField, observerRef, that));
		assert(holder->observerField->id == 55);

		Variant fixedInput;
		fixedInput.assignPrimitive(202);
		assert(ErrorCode::s_ok == Reflection::ArrayInstanceFieldSet(fixedValues, that, 1, fixedInput));

		Variant fixedRef;
		assert(ErrorCode::s_ok == Reflection::ArrayInstanceFieldGetRef(fixedValues, fixedRef, that, 1));
		assert(fixedRef.typeCompound() == TypeCompound::ref);
		int fixedRead = 0;
		assert(fixedRef.castToPrimitive(fixedRead));
		assert(fixedRead == 202);

		Variant propertyValue;
		propertyValue.assignPrimitive(203);
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertySetByIndex(fixedProperty, that, 1, propertyValue));

		Variant fixedPropertyRef;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(fixedProperty, fixedPropertyRef, that, 1));
		int fixedPropertyRead = 0;
		assert(fixedPropertyRef.castToPrimitive(fixedPropertyRead));
		assert(fixedPropertyRead == 203);

		Variant dynamicSize;
		assert(ErrorCode::s_ok == Reflection::ArrayInstancePropertySize(dynamicProperty, dynamicSize, that));
		size_t dynamicCount = 0;
		assert(dynamicSize.castToPrimitive(dynamicCount));
		assert(dynamicCount == 3);

		Variant resizeValue;
		resizeValue.assignPrimitive(4);
		assert(ErrorCode::s_ok == Reflection::ArrayInstancePropertyResize(dynamicProperty, that, resizeValue));
		assert(ErrorCode::s_ok == Reflection::ArrayInstancePropertySize(dynamicProperty, dynamicSize, that));
		assert(dynamicSize.castToPrimitive(dynamicCount));
		assert(dynamicCount == 4);

		Variant dynamicValue;
		dynamicValue.assignPrimitive(404);
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertySetByIndex(dynamicProperty, that, 3, dynamicValue));
		Variant dynamicRef;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(dynamicProperty, dynamicRef, that, 3));
		int dynamicRead = 0;
		assert(dynamicRef.castToPrimitive(dynamicRead));
		assert(dynamicRead == 404);

		Variant listIterator;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyIterate(listProperty, listIterator, that));
		Variant listInsertValue;
		listInsertValue.assignPrimitive(505);
		assert(ErrorCode::s_ok == Reflection::ListInstancePropertyInsert(listProperty, that, listIterator, listInsertValue));
		Variant listFirst;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(listProperty, listFirst, that, 0));
		int listRead = 0;
		assert(listFirst.castToPrimitive(listRead));
		assert(listRead == 505);

		Variant listEraseIterator;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyIterate(listProperty, listEraseIterator, that));
		assert(ErrorCode::s_ok == Reflection::ListInstancePropertyErase(listProperty, that, listEraseIterator));
		Variant listAfterErase;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(listProperty, listAfterErase, that, 0));
		assert(listAfterErase.castToPrimitive(listRead));
		assert(listRead == 40);
	}

	void CheckProperties()
	{
		TestHolder* holder = TestHolder::create(0);
		assert(holder);

		ClassType* holderType = TestHolder::GetType();
		InstanceProperty* primitiveProperty = holderType->findInstanceProperty("primitiveProperty", false);
		InstanceProperty* nativeProperty = holderType->findInstanceProperty("nativeProperty", false);
		InstanceProperty* valueProperty = holderType->findInstanceProperty("valueProperty", false);
		InstanceProperty* enumProperty = holderType->findInstanceProperty("enumProperty", false);
		InstanceProperty* plainEnumProperty = holderType->findInstanceProperty("plainEnumProperty", false);
		InstanceProperty* observerProperty = holderType->findInstanceProperty("observerProperty", false);
		InstanceProperty* fixedProperty = holderType->findInstanceProperty("fixedProperty", false);
		InstanceProperty* dynamicProperty = holderType->findInstanceProperty("dynamicProperty", false);
		InstanceProperty* listProperty = holderType->findInstanceProperty("listProperty", false);
		StaticProperty* staticRefProperty = holderType->findStaticProperty("staticRefProperty", false);

		Variant that;
		that.assignRawPtr(holder);

		Variant primitiveInput;
		primitiveInput.assignPrimitive(321);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(primitiveProperty, that, primitiveInput));

		Variant primitiveResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(primitiveProperty, primitiveResult, that));
		int primitiveRead = 0;
		assert(primitiveResult.castToPrimitive(primitiveRead));
		assert(primitiveRead == 321);

		Variant nativeInput;
		nativeInput.assignPrimitive(654);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(nativeProperty, that, nativeInput));

		Variant nativeResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(nativeProperty, nativeResult, that));
		int nativeRead = 0;
		assert(nativeResult.castToPrimitive(nativeRead));
		assert(nativeRead == 654);

		TestValueSmall value;
		value.x = 31;
		value.y = 32;
		Variant valueInput;
		valueInput.assignClass(value);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(valueProperty, that, valueInput));

		Variant valueResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(valueProperty, valueResult, that));
		TestValueSmall valueRead;
		assert(valueResult.castToClass(valueRead));
		assert(valueRead.x == 31);
		assert(valueRead.y == 32);

		Variant enumInput;
		enumInput.assignEnum(TestEnum::two);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(enumProperty, that, enumInput));

		Variant enumResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(enumProperty, enumResult, that));
		TestEnum enumRead = TestEnum::zero;
		assert(enumResult.castToEnum(enumRead));
		assert(enumRead == TestEnum::two);

		Variant plainEnumInput;
		plainEnumInput.assignEnum(plain_two);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(plainEnumProperty, that, plainEnumInput));

		Variant plainEnumResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(plainEnumProperty, plainEnumResult, that));
		PlainEnum plainEnumRead = plain_zero;
		assert(plainEnumResult.castToEnum(plainEnumRead));
		assert(plainEnumRead == plain_two);

		TestObject* object = TestObject::create(88);
		Variant observerInput;
		observerInput.assignRawPtr(object);
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertySet(observerProperty, that, observerInput));

		Variant observerResult;
		assert(ErrorCode::s_ok == Reflection::ScalarInstancePropertyGet(observerProperty, observerResult, that));
		TestObject* observerRead = 0;
		assert(observerResult.castToRawPtr(observerRead));
		assert(observerRead->id == 88);

		Variant fixedValue;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(fixedProperty, fixedValue, that, 1));
		int fixedRead = 0;
		assert(fixedValue.castToPrimitive(fixedRead));
		assert(fixedRead == 21);
		fixedValue.assignPrimitive(221);
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertySetByIndex(fixedProperty, that, 1, fixedValue));
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(fixedProperty, fixedValue, that, 1));
		assert(fixedValue.castToPrimitive(fixedRead));
		assert(fixedRead == 221);

		Variant dynamicSize;
		assert(ErrorCode::s_ok == Reflection::ArrayInstancePropertySize(dynamicProperty, dynamicSize, that));
		size_t dynamicCount = 0;
		assert(dynamicSize.castToPrimitive(dynamicCount));
		assert(dynamicCount == 3);

		Variant dynamicValue;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(dynamicProperty, dynamicValue, that, 2));
		int dynamicRead = 0;
		assert(dynamicValue.castToPrimitive(dynamicRead));
		assert(dynamicRead == 32);

		Variant listValue;
		assert(ErrorCode::s_ok == Reflection::CollectionInstancePropertyGetByIndex(listProperty, listValue, that, 1));
		int listRead = 0;
		assert(listValue.castToPrimitive(listRead));
		assert(listRead == 41);

		Variant staticRefValue;
		assert(ErrorCode::s_ok == Reflection::ScalarStaticPropertyGet(staticRefProperty, staticRefValue));
		int staticRefRead = 0;
		assert(staticRefValue.castToPrimitive(staticRefRead));
		assert(staticRefRead == 810);
		staticRefValue.assignPrimitive(811);
		assert(ErrorCode::s_ok == Reflection::ScalarStaticPropertySet(staticRefProperty, staticRefValue));
		assert(ErrorCode::s_ok == Reflection::ScalarStaticPropertyGet(staticRefProperty, staticRefValue));
		assert(staticRefValue.castToPrimitive(staticRefRead));
		assert(staticRefRead == 811);
	}

	void CheckMethods()
	{
		TestHolder* holder = TestHolder::create(0);
		assert(holder);
		ClassType* holderType = TestHolder::GetType();
		Variant that;
		that.assignRawPtr(holder);
		TestObject* observerObject = TestObject::create(66);
		Variant observeArgs[1];

		{
			InstanceMethod* method = holderType->findInstanceMethod("resetPrimitive", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 0, &that, 0, 0));
			assert(holder->primitiveField == 0);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("readPrimitive", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			int read = 0;
			assert(result.castToPrimitive(read));
			assert(read == 0);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("sumPrimitive", false);
			Variant args[1];
			args[0].assignPrimitive(7);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			int sum = 0;
			assert(result.castToPrimitive(sum));
			assert(sum == 7);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("attributedMethod", false);
			assert(method && method->_hasAttribute_("role"));
			Variant args[1];
			args[0].assignPrimitive(5);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			int read = 0;
			assert(result.castToPrimitive(read));
			assert(read == 1005);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("nativeMethod", false);
			Variant args[1];
			args[0].assignPrimitive(6);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			int read = 0;
			assert(result.castToPrimitive(read));
			assert(read == 2006);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("addValue", false);
			Variant args[1];
			TestValueSmall value;
			value.x = 5;
			value.y = 6;
			args[0].assignClass(value);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			TestValueSmall read;
			assert(result.castToClass(read));
			assert(read.x == 7);
			assert(read.y == 9);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("addLargeValue", false);
			Variant args[1];
			TestValueLarge value{};
			value.v0 = 3;
			value.v19 = 4;
			args[0].assignClass(value);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			TestValueLarge read;
			assert(result.castToClass(read));
			assert(read.v0 == 23);
			assert(read.v19 == 43);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("acceptEnum", false);
			Variant args[1];
			args[0].assignEnum(TestEnum::two);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			TestEnum read = TestEnum::zero;
			assert(result.castToEnum(read));
			assert(read == TestEnum::two);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("acceptPlainEnum", false);
			Variant args[1];
			args[0].assignEnum(plain_two);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			PlainEnum read = plain_zero;
			assert(result.castToEnum(read));
			assert(read == plain_two);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("acceptObserver", false);
			Variant args[1];
			args[0].assignRawPtr(observerObject);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			TestObject* read = 0;
			assert(result.castToRawPtr(read));
			assert(read->id == 66);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("maybeObserver", false);
			Variant args[1];
			args[0].assignRawPtr(observerObject);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, args, 1));
			TestObject* read = 0;
			assert(result.castToRawPtr(read));
			assert(read->id == 66);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("observe", false);
			pafcore::SharedPtr<TestObject> object = pafcore::MakeShared<TestObject>(99);
			const pafcore::SharedPtr<TestObject>& constObject = object;
			observeArgs[0].assignSharedPtr(constObject);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, observeArgs, 1));
			TestObject* read = 0;
			assert(result.castToRawPtr(read));
			assert(read->id == 99);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("splitPrimitive", false);
			Variant args[2];
			args[0].assignPrimitive(6);
			args[1].assignPrimitive(7);
			Variant results[2];
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, results, 2, &that, args, 2));
			int sum = 0;
			int product = 0;
			assert(results[0].castToPrimitive(sum));
			assert(results[1].castToPrimitive(product));
			assert(sum == 13);
			assert(product == 42);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makePrimitive", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			int read = 0;
			assert(result.castToPrimitive(read));
			assert(read == 107);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeValue", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			TestValueSmall read;
			assert(result.castToClass(read));
			assert(read.x == 12);
			assert(read.y == 23);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeEnum", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			TestEnum read = TestEnum::zero;
			assert(result.castToEnum(read));
			assert(read == TestEnum::two);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makePlainEnum", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			PlainEnum read = plain_zero;
			assert(result.castToEnum(read));
			assert(read == plain_two);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeObserver", false);
			Variant result;
			assert(ErrorCode::s_ok == Reflection::CallInstanceMethod(method, &result, 1, &that, 0, 0));
			TestObject* read = 0;
			assert(result.castToRawPtr(read));
			assert(read->id == 99);
		}
	}

	void CheckStaticMethods()
	{
		ClassType* holderType = TestHolder::GetType();
		StaticMethod* createMethod = holderType->findStaticMethod("create", false);
		assert(0 != createMethod);
		Variant createArgs[1];
		createArgs[0].assignPrimitive(0);
		Variant result;
		assert(ErrorCode::s_ok == Reflection::CallStaticMethod(createMethod, &result, 1, createArgs, 1));
		TestHolder* holder = 0;
		assert(result.castToRawPtr(holder));
		assert(0 != holder);

		StaticMethod* staticAdd = holderType->findStaticMethod("staticAdd", false);
		assert(0 != staticAdd);
		Variant staticArgs[2];
		staticArgs[0].assignPrimitive(20);
		staticArgs[1].assignPrimitive(22);
		Variant addResult;
		assert(ErrorCode::s_ok == Reflection::CallStaticMethod(staticAdd, &addResult, 1, staticArgs, 2));
		int sum = 0;
		assert(addResult.castToPrimitive(sum));
		assert(sum == 42);

		StaticMethod* createSharedObject = holderType->findStaticMethod("createSharedObject", false);
		assert(0 != createSharedObject);
		assert(createSharedObject->getResult(0, 0)->typeCompound() == TypeCompound::shared_ptr);
		Variant sharedArgs[1];
		sharedArgs[0].assignPrimitive(101);
		Variant sharedResult;
		assert(ErrorCode::s_ok == Reflection::CallStaticMethod(createSharedObject, &sharedResult, 1, sharedArgs, 1));
		assert(sharedResult.typeCompound() == TypeCompound::shared_ptr);
		pafcore::SharedPtr<TestObject> sharedRead;
		assert(sharedResult.castToSharedPtr(sharedRead));
		assert(sharedRead->id == 101);

		StaticMethod* staticSplitPrimitive = holderType->findStaticMethod("staticSplitPrimitive", false);
		assert(0 != staticSplitPrimitive);
		assert(staticSplitPrimitive->maxNumResults() == 2);
		assert(staticSplitPrimitive->getResultCount(0) == 2);
		Variant splitArgs[2];
		splitArgs[0].assignPrimitive(8);
		splitArgs[1].assignPrimitive(9);
		Variant splitResults[2];
		assert(ErrorCode::s_ok == Reflection::CallStaticMethod(staticSplitPrimitive, splitResults, 2, splitArgs, 2));
		int splitSum = 0;
		int splitProduct = 0;
		assert(splitResults[0].castToPrimitive(splitSum));
		assert(splitResults[1].castToPrimitive(splitProduct));
		assert(splitSum == 17);
		assert(splitProduct == 72);
	}

	void CheckStaticFieldsAndProperties()
	{
		ClassType* holderType = TestHolder::GetType();
		StaticField* staticFixedValues = holderType->findStaticField("staticFixedValues", false);
		StaticField* staticPrimitiveField = holderType->findStaticField("staticPrimitiveField", false);
		assert(0 != staticFixedValues);
		assert(0 != staticPrimitiveField);

		Variant arrayInput;
		arrayInput.assignPrimitive(904);
		assert(ErrorCode::s_ok == Reflection::ArrayStaticFieldSet(staticFixedValues, 1, arrayInput));

		Variant arrayRef;
		assert(ErrorCode::s_ok == Reflection::ArrayStaticFieldGetRef(staticFixedValues, arrayRef, 1));
		assert(arrayRef.typeCompound() == TypeCompound::ref);
		int arrayRead = 0;
		assert(arrayRef.castToPrimitive(arrayRead));
		assert(arrayRead == 904);

		Variant fieldInput;
		fieldInput.assignPrimitive(901);
		assert(ErrorCode::s_ok == Reflection::ScalarStaticFieldSet(staticPrimitiveField, fieldInput));

		Variant fieldRef;
		assert(ErrorCode::s_ok == Reflection::ScalarStaticFieldGetRef(staticPrimitiveField, fieldRef));
		assert(fieldRef.typeCompound() == TypeCompound::ref);
		int fieldRead = 0;
		assert(fieldRef.castToPrimitive(fieldRead));
		assert(fieldRead == 901);

		StaticProperty* staticPrimitiveProperty = holderType->findStaticProperty("staticPrimitiveProperty", false);
		assert(0 != staticPrimitiveProperty);

		Variant propertyInput;
		propertyInput.assignPrimitive(902);
		assert(ErrorCode::s_ok == Reflection::ScalarStaticPropertySet(staticPrimitiveProperty, propertyInput));

		Variant propertyResult;
		assert(ErrorCode::s_ok == Reflection::ScalarStaticPropertyGet(staticPrimitiveProperty, propertyResult));
		int propertyRead = 0;
		assert(propertyResult.castToPrimitive(propertyRead));
		assert(propertyRead == 902);
	}

	void CheckStaticCollectionProperties()
	{
		ClassType* holderType = TestHolder::GetType();
		StaticProperty* fixedProperty = holderType->findStaticProperty("staticFixedProperty", false);
		StaticProperty* dynamicProperty = holderType->findStaticProperty("staticDynamicProperty", false);
		StaticProperty* listProperty = holderType->findStaticProperty("staticListProperty", false);

		assert(fixedProperty && fixedProperty->isFixedArray());
		assert(dynamicProperty && dynamicProperty->isDynamicArray());
		assert(listProperty && listProperty->isList());

		Variant fixedValue;
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyGetByIndex(fixedProperty, fixedValue, 1));
		int fixedRead = 0;
		assert(fixedValue.castToPrimitive(fixedRead));
		assert(fixedRead == 821);
		fixedValue.assignPrimitive(921);
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertySetByIndex(fixedProperty, 1, fixedValue));
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyGetByIndex(fixedProperty, fixedValue, 1));
		assert(fixedValue.castToPrimitive(fixedRead));
		assert(fixedRead == 921);

		Variant dynamicSize;
		assert(ErrorCode::s_ok == Reflection::ArrayStaticPropertySize(dynamicProperty, dynamicSize));
		size_t dynamicCount = 0;
		assert(dynamicSize.castToPrimitive(dynamicCount));
		assert(dynamicCount == 3);
		dynamicSize.assignPrimitive(4);
		assert(ErrorCode::s_ok == Reflection::ArrayStaticPropertyResize(dynamicProperty, dynamicSize));
		assert(ErrorCode::s_ok == Reflection::ArrayStaticPropertySize(dynamicProperty, dynamicSize));
		assert(dynamicSize.castToPrimitive(dynamicCount));
		assert(dynamicCount == 4);

		Variant dynamicValue;
		dynamicValue.assignPrimitive(934);
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertySetByIndex(dynamicProperty, 3, dynamicValue));
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyGetByIndex(dynamicProperty, dynamicValue, 3));
		int dynamicRead = 0;
		assert(dynamicValue.castToPrimitive(dynamicRead));
		assert(dynamicRead == 934);

		Variant listIterator;
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyIterate(listProperty, listIterator));
		Variant listInsertValue;
		listInsertValue.assignPrimitive(945);
		assert(ErrorCode::s_ok == Reflection::ListStaticPropertyInsert(listProperty, listIterator, listInsertValue));
		Variant listValue;
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyGetByIndex(listProperty, listValue, 0));
		int listRead = 0;
		assert(listValue.castToPrimitive(listRead));
		assert(listRead == 945);
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyIterate(listProperty, listIterator));
		assert(ErrorCode::s_ok == Reflection::ListStaticPropertyErase(listProperty, listIterator));
		assert(ErrorCode::s_ok == Reflection::CollectionStaticPropertyGetByIndex(listProperty, listValue, 0));
		assert(listValue.castToPrimitive(listRead));
		assert(listRead == 840);
	}

	void CheckErrorPaths()
	{
		TestHolder* holder = TestHolder::create(0);
		ClassType* holderType = TestHolder::GetType();
		InstanceField* fixedValues = holderType->findInstanceField("fixedValues", false);
		InstanceProperty* primitiveProperty = holderType->findInstanceProperty("primitiveProperty", false);
		InstanceProperty* fixedProperty = holderType->findInstanceProperty("fixedProperty", false);
		InstanceProperty* dynamicProperty = holderType->findInstanceProperty("dynamicProperty", false);
		InstanceProperty* listProperty = holderType->findInstanceProperty("listProperty", false);
		StaticProperty* staticListProperty = holderType->findStaticProperty("staticListProperty", false);
		InstanceMethod* sumPrimitive = holderType->findInstanceMethod("sumPrimitive", false);
		Variant that;
		that.assignRawPtr(holder);

		Variant value;
		assert(ErrorCode::e_index_out_of_range == Reflection::ArrayInstanceFieldGetRef(fixedValues, value, that, 9));
		assert(ErrorCode::e_index_out_of_range == Reflection::CollectionInstancePropertyGetByIndex(fixedProperty, value, that, 9));
		assert(ErrorCode::e_is_not_collection_property == Reflection::CollectionInstancePropertyGetByIndex(primitiveProperty, value, that, 0));
		assert(ErrorCode::e_is_not_dynamic_array_property == Reflection::ArrayInstancePropertyResize(fixedProperty, that, value));
		assert(ErrorCode::e_is_not_array_property == Reflection::ArrayInstancePropertySize(listProperty, value, that));
		assert(ErrorCode::e_is_not_array_property == Reflection::ArrayStaticPropertySize(staticListProperty, value));
		assert(ErrorCode::e_invalid_arg_num == Reflection::CallInstanceMethod(sumPrimitive, &value, 1, &that, 0, 0));

		Variant invalidIndex;
		invalidIndex.assignPrimitive(99);
		assert(ErrorCode::e_index_out_of_range == Reflection::CollectionInstancePropertySetByIndex(dynamicProperty, that, 99, invalidIndex));
	}
}

int main()
{
	std::puts("CheckMethodSignature");
	std::fflush(stdout);
	CheckMethodSignature();
	std::puts("CheckFieldMetadata");
	std::fflush(stdout);
	CheckFieldMetadata();
	std::puts("CheckPropertyMetadata");
	std::fflush(stdout);
	CheckPropertyMetadata();
	std::puts("CheckTypeAliasMetadata");
	std::fflush(stdout);
	CheckTypeAliasMetadata();
	std::puts("CheckTemplateTypes");
	std::fflush(stdout);
	CheckTemplateTypes();
	std::puts("CheckVariantConstruction");
	std::fflush(stdout);
	CheckVariantConstruction();
	std::puts("CheckEnumAndNestedMetadata");
	std::fflush(stdout);
	CheckEnumAndNestedMetadata();
	std::puts("CheckFields");
	std::fflush(stdout);
	CheckFields();
	std::puts("CheckProperties");
	std::fflush(stdout);
	CheckProperties();
	std::puts("CheckMethods");
	std::fflush(stdout);
	CheckMethods();
	std::puts("CheckStaticMethods");
	std::fflush(stdout);
	CheckStaticMethods();
	std::puts("CheckStaticFieldsAndProperties");
	std::fflush(stdout);
	CheckStaticFieldsAndProperties();
	std::puts("CheckStaticCollectionProperties");
	std::fflush(stdout);
	CheckStaticCollectionProperties();
	std::puts("CheckErrorPaths");
	std::fflush(stdout);
	CheckErrorPaths();
	std::puts("Done");
	std::fflush(stdout);
	return 0;
}
