#include "../../paf/pafcore/flat_set.h"
#include "../../paf/pafcore/memory.h"
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
#include "../../paf/pafcore/utility.h"

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

	void TestRawAllocate()
	{
		void* block = pafcore::Allocate(128);
		assert(0 != block);
		pafcore::Deallocate(block);
	}

	void TestAlignedCreate()
	{
		AlignedObject* object = pafcore::Create<AlignedObject>();
		assert(0 != object);
		assert(reinterpret_cast<uintptr_t>(object) % alignof(AlignedObject) == 0);
		assert(object->m_value == 7);
		pafcore::Destroy(object);
	}

	void TestArrayLifecycle()
	{
		CountingObject* objects = pafcore::CreateArray<CountingObject>(8);
		assert(0 != objects);
		assert(pafcore::ArraySizeOf(objects) == 8);
		assert(CountingObject::s_liveCount == 8);

		for (size_t i = 0; i < 8; ++i)
		{
			objects[i].m_value = static_cast<int>(i);
		}

		pafcore::DestroyArray(objects);
		assert(CountingObject::s_liveCount == 0);
	}

	void TestUtilityArrayHelpers()
	{
		int* numbers = paf_new_array<int>(4);
		assert(0 != numbers);
		assert(paf_new_array_size_of<int>(numbers) == 4);
		paf_delete_array(numbers);
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
}

int main()
{
	TestRawAllocate();
	TestAlignedCreate();
	TestArrayLifecycle();
	TestUtilityArrayHelpers();
	TestString();
	TestStdAllocator();
	TestFlatSet();
	return 0;
}
