#include "test_smart_types.h"
#include "test_smart_types.mh"
#include "test_smart_types.ic"
#include "test_smart_types.mc"

#include "../../paf/pafcore/memory.h"

namespace testsmart
{
	using ::pafcore::Create;
	using ::pafcore::CreateObject;
	using ::pafcore::ObserverPtr;
	using ::pafcore::SharedPtr;
	using ::pafcore::UniquePtr;

	namespace
	{
		TestValueLarge MakeLargeValue(int base)
		{
			TestValueLarge value;
			value.v0 = base + 0;
			value.v1 = base + 1;
			value.v2 = base + 2;
			value.v3 = base + 3;
			value.v4 = base + 4;
			value.v5 = base + 5;
			value.v6 = base + 6;
			value.v7 = base + 7;
			value.v8 = base + 8;
			value.v9 = base + 9;
			value.v10 = base + 10;
			value.v11 = base + 11;
			value.v12 = base + 12;
			value.v13 = base + 13;
			value.v14 = base + 14;
			value.v15 = base + 15;
			value.v16 = base + 16;
			value.v17 = base + 17;
			value.v18 = base + 18;
			value.v19 = base + 19;
			return value;
		}
	}

	int TestObject::s_liveCount = 0;
	int TestObject::s_createdCount = 0;
	int TestObject::s_destroyedCount = 0;

	TestObject::TestObject()
		: id(0)
	{
		++s_liveCount;
		++s_createdCount;
	}

	TestObject::TestObject(int value)
		: id(value)
	{
		++s_liveCount;
		++s_createdCount;
	}

	TestObject::~TestObject()
	{
		--s_liveCount;
		++s_destroyedCount;
	}

	int TestObject::getLiveCount()
	{
		return s_liveCount;
	}

	int TestObject::getCreatedCount()
	{
		return s_createdCount;
	}

	int TestObject::getDestroyedCount()
	{
		return s_destroyedCount;
	}

	TestHolder::TestHolder()
		: primitiveField(1)
		, largeValueField(MakeLargeValue(20))
		, observerField(nullptr)
		, m_primitiveProperty(11)
		, m_largeValueProperty(MakeLargeValue(40))
		, m_observerProperty(nullptr)
	{
		valueField.x = 2;
		valueField.y = 3;
		m_valueProperty.x = 4;
		m_valueProperty.y = 5;
		TestValueSmall* uniqueFieldValue = Create<TestValueSmall>();
		uniqueFieldValue->x = 6;
		uniqueFieldValue->y = 7;
		uniqueField = UniquePtr<TestValueSmall>(uniqueFieldValue);

		TestValueSmall* uniquePropertyValue = Create<TestValueSmall>();
		uniquePropertyValue->x = 8;
		uniquePropertyValue->y = 9;
		m_uniqueProperty = UniquePtr<TestValueSmall>(uniquePropertyValue);

		sharedField = SharedPtr<TestObject>(TestObject::New(12));
		m_sharedProperty = SharedPtr<TestObject>(TestObject::New(13));
	}

	TestHolder::~TestHolder()
	{
	}

	int TestHolder::primitiveProperty() const
	{
		return m_primitiveProperty;
	}

	TestHolder& TestHolder::primitiveProperty(int value)
	{
		m_primitiveProperty = value;
		return *this;
	}

	TestValueSmall TestHolder::valueProperty() const
	{
		return m_valueProperty;
	}

	TestHolder& TestHolder::valueProperty(TestValueSmall value)
	{
		m_valueProperty = value;
		return *this;
	}

	TestValueLarge TestHolder::largeValueProperty() const
	{
		return m_largeValueProperty;
	}

	TestHolder& TestHolder::largeValueProperty(TestValueLarge value)
	{
		m_largeValueProperty = value;
		return *this;
	}

	ObserverPtr<TestObject> TestHolder::observerProperty() const
	{
		return ObserverPtr<TestObject>(m_observerProperty);
	}

	TestHolder& TestHolder::observerProperty(ObserverPtr<TestObject> value)
	{
		m_observerProperty = value.get();
		return *this;
	}

	UniquePtr<TestValueSmall> TestHolder::uniqueProperty() const
	{
		TestValueSmall* copy = Create<TestValueSmall>();
		*copy = *m_uniqueProperty.get();
		return UniquePtr<TestValueSmall>(copy);
	}

	TestHolder& TestHolder::uniqueProperty(UniquePtr<TestValueSmall> value)
	{
		m_uniqueProperty = std::move(value);
		return *this;
	}

	SharedPtr<TestObject> TestHolder::sharedProperty() const
	{
		return m_sharedProperty;
	}

	TestHolder& TestHolder::sharedProperty(SharedPtr<TestObject> value)
	{
		m_sharedProperty = value;
		return *this;
	}

	int TestHolder::sumPrimitive(int delta)
	{
		primitiveField += delta;
		return primitiveField;
	}

	TestValueSmall TestHolder::addValue(const TestValueSmall& value)
	{
		TestValueSmall result = valueField;
		result.x += value.x;
		result.y += value.y;
		return result;
	}

	TestValueLarge TestHolder::addLargeValue(const TestValueLarge& value)
	{
		TestValueLarge result = largeValueField;
		result.v0 += value.v0;
		result.v1 += value.v1;
		result.v2 += value.v2;
		result.v3 += value.v3;
		result.v4 += value.v4;
		result.v5 += value.v5;
		result.v6 += value.v6;
		result.v7 += value.v7;
		result.v8 += value.v8;
		result.v9 += value.v9;
		result.v10 += value.v10;
		result.v11 += value.v11;
		result.v12 += value.v12;
		result.v13 += value.v13;
		result.v14 += value.v14;
		result.v15 += value.v15;
		result.v16 += value.v16;
		result.v17 += value.v17;
		result.v18 += value.v18;
		result.v19 += value.v19;
		return result;
	}

	ObserverPtr<TestObject> TestHolder::acceptObserver(ObserverPtr<TestObject> value)
	{
		observerField = value.get();
		return ObserverPtr<TestObject>(observerField);
	}

	UniquePtr<TestValueSmall> TestHolder::acceptUnique(UniquePtr<TestValueSmall> value)
	{
		uniqueField = std::move(value);
		TestValueSmall* copy = Create<TestValueSmall>();
		*copy = *uniqueField.get();
		return UniquePtr<TestValueSmall>(copy);
	}

	SharedPtr<TestObject> TestHolder::acceptShared(SharedPtr<TestObject> value)
	{
		sharedField = value;
		return sharedField;
	}

	int TestHolder::makePrimitive()
	{
		return primitiveField + 100;
	}

	TestValueSmall TestHolder::makeValue()
	{
		TestValueSmall result;
		result.x = valueField.x + 10;
		result.y = valueField.y + 20;
		return result;
	}

	TestValueLarge TestHolder::makeLargeValue()
	{
		return MakeLargeValue(200);
	}

	ObserverPtr<TestObject> TestHolder::makeObserver()
	{
		return ObserverPtr<TestObject>(observerField ? observerField : sharedField.get());
	}

	UniquePtr<TestValueSmall> TestHolder::makeUnique()
	{
		TestValueSmall* value = Create<TestValueSmall>();
		value->x = 30;
		value->y = 40;
		return UniquePtr<TestValueSmall>(value);
	}

	SharedPtr<TestObject> TestHolder::makeShared()
	{
		return SharedPtr<TestObject>(TestObject::New(99));
	}

	int TestHolder::staticAdd(int left, int right)
	{
		return left + right;
	}
}
