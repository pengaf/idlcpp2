#include "../../paf/pafcore/flat_set.h"
#include "../../paf/pafcore/memory.h"
#include "../../paf/pafcore/object.h"
#include "../../paf/pafcore/primitive_type.h"
#include "../../paf/pafcore/std_deque.h"
#include "../../paf/pafcore/std_forward_list.h"
#include "../../paf/pafcore/std_list.h"
#include "../../paf/pafcore/std_map.h"
#include "../../paf/pafcore/std_queue.h"
#include "../../paf/pafcore/std_set.h"
#include "../../paf/pafcore/std_string.h"
#include "../../paf/pafcore/std_unordered_map.h"
#include "../../paf/pafcore/std_unordered_set.h"
#include "../../paf/pafcore/std_vector.h"
#include "../../paf/pafcore/string.h"
#include "../../paf/pafcore/variant.h"
#include "../test_smart_types/test_smart_types.h"
#include "../test_smart_types/test_smart_types.mh"

#include <cassert>
#include <cstdint>
#include <cstring>

namespace
{
	struct CountingObject
	{
		CountingObject()
			: m_value(0)
		{
			++s_liveCount;
		}

		~CountingObject()
		{
			--s_liveCount;
		}

		int m_value;

		static int s_liveCount;
	};

	int CountingObject::s_liveCount = 0;

	struct alignas(64) AlignedObject
	{
		AlignedObject()
			: m_value(7)
		{
		}

		int m_value;
	};

	struct SmartValue
	{
		SmartValue()
			: m_value(0)
		{
			++s_liveCount;
		}

		~SmartValue()
		{
			--s_liveCount;
		}

		int m_value;

		static int s_liveCount;
	};

	int SmartValue::s_liveCount = 0;

	struct SmartObject : pafcore::Object
	{
		SmartObject()
			: m_value(0)
		{
			++s_liveCount;
		}

		~SmartObject()
		{
			--s_liveCount;
		}

		int m_value;

		static int s_liveCount;
	};

	int SmartObject::s_liveCount = 0;

	void TestRawAllocate()
	{
		void* block = pafcore::Malloc(128);
		assert(0 != block);
		pafcore::Free(block);
	}

	void TestNewDelete()
	{
		CountingObject* object = pafcore::New<CountingObject>();
		assert(0 != object);
		assert(CountingObject::s_liveCount == 1);
		object->m_value = 42;
		assert(object->m_value == 42);
		pafcore::Delete(object);
		assert(CountingObject::s_liveCount == 0);
	}

	void TestAlignedNew()
	{
		AlignedObject* object = pafcore::New<AlignedObject>();
		assert(0 != object);
		assert(object->m_value == 7);
		pafcore::Delete(object);
	}

	void TestString()
	{
		pafcore::String text("hello");
		text.append(" world");
		assert(std::strcmp(text.c_str(), "hello world") == 0);
	}

	void TestStdAllocator()
	{
		pafcore::vector<int> values;
		values.push_back(3);
		values.push_back(1);
		values.push_back(4);
		assert(values.size() == 3);
		assert(values[0] == 3);
		assert(values[1] == 1);
		assert(values[2] == 4);

		pafcore::deque<int> dequeValues;
		dequeValues.push_back(5);
		dequeValues.push_front(2);
		assert(dequeValues.front() == 2);
		assert(dequeValues.back() == 5);

		pafcore::list<int> listValues;
		listValues.push_back(7);
		listValues.push_front(6);
		assert(listValues.front() == 6);
		assert(listValues.back() == 7);

		pafcore::forward_list<int> forwardListValues;
		forwardListValues.push_front(9);
		assert(forwardListValues.front() == 9);

		pafcore::set<int> setValues;
		setValues.insert(4);
		setValues.insert(4);
		setValues.insert(1);
		assert(setValues.size() == 2);

		pafcore::map<int, int> mapValues;
		mapValues[1] = 10;
		mapValues[2] = 20;
		assert(mapValues[1] == 10);
		assert(mapValues[2] == 20);

		pafcore::unordered_set<int> unorderedSetValues;
		unorderedSetValues.insert(8);
		unorderedSetValues.insert(8);
		assert(unorderedSetValues.size() == 1);

		pafcore::unordered_map<int, int> unorderedMapValues;
		unorderedMapValues[3] = 30;
		assert(unorderedMapValues[3] == 30);

		pafcore::queue<int> queueValues;
		queueValues.push(11);
		queueValues.push(12);
		assert(queueValues.front() == 11);
		queueValues.pop();
		assert(queueValues.front() == 12);

		pafcore::stack<int> stackValues;
		stackValues.push(13);
		stackValues.push(14);
		assert(stackValues.top() == 14);
		stackValues.pop();
		assert(stackValues.top() == 13);

		pafcore::priority_queue<int> priorityQueueValues;
		priorityQueueValues.push(15);
		priorityQueueValues.push(18);
		priorityQueueValues.push(16);
		assert(priorityQueueValues.top() == 18);

		pafcore::string stringValue("allocator");
		stringValue += " test";
		assert(std::strcmp(stringValue.c_str(), "allocator test") == 0);
	}

	void TestFlatSet()
	{
		pafcore::FlatSet<int> values;
		values.insert(3);
		values.insert(1);
		values.insert(2);
		values.insert(2);

		assert(values.size() == 3);
		assert(values[0] == 1);
		assert(values[1] == 2);
		assert(values[2] == 3);
	}

	void TestSmartPointers()
	{
		{
			pafcore::SharedArray<SmartValue> values = pafcore::MakeSharedArray<SmartValue>(2);
			assert(values);
			values[0].m_value = 21;
			values[1].m_value = 22;
			assert(values[0].m_value == 21);
			assert(values[1].m_value == 22);
			assert(SmartValue::s_liveCount == 2);
			{
				pafcore::SharedArray<SmartValue> values2 = values;
				assert(values2[0].m_value == 21);
				assert(values2[1].m_value == 22);
				assert(SmartValue::s_liveCount == 2);
			}
			assert(SmartValue::s_liveCount == 2);
		}
		assert(SmartValue::s_liveCount == 0);

		{
			pafcore::SharedPtr<SmartObject> object = pafcore::MakeShared<SmartObject>();
			assert(object);
			object->m_value = 7;
			assert(object->m_value == 7);
			assert(SmartObject::s_liveCount == 1);
		}
	}

	void TestVariantAssignSharedPtrMove()
	{
		pafcore::SharedPtr<testsmart::TestObject> object = pafcore::MakeShared<testsmart::TestObject>(123);
		assert(object);
		assert(object->id == 123);

		pafcore::Variant variant;
		variant.assignSharedPtr(std::move(object));

		assert(!object);
		assert(variant.typeCompound() == pafcore::TypeCompound::shared_ptr);

		pafcore::SharedPtr<testsmart::TestObject> read;
		assert(variant.castToSharedPtr(read));
		assert(read);
		assert(read->id == 123);
	}
}

int main()
{
	TestRawAllocate();
	TestNewDelete();
	TestAlignedNew();
	TestString();
	TestStdAllocator();
	TestFlatSet();
	TestSmartPointers();
	TestVariantAssignSharedPtrMove();
	return 0;
}
