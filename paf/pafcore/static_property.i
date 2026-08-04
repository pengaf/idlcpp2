#import "metadata.i"
#{
#include "memory.h"
#}

namespace pafcore
{

#{
	struct Attributes;
	class StaticProperty;
	class Iterator;
	class Variant;

	typedef ErrorCode(*StaticPropertyEnumerate)(Variant* candidates);

	typedef ErrorCode(*StaticPropertyGet)(Variant* value);
	typedef ErrorCode(*StaticPropertySet)(Variant* value);

	typedef ErrorCode(*StaticPropertyArrayGet)(size_t index, Variant* value);
	typedef ErrorCode(*StaticPropertyArraySet)(size_t index, Variant* value);
	typedef ErrorCode(*StaticPropertyArraySize)(size_t& size);
	typedef ErrorCode(*StaticPropertyArrayResize)(size_t size);

	typedef ErrorCode(*StaticPropertyListIterate)(Variant* iterator);
	typedef ErrorCode(*StaticPropertyListGet)(Iterator* iterator, Variant* value);
	typedef ErrorCode(*StaticPropertyListSet)(Iterator* iterator, Variant* value);
	typedef ErrorCode(*StaticPropertyListInsert)(Iterator* iterator, Variant* value);
	typedef ErrorCode(*StaticPropertyListErase)(Iterator* iterator);


#}

	class(static_property)#PAFCORE_EXPORT StaticProperty : Metadata
	{
		bool isSimple{ get };
		bool isFixedArray{ get };
		bool isDynamicArray{ get };
		bool isList { get };

		bool hasEnumerate{ get };
		bool hasGet { get };
		bool hasSet { get };

		Type* type { get };
		TypeCompound typeCompound { get };
#{
	public:
		StaticProperty(
			const char* name,
			Attributes* attributes,
			Type* type,
			TypeCompound typeCompound,
			StaticPropertyEnumerate enumerate,
			StaticPropertyGet get,
			StaticPropertySet set);

		StaticProperty(
			const char* name,
			Attributes* attributes,
			Type* type,
			TypeCompound typeCompound,
			StaticPropertyEnumerate enumerate,
			StaticPropertyArrayGet arrayGet,
			StaticPropertyArraySet arraySet,
			StaticPropertyArraySize arraySize,
			StaticPropertyArrayResize arrayResize);

		StaticProperty(
			const char* name,
			Attributes* attributes,
			Type* type,
			TypeCompound typeCompound,
			StaticPropertyEnumerate enumerate,
			StaticPropertyListGet listGet,
			StaticPropertyListSet listSet,
			StaticPropertyListIterate listIterate,
			StaticPropertyListInsert listInsert,
			StaticPropertyListErase listErase);

	public:
		StaticPropertyEnumerate m_enumerate{ nullptr };
		union
		{
			struct
			{
				StaticPropertyGet m_get;
				StaticPropertySet m_set;
			};
			struct
			{
				StaticPropertyArrayGet m_arrayGet;
				StaticPropertyArraySet m_arraySet;
				StaticPropertyArraySize m_arraySize;
				StaticPropertyArrayResize m_arrayResize;
			};
			struct
			{
				StaticPropertyListGet m_listGet;
				StaticPropertyListSet m_listSet;
				StaticPropertyListIterate m_listIterate;
				StaticPropertyListInsert m_listInsert;
				StaticPropertyListErase m_listErase;
			};
		};
		Type* m_type{ nullptr };
		TypeCompound m_typeCompound;
		PropertyKind m_kind;
#}
	};


#{

#}

}