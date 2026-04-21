#include "../../paf/pafcore/class_type.h"
#include "../../paf/pafcore/instance_field.h"
#include "../../paf/pafcore/instance_method.h"
#include "../../paf/pafcore/instance_property.h"
#include "../../paf/pafcore/metadata.h"
#include "../../paf/pafcore/reflection.h"
#include "../../paf/pafcore/static_method.h"
#include "../../paf/pafcore/variant.h"
#include "../test_smart_types/test_smart_types.h"

#include <cassert>
#include <cstdio>

namespace
{
	using namespace pafcore;
	using namespace testsmart;

	void CheckMethodSignature()
	{
		ClassType* holderType = TestHolder::GetType();
		InstanceMethod* acceptUnique = holderType->findInstanceMethod("acceptUnique", false);
		assert(0 != acceptUnique);
		Result* acceptUniqueResult = acceptUnique->getResult(0);
		assert(acceptUniqueResult->typeCompound() == Metadata::tc_unique_ptr);
		assert(!acceptUniqueResult->byValue());

		Argument* acceptUniqueArg = acceptUnique->getArgument(0, 0);
		assert(acceptUniqueArg->typeCompound() == Metadata::tc_unique_ptr);

		InstanceMethod* acceptShared = holderType->findInstanceMethod("acceptShared", false);
		assert(0 != acceptShared);
		Result* acceptSharedResult = acceptShared->getResult(0);
		assert(acceptSharedResult->typeCompound() == Metadata::tc_shared_ptr);
		Argument* acceptSharedArg = acceptShared->getArgument(0, 0);
		assert(acceptSharedArg->typeCompound() == Metadata::tc_shared_ptr);

		InstanceMethod* acceptObserver = holderType->findInstanceMethod("acceptObserver", false);
		assert(0 != acceptObserver);
		Result* acceptObserverResult = acceptObserver->getResult(0);
		assert(acceptObserverResult->typeCompound() == Metadata::tc_observer_ptr);
		Argument* acceptObserverArg = acceptObserver->getArgument(0, 0);
		assert(acceptObserverArg->typeCompound() == Metadata::tc_observer_ptr);
	}

	void CheckFieldMetadata()
	{
		ClassType* holderType = TestHolder::GetType();
		InstanceField* primitiveField = holderType->findInstanceField("primitiveField", false);
		InstanceField* valueField = holderType->findInstanceField("valueField", false);
		InstanceField* observerField = holderType->findInstanceField("observerField", false);
		InstanceField* uniqueField = holderType->findInstanceField("uniqueField", false);
		InstanceField* sharedField = holderType->findInstanceField("sharedField", false);

		assert(primitiveField && primitiveField->m_typeCompound == Metadata::tc_none);
		assert(valueField && valueField->m_typeCompound == Metadata::tc_none);
		assert(observerField && observerField->m_typeCompound == Metadata::tc_observer_ptr);
		assert(uniqueField && uniqueField->m_typeCompound == Metadata::tc_unique_ptr);
		assert(sharedField && sharedField->m_typeCompound == Metadata::tc_shared_ptr);
	}

	void CheckPropertyMetadata()
	{
		ClassType* holderType = TestHolder::GetType();
		InstanceProperty* primitiveProperty = holderType->findInstanceProperty("primitiveProperty", false);
		InstanceProperty* valueProperty = holderType->findInstanceProperty("valueProperty", false);
		InstanceProperty* observerProperty = holderType->findInstanceProperty("observerProperty", false);
		InstanceProperty* uniqueProperty = holderType->findInstanceProperty("uniqueProperty", false);
		InstanceProperty* sharedProperty = holderType->findInstanceProperty("sharedProperty", false);

		assert(primitiveProperty && !primitiveProperty->isPtr());
		assert(valueProperty && !valueProperty->isPtr());
		assert(observerProperty && observerProperty->isPtr());
		assert(uniqueProperty && uniqueProperty->isPtr());
		assert(sharedProperty && sharedProperty->isPtr());
	}

