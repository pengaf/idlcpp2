#include "test_smart_types.h"
#include "test_smart_types.mh"
#include "test_smart_types.ic"
#include "test_smart_types.mc"

#include "../../paf/pafcore/memory.h"
#include "../../paf/pafcore/iterator.h"
#include "../../paf/pafcore/std_vector.h"

namespace testsmart
{
	using ::pafcore::ObserverPtr;
	using ::pafcore::SharedPtr;

	namespace
	{
		pafcore::vector<SharedPtr<TestObject>>& ObjectPool()
		{
			static pafcore::vector<SharedPtr<TestObject>> s_objects;
			return s_objects;
		}

		pafcore::vector<SharedPtr<TestHolder>>& HolderPool()
		{
			static pafcore::vector<SharedPtr<TestHolder>> s_holders;
			return s_holders;
		}

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

	template<typename T>
	T TestBox<T>::valueProperty() const
	{
		return m_valueProperty;
	}

	template<typename T>
	TestBox<T>& TestBox<T>::valueProperty(T value)
	{
		m_valueProperty = value;
		return *this;
	}

	template<typename T>
	T TestBox<T>::add(T delta)
	{
		value += delta;
		return value;
	}

	template<typename T>
	T TestBox<T>::staticAdd(T left, T right)
	{
		return left + right;
	}

	int TestObject::s_liveCount = 0;
	int TestObject::s_createdCount = 0;
	int TestObject::s_destroyedCount = 0;
	int TestLifeCycle::s_liveCount = 0;
	int TestLifeCycle::s_createdCount = 0;
	int TestLifeCycle::s_destroyedCount = 0;
	int TestLifeCycle::s_nextSerial = 1;
	int TestHolder::s_staticRefProperty = 810;
	int TestHolder::staticFixedValues[3] = { 710, 711, 712 };
	int TestHolder::staticPrimitiveField = 700;
	int TestHolder::s_staticFixedProperty[3] = { 820, 821, 822 };
	::pafcore::vector<int> TestHolder::s_staticDynamicProperty = { 830, 831, 832 };
	::pafcore::vector<int> TestHolder::s_staticListProperty = { 840, 841, 842 };
	int TestHolder::s_staticPrimitiveProperty = 800;

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

	TestObject* TestObject::create(int id)
	{
		ObjectPool().push_back(::pafcore::MakeShared<TestObject>(id));
		return ObjectPool().back().get();
	}

	int TestObject::getCreatedCount()
	{
		return s_createdCount;
	}

	int TestObject::getDestroyedCount()
	{
		return s_destroyedCount;
	}

	TestLifeCycle::TestLifeCycle()
		: id(-1)
		, serial(s_nextSerial++)
	{
		++s_liveCount;
		++s_createdCount;
	}

	TestLifeCycle::TestLifeCycle(int value)
		: id(value)
		, serial(s_nextSerial++)
	{
		++s_liveCount;
		++s_createdCount;
	}

	TestLifeCycle::~TestLifeCycle()
	{
		--s_liveCount;
		++s_destroyedCount;
	}

	void TestLifeCycle::resetCounters()
	{
		s_liveCount = 0;
		s_createdCount = 0;
		s_destroyedCount = 0;
		s_nextSerial = 1;
	}

	int TestLifeCycle::getLiveCount()
	{
		return s_liveCount;
	}

	int TestLifeCycle::getCreatedCount()
	{
		return s_createdCount;
	}

	int TestLifeCycle::getDestroyedCount()
	{
		return s_destroyedCount;
	}

	int TestLifeCycle::getNextSerial()
	{
		return s_nextSerial;
	}

