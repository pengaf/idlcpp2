#import "../../paf/pafcore/object.i"
###include "../../paf/pafcore/memory.h"
###include "../../paf/pafcore/std_vector.h"

namespace testsmart
{
	typedef int TestIntAlias;
	typedef unsigned long long int TestUnsignedLongLongAlias;

	enum class TestEnum
	{
		zero,
		one,
		two,
	};

	[label="plain"]
	enum PlainEnum
	{
		[meaning="first"] plain_zero,
		plain_one,
		plain_two,
	};

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

	template<T>
	struct #TEST_SMART_TYPES_EXPORT TestBox
	{
		nocode TestBox();
		T value;
		T valueProperty { get set };
		T add(T delta);
		static T staticAdd(T left, T right);
#{
	private:
		T m_valueProperty;
#}
	};

	export TestBox<int>;
	export TestBox<float>;
	typedef TestBox<int> TestIntBox;
	typedef TestBox<float> TestFloatBox;

	[category="object"]
	class #TEST_SMART_TYPES_EXPORT TestObject : pafcore::Object
	{
		int id;
		int getLiveCount();
		static TestObject* create(int id);
		static int getCreatedCount();
		static int getDestroyedCount();
#{
	public:
		TestObject(int id);
		~TestObject();
	public:
		static int s_liveCount;
		static int s_createdCount;
		static int s_destroyedCount;
#}
	};

	class #TEST_SMART_TYPES_EXPORT TestLifeCycle : pafcore::STRCObject
	{
		TestLifeCycle();
		TestLifeCycle(int id);
		int id;
		int serial;
		static void resetCounters();
		static int getLiveCount();
		static int getCreatedCount();
		static int getDestroyedCount();
		static int getNextSerial();
#{
	public:
		~TestLifeCycle();
	public:
		static int s_liveCount;
		static int s_createdCount;
		static int s_destroyedCount;
		static int s_nextSerial;
#}
	};

	class #TEST_SMART_TYPES_EXPORT TestHolder : pafcore::Object
	{
		struct NestedValue
		{
			int nestedNumber;
		};

		enum NestedEnum
		{
			nested_zero,
			nested_one,
		};

		typedef int NestedAlias;

		int primitiveField;
		signed char signedCharField;
		unsigned int unsignedIntField;
		unsigned long long int unsignedLongLongField;
		long double longDoubleField;
		string_t stringField;
		TestValueSmall valueField;
		TestValueLarge largeValueField;
		TestEnum enumField;
		PlainEnum plainEnumField;
		NestedValue nestedField;
		NestedEnum nestedEnumField;
		[flag="true", note="field"] int attributedField;
		[utf8_note=u8"field"] int u8AttributedField;
		int nativeField = "m_nativeField";
		nometa int noMetaField;
		nocode int noCodeField;
		TestObject* observerField;
		int fixedValues[#3];
		static int staticFixedValues[#3];
		static int staticPrimitiveField;

		int primitiveProperty { get set };
		int nativeProperty { get = "getNativeProperty" set = "setNativeProperty" };
		TestValueSmall valueProperty { get set };
		TestValueLarge largeValueProperty { get set };
		TestEnum enumProperty { get set };
		PlainEnum plainEnumProperty { get set };
		TestObject* observerProperty { get set };
		int fixedProperty[] { get set };
		int dynamicProperty[?] { get set };
		int listProperty[*] { get set };
		static int staticFixedProperty[] { get set };
		static int staticDynamicProperty[?] { get set };
		static int staticListProperty[*] { get set };
		static int staticRefProperty { get& set& };
		static int staticPrimitiveProperty { get set };

		void resetPrimitive();
		int readPrimitive() const;
		int sumPrimitive(int delta);
		[role="method"] int attributedMethod(int value);
		TestValueSmall addValue(TestValueSmall value);
		TestValueLarge addLargeValue(TestValueLarge value);
		TestEnum acceptEnum(TestEnum value);
		PlainEnum acceptPlainEnum(PlainEnum value);
		TestObject* acceptObserver(TestObject* value);
		TestObject* maybeObserver(TestObject* value?);
		TestObject: observe(TestObject^ value);
		(int sum, int product) splitPrimitive(int left, int right);
		int nativeMethod(int value) = "callNativeMethod";

		int makePrimitive();
		TestValueSmall makeValue();
		TestValueLarge makeLargeValue();
		TestEnum makeEnum();
		PlainEnum makePlainEnum();
		TestObject* makeObserver();

		static TestHolder* create(int seed);
		static int staticAdd(int left, int right);
		static TestObject^ createSharedObject(int id);
		static (int sum, int product) staticSplitPrimitive(int left, int right);
#{
	public:
		TestHolder(int seed);
		~TestHolder();
	private:
		int m_primitiveProperty;
		TestValueSmall m_valueProperty;
		TestValueLarge m_largeValueProperty;
		TestEnum m_enumProperty;
		PlainEnum m_plainEnumProperty;
		TestObject* m_observerProperty;
		int m_fixedProperty[3];
		::pafcore::vector<int> m_dynamicProperty;
		::pafcore::vector<int> m_listProperty;
	public:
		int m_nativeField;
		int m_nativeProperty;
		int noCodeField;
	private:
		static int s_staticFixedProperty[3];
		static ::pafcore::vector<int> s_staticDynamicProperty;
		static ::pafcore::vector<int> s_staticListProperty;
		static int s_staticRefProperty;
		static int s_staticPrimitiveProperty;
	public:
		int getNativeProperty() const;
		TestHolder& setNativeProperty(int value);
		int callNativeMethod(int value);
#}
	};
}