	void CheckFields()
	{
		SharedPtr<TestHolder> holder(TestHolder::New());
		assert(holder);

		ClassType* holderType = TestHolder::GetType();
		InstanceField* primitiveField = holderType->findInstanceField("primitiveField", false);
		InstanceField* valueField = holderType->findInstanceField("valueField", false);
		InstanceField* observerField = holderType->findInstanceField("observerField", false);
		InstanceField* uniqueField = holderType->findInstanceField("uniqueField", false);
		InstanceField* sharedField = holderType->findInstanceField("sharedField", false);

		Variant that;
		that.assignRcPtr(holder.get(), false, Variant::by_ptr);

		Variant primitiveValue;
		int primitive = 123;
		primitiveValue.assignPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &primitive);
		assert(s_ok == Reflection::SetInstanceField(&that, primitiveField, primitiveValue));

		Variant primitiveRef;
		assert(s_ok == Reflection::GetInstanceFieldRef(primitiveRef, &that, primitiveField));
		int primitiveRead = 0;
		assert(primitiveRef.castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &primitiveRead));
		assert(primitiveRead == 123);

		TestValueSmall inputValue;
		inputValue.x = 8;
		inputValue.y = 9;
		Variant valueInput;
		valueInput.assignValuePtr(TestValueSmall::GetType(), &inputValue, false, Variant::by_ref);
		assert(s_ok == Reflection::SetInstanceField(&that, valueField, valueInput));

		Variant valueRef;
		assert(s_ok == Reflection::GetInstanceFieldRef(valueRef, &that, valueField));
		TestValueSmall valueRead;
		assert(valueRef.castToValue(TestValueSmall::GetType(), &valueRead));
		assert(valueRead.x == 8);
		assert(valueRead.y == 9);

		SharedPtr<TestObject> sharedObject(TestObject::New(55));
		Variant observerInput;
		observerInput.assignRcPtr(TestObject::GetType(), sharedObject.get(), false, Variant::by_ptr);
		assert(s_ok == Reflection::SetInstanceField(&that, observerField, observerInput));

		Variant observerRef;
		assert(s_ok == Reflection::GetInstanceFieldRef(observerRef, &that, observerField));
		TestObject* observerRead = 0;
		assert(observerRef.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&observerRead)));
		assert(observerRead == sharedObject.get());

		TestValueSmall* uniqueValue = Create<TestValueSmall>();
		uniqueValue->x = 44;
		uniqueValue->y = 45;
		Variant uniqueInput;
		uniqueInput.assignOwningValuePtr(TestValueSmall::GetType(), uniqueValue, false);
		assert(s_ok == Reflection::SetInstanceField(&that, uniqueField, uniqueInput));
		assert(!uniqueInput.isNull());
		assert(holder->uniqueField.get()->x == 44);
		assert(holder->uniqueField.get()->y == 45);

		Variant uniqueRef;
		assert(s_ok == Reflection::GetInstanceFieldRef(uniqueRef, &that, uniqueField));
		TestValueSmall* uniqueRead = 0;
		assert(uniqueRef.castToValuePtr(TestValueSmall::GetType(), reinterpret_cast<void**>(&uniqueRead)));
		assert(uniqueRead->x == 44);
		assert(uniqueRead->y == 45);

		Variant sharedInput;
		sharedInput.assignRcPtr(TestObject::GetType(), sharedObject.get(), false, Variant::by_ptr);
		assert(s_ok == Reflection::SetInstanceField(&that, sharedField, sharedInput));

		Variant sharedRef;
		assert(s_ok == Reflection::GetInstanceFieldRef(sharedRef, &that, sharedField));
		TestObject* sharedRead = 0;
		assert(sharedRef.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&sharedRead)));
		assert(sharedRead == sharedObject.get());
	}

	void CheckProperties()
	{
		SharedPtr<TestHolder> holder(TestHolder::New());
		assert(holder);

		ClassType* holderType = TestHolder::GetType();
		InstanceProperty* primitiveProperty = holderType->findInstanceProperty("primitiveProperty", false);
		InstanceProperty* valueProperty = holderType->findInstanceProperty("valueProperty", false);
		InstanceProperty* observerProperty = holderType->findInstanceProperty("observerProperty", false);
		InstanceProperty* uniqueProperty = holderType->findInstanceProperty("uniqueProperty", false);
		InstanceProperty* sharedProperty = holderType->findInstanceProperty("sharedProperty", false);

		Variant that;
		that.assignRcPtr(holder.get(), false, Variant::by_ptr);

		Variant primitiveInput;
		int primitive = 321;
		primitiveInput.assignPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &primitive);
		assert(s_ok == Reflection::SetInstanceProperty(&that, primitiveProperty, primitiveInput));

		Variant primitiveResult;
		assert(s_ok == Reflection::GetInstanceProperty(primitiveResult, &that, primitiveProperty));
		int primitiveRead = 0;
		assert(primitiveResult.castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &primitiveRead));
		assert(primitiveRead == 321);

		TestValueSmall value;
		value.x = 31;
		value.y = 32;
		Variant valueInput;
		valueInput.assignValuePtr(TestValueSmall::GetType(), &value, false, Variant::by_ref);
		assert(s_ok == Reflection::SetInstanceProperty(&that, valueProperty, valueInput));

		Variant valueResult;
		assert(s_ok == Reflection::GetInstanceProperty(valueResult, &that, valueProperty));
		TestValueSmall valueRead;
		assert(valueResult.castToValue(TestValueSmall::GetType(), &valueRead));
		assert(valueRead.x == 31);
		assert(valueRead.y == 32);

		SharedPtr<TestObject> object(TestObject::New(88));
		Variant observerInput;
		observerInput.assignRcPtr(TestObject::GetType(), object.get(), false, Variant::by_ptr);
		assert(s_ok == Reflection::SetInstanceProperty(&that, observerProperty, observerInput));

		Variant observerResult;
		assert(s_ok == Reflection::GetInstanceProperty(observerResult, &that, observerProperty));
		TestObject* observerRead = 0;
		assert(observerResult.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&observerRead)));
		assert(observerRead == object.get());

		TestValueSmall* uniqueValue = Create<TestValueSmall>();
		uniqueValue->x = 77;
		uniqueValue->y = 78;
		Variant uniqueInput;
		uniqueInput.assignOwningValuePtr(TestValueSmall::GetType(), uniqueValue, false);
		assert(s_ok == Reflection::SetInstanceProperty(&that, uniqueProperty, uniqueInput));

		Variant uniqueResult;
		assert(s_ok == Reflection::GetInstanceProperty(uniqueResult, &that, uniqueProperty));
		TestValueSmall* uniqueRead = 0;
		assert(uniqueResult.castToValuePtr(TestValueSmall::GetType(), reinterpret_cast<void**>(&uniqueRead)));
		assert(uniqueRead->x == 77);
		assert(uniqueRead->y == 78);

		Variant sharedInput;
		sharedInput.assignRcPtr(TestObject::GetType(), object.get(), false, Variant::by_ptr);
		assert(s_ok == Reflection::SetInstanceProperty(&that, sharedProperty, sharedInput));

		Variant sharedResult;
		assert(s_ok == Reflection::GetInstanceProperty(sharedResult, &that, sharedProperty));
		TestObject* sharedRead = 0;
		assert(sharedResult.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&sharedRead)));
		assert(sharedRead == object.get());
	}

	void CheckMethods()
	{
		SharedPtr<TestHolder> holder(TestHolder::New());
		assert(holder);
		ClassType* holderType = TestHolder::GetType();
		Variant that;
		that.assignRcPtr(holder.get(), false, Variant::by_ptr);
		SharedPtr<TestObject> observerObject(TestObject::New(66));

		{
			InstanceMethod* method = holderType->findInstanceMethod("sumPrimitive", false);
			Variant args[1];
			int value = 7;
			args[0].assignPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &value);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, args, 1));
			int sum = 0;
			assert(result.castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &sum));
			assert(sum == 8);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("addValue", false);
			Variant args[1];
			TestValueSmall value;
			value.x = 5;
			value.y = 6;
			args[0].assignValuePtr(TestValueSmall::GetType(), &value, true, Variant::by_ref);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, args, 1));
			TestValueSmall read;
			assert(result.castToValue(TestValueSmall::GetType(), &read));
			assert(read.x == 7);
			assert(read.y == 9);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("acceptObserver", false);
			Variant args[1];
			args[0].assignRcPtr(TestObject::GetType(), observerObject.get(), false, Variant::by_ptr);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, args, 1));
			TestObject* read = 0;
			assert(result.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&read)));
			assert(read == observerObject.get());
		}

		{
			TestValueSmall* uniqueValue = Create<TestValueSmall>();
			uniqueValue->x = 100;
			uniqueValue->y = 101;
			InstanceMethod* method = holderType->findInstanceMethod("acceptUnique", false);
			Variant args[1];
			args[0].assignOwningValuePtr(TestValueSmall::GetType(), uniqueValue, false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, args, 1));
			TestValueSmall* read = 0;
			assert(result.castToValuePtr(TestValueSmall::GetType(), reinterpret_cast<void**>(&read)));
			assert(read->x == 100);
			assert(read->y == 101);
		}

		{
			SharedPtr<TestObject> object(TestObject::New(67));
			InstanceMethod* method = holderType->findInstanceMethod("acceptShared", false);
			Variant args[1];
			args[0].assignRcPtr(TestObject::GetType(), object.get(), false, Variant::by_ptr);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, args, 1));
			TestObject* read = 0;
			assert(result.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&read)));
			assert(read == object.get());
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makePrimitive", false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, 0, 0));
			int read = 0;
			assert(result.castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &read));
			assert(read == 108);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeValue", false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, 0, 0));
			TestValueSmall read;
			assert(result.castToValue(TestValueSmall::GetType(), &read));
			assert(read.x == 12);
			assert(read.y == 23);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeObserver", false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, 0, 0));
			TestObject* read = 0;
			assert(result.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&read)));
			assert(read == observerObject.get());
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeUnique", false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, 0, 0));
			TestValueSmall* read = 0;
			assert(result.castToValuePtr(TestValueSmall::GetType(), reinterpret_cast<void**>(&read)));
			assert(read->x == 30);
			assert(read->y == 40);
		}

		{
			InstanceMethod* method = holderType->findInstanceMethod("makeShared", false);
			Variant result;
			assert(s_ok == Reflection::CallInstanceMethod(result, &that, method, 0, 0));
			TestObject* read = 0;
			assert(result.castToRcPtr(TestObject::GetType(), reinterpret_cast<void**>(&read)));
			assert(read->id == 99);
		}
	}

	void CheckStaticMethods()
	{
		ClassType* holderType = TestHolder::GetType();
		StaticMethod* createMethod = holderType->findStaticMethod("New", false);
		assert(0 != createMethod);
		Variant result;
		assert(s_ok == Reflection::CallStaticMethod(result, createMethod, 0, 0));
		TestHolder* holder = 0;
		assert(result.castToRcPtr(TestHolder::GetType(), reinterpret_cast<void**>(&holder)));
		assert(0 != holder);

		StaticMethod* staticAdd = holderType->findStaticMethod("staticAdd", false);
		assert(0 != staticAdd);
		Variant args[2];
		int left = 20;
		int right = 22;
		args[0].assignPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &left);
		args[1].assignPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &right);
		Variant addResult;
		assert(s_ok == Reflection::CallStaticMethod(addResult, staticAdd, args, 2));
		int sum = 0;
		assert(addResult.castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &sum));
		assert(sum == 42);
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
	std::puts("Done");
	std::fflush(stdout);
	return 0;
}