	TestHolder::TestHolder(int seed)
		: primitiveField(seed + 1)
		, signedCharField(static_cast<signed char>(-8))
		, unsignedIntField(9)
		, unsignedLongLongField(10)
		, longDoubleField(11.0)
		, largeValueField(MakeLargeValue(20))
		, enumField(TestEnum::zero)
		, plainEnumField(plain_zero)
		, nestedEnumField(nested_zero)
		, attributedField(12)
		, noMetaField(13)
		, observerField(nullptr)
		, m_primitiveProperty(11)
		, m_largeValueProperty(MakeLargeValue(40))
		, m_enumProperty(TestEnum::one)
		, m_plainEnumProperty(plain_one)
		, m_observerProperty(nullptr)
		, m_dynamicProperty()
		, m_listProperty()
		, m_nativeField(seed + 60)
		, m_nativeProperty(seed + 70)
		, noCodeField(seed + 80)
	{
		stringField = "smart";
		valueField.x = 2;
		valueField.y = 3;
		nestedField.nestedNumber = seed + 50;
		fixedValues[0] = seed + 10;
		fixedValues[1] = seed + 11;
		fixedValues[2] = seed + 12;
		m_fixedProperty[0] = seed + 20;
		m_fixedProperty[1] = seed + 21;
		m_fixedProperty[2] = seed + 22;
		m_dynamicProperty.push_back(seed + 30);
		m_dynamicProperty.push_back(seed + 31);
		m_dynamicProperty.push_back(seed + 32);
		m_listProperty.push_back(seed + 40);
		m_listProperty.push_back(seed + 41);
		m_listProperty.push_back(seed + 42);
		m_valueProperty.x = 4;
		m_valueProperty.y = 5;
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

	int TestHolder::getNativeProperty() const
	{
		return m_nativeProperty;
	}

	TestHolder& TestHolder::setNativeProperty(int value)
	{
		m_nativeProperty = value;
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

	TestEnum TestHolder::enumProperty() const
	{
		return m_enumProperty;
	}

	TestHolder& TestHolder::enumProperty(TestEnum value)
	{
		m_enumProperty = value;
		return *this;
	}

	PlainEnum TestHolder::plainEnumProperty() const
	{
		return m_plainEnumProperty;
	}

	TestHolder& TestHolder::plainEnumProperty(PlainEnum value)
	{
		m_plainEnumProperty = value;
		return *this;
	}

	TestObject* TestHolder::observerProperty() const
	{
		return m_observerProperty;
	}

	TestHolder& TestHolder::observerProperty(TestObject* value)
	{
		m_observerProperty = value;
		return *this;
	}

	int TestHolder::fixedProperty(::pafcore::Iterator* iterator) const
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return m_fixedProperty[it->index()];
	}

	TestHolder& TestHolder::fixedProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		m_fixedProperty[it->index()] = value;
		return *this;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_fixedProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(m_fixedProperty, 3);
	}

	size_t TestHolder::size_fixedProperty() const
	{
		return 3;
	}

	int TestHolder::dynamicProperty(::pafcore::Iterator* iterator) const
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return m_dynamicProperty[it->index()];
	}

