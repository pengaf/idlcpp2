#import "../../paf/pafcore/object.i"

namespace testsmart
{
	struct #TEST_SMART_TYPES_EXPORT TestValueSmall
	{
		int x;
		int y;
	};

	struct #TEST_SMART_TYPES_EXPORT TestValueLarge
	{
		int v0;
		int v1;
		int v2;
		int v3;
		int v4;
		int v5;
		int v6;
		int v7;
		int v8;
		int v9;
		int v10;
		int v11;
		int v12;
		int v13;
		int v14;
		int v15;
		int v16;
		int v17;
		int v18;
		int v19;
	};

	class #TEST_SMART_TYPES_EXPORT TestObject : pafcore::Object
	{
		TestObject();
		TestObject(int id);

		int id;
		int getLiveCount();
		static int getCreatedCount();
		static int getDestroyedCount();
#{
	public:
		~TestObject();
	public:
		static int s_liveCount;
		static int s_createdCount;
		static int s_destroyedCount;
#}
	};

	class #TEST_SMART_TYPES_EXPORT TestHolder : pafcore::Object
	{
		TestHolder();

		int primitiveField;
		TestValueSmall valueField;
		TestValueLarge largeValueField;
		TestObject* observerField;
		TestValueSmall! uniqueField;
		TestObject^ sharedField;

		int primitiveProperty { get set };
		TestValueSmall valueProperty { get set };
		TestValueLarge largeValueProperty { get set };
		TestObject* observerProperty { get set };
		TestValueSmall! uniqueProperty { get set };
		TestObject^ sharedProperty { get set };

		int sumPrimitive(int delta);
		TestValueSmall addValue(const TestValueSmall& value);
		TestValueLarge addLargeValue(const TestValueLarge& value);
		TestObject* acceptObserver(TestObject* value);
		TestValueSmall! acceptUnique(TestValueSmall! value);
		TestObject^ acceptShared(TestObject^ value);

		int makePrimitive();
		TestValueSmall makeValue();
		TestValueLarge makeLargeValue();
		TestObject* makeObserver();
		TestValueSmall! makeUnique();
		TestObject^ makeShared();

		static int staticAdd(int left, int right);
#{
	public:
		~TestHolder();
	private:
		int m_primitiveProperty;
		TestValueSmall m_valueProperty;
		TestValueLarge m_largeValueProperty;
		TestObject* m_observerProperty;
		::pafcore::UniquePtr<TestValueSmall> m_uniqueProperty;
		::pafcore::SharedPtr<TestObject> m_sharedProperty;
#}
	};
}