	TestHolder& TestHolder::dynamicProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		m_dynamicProperty[it->index()] = value;
		return *this;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_dynamicProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(m_dynamicProperty.data(), m_dynamicProperty.size());
	}

	size_t TestHolder::size_dynamicProperty() const
	{
		return m_dynamicProperty.size();
	}

	void TestHolder::resize_dynamicProperty(size_t size)
	{
		m_dynamicProperty.resize(size, 0);
	}

	int TestHolder::listProperty(::pafcore::Iterator* iterator) const
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return m_listProperty[it->index()];
	}

	TestHolder& TestHolder::listProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		m_listProperty[it->index()] = value;
		return *this;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_listProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(m_listProperty.data(), m_listProperty.size());
	}

	void TestHolder::insert_listProperty(::pafcore::Iterator* iterator, int const& value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		m_listProperty.insert(m_listProperty.begin() + it->index(), value);
	}

	void TestHolder::erase_listProperty(::pafcore::Iterator* iterator)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		m_listProperty.erase(m_listProperty.begin() + it->index());
	}

	int TestHolder::staticFixedProperty(::pafcore::Iterator* iterator)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return s_staticFixedProperty[it->index()];
	}

	void TestHolder::staticFixedProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		s_staticFixedProperty[it->index()] = value;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_staticFixedProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(s_staticFixedProperty, 3);
	}

	size_t TestHolder::size_staticFixedProperty()
	{
		return 3;
	}

	int TestHolder::staticDynamicProperty(::pafcore::Iterator* iterator)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return s_staticDynamicProperty[it->index()];
	}

	void TestHolder::staticDynamicProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		s_staticDynamicProperty[it->index()] = value;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_staticDynamicProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(s_staticDynamicProperty.data(), s_staticDynamicProperty.size());
	}

	size_t TestHolder::size_staticDynamicProperty()
	{
		return s_staticDynamicProperty.size();
	}

	void TestHolder::resize_staticDynamicProperty(size_t size)
	{
		s_staticDynamicProperty.resize(size, 0);
	}

	int TestHolder::staticListProperty(::pafcore::Iterator* iterator)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		return s_staticListProperty[it->index()];
	}

	void TestHolder::staticListProperty(::pafcore::Iterator* iterator, int value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		s_staticListProperty[it->index()] = value;
	}

	::pafcore::SharedPtr<::pafcore::Iterator> TestHolder::iterate_staticListProperty()
	{
		return ::pafcore::MakeShared<::pafcore::ArrayIteratorImpl<int>>(s_staticListProperty.data(), s_staticListProperty.size());
	}

	void TestHolder::insert_staticListProperty(::pafcore::Iterator* iterator, int const& value)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		s_staticListProperty.insert(s_staticListProperty.begin() + it->index(), value);
	}

	void TestHolder::erase_staticListProperty(::pafcore::Iterator* iterator)
	{
		::pafcore::ArrayIteratorImpl<int>* it = static_cast<::pafcore::ArrayIteratorImpl<int>*>(iterator);
		s_staticListProperty.erase(s_staticListProperty.begin() + it->index());
	}

	int const& TestHolder::staticRefProperty()
	{
		return s_staticRefProperty;
	}

	void TestHolder::staticRefProperty(int const& value)
	{
		s_staticRefProperty = value;
	}

	int TestHolder::staticPrimitiveProperty()
	{
		return s_staticPrimitiveProperty;
	}

	void TestHolder::staticPrimitiveProperty(int value)
	{
		s_staticPrimitiveProperty = value;
	}

	void TestHolder::resetPrimitive()
	{
		primitiveField = 0;
	}

	int TestHolder::readPrimitive() const
	{
		return primitiveField;
	}

	int TestHolder::sumPrimitive(int delta)
	{
		primitiveField += delta;
		return primitiveField;
	}

	int TestHolder::attributedMethod(int value)
	{
		return value + 1000;
	}

	int TestHolder::callNativeMethod(int value)
	{
		return value + 2000;
	}

	TestValueSmall TestHolder::addValue(TestValueSmall value)
	{
		TestValueSmall result = valueField;
		result.x += value.x;
		result.y += value.y;
		return result;
	}

	TestValueLarge TestHolder::addLargeValue(TestValueLarge value)
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

	TestEnum TestHolder::acceptEnum(TestEnum value)
	{
		enumField = value;
		return enumField;
	}

	PlainEnum TestHolder::acceptPlainEnum(PlainEnum value)
	{
		plainEnumField = value;
		return plainEnumField;
	}

	TestObject* TestHolder::acceptObserver(TestObject* value)
	{
		observerField = value;
		return observerField;
	}

	TestObject* TestHolder::maybeObserver(TestObject* value)
	{
		observerField = value;
		return observerField;
	}

	ObserverPtr<TestObject> TestHolder::observe(SharedPtr<TestObject> value)
	{
		observerField = value.get();
		return ObserverPtr<TestObject>(value.get());
	}

	int TestHolder::splitPrimitive(int& product, int left, int right)
	{
		product = left * right;
		return left + right;
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

	TestEnum TestHolder::makeEnum()
	{
		return enumField;
	}

	PlainEnum TestHolder::makePlainEnum()
	{
		return plainEnumField;
	}

	TestObject* TestHolder::makeObserver()
	{
		return observerField;
	}

	TestHolder* TestHolder::create(int seed)
	{
		HolderPool().push_back(::pafcore::MakeShared<TestHolder>(seed));
		return HolderPool().back().get();
	}

	int TestHolder::staticAdd(int left, int right)
	{
		return left + right;
	}

	SharedPtr<TestObject> TestHolder::createSharedObject(int id)
	{
		return ::pafcore::MakeShared<TestObject>(id);
	}

	int TestHolder::staticSplitPrimitive(int& product, int left, int right)
	{
		product = left * right;
		return left + right;
	}
}
