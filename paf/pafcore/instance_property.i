#import "class_type.i"
#{
#include "memory.h"
#}

namespace pafcore
{

#{
	struct Attributes;
	class InstanceProperty;
	class Iterator;
	class Variant;
	
	typedef ErrorCode(*InstancePropertyEnumerate)(InstanceProperty* instanceProperty, Variant* that, Variant* candidates);
	
	typedef ErrorCode(*InstancePropertyGet)(InstanceProperty* instanceProperty, Variant* that, Variant* value);
	typedef ErrorCode(*InstancePropertySet)(InstanceProperty* instanceProperty, Variant* that, Variant* value);

	typedef ErrorCode(*InstancePropertyArrayGet)(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
	typedef ErrorCode(*InstancePropertyArraySet)(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
	typedef ErrorCode(*InstancePropertyArraySize)(InstanceProperty* instanceProperty, Variant* that, size_t& size);
	typedef ErrorCode(*InstancePropertyArrayResize)(InstanceProperty* instanceProperty, Variant* that, size_t size);

	typedef ErrorCode(*InstancePropertyListIterate)(InstanceProperty* instanceProperty, Variant* that, Variant* iterator);
	typedef ErrorCode(*InstancePropertyListGet)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
	typedef ErrorCode(*InstancePropertyListSet)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
	typedef ErrorCode(*InstancePropertyListInsert)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
	typedef ErrorCode(*InstancePropertyListErase)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator);


#}

	class(instance_property)#PAFCORE_EXPORT InstanceProperty : Metadata
	{
		ClassType* objectType { get };
		
		bool isSimple{ get };
		bool isFixedArray{ get };
		bool isDynamicArray{ get };
		bool isList { get };

		bool hasEnumerate{ get };
		bool hasGet { get };
		bool hasSet { get };
		bool serializable { get };

		Type* type { get };
		TypeCompound typeCompound { get };
#{
	public:
		InstanceProperty(
			const char* name,
			Attributes* attributes,
			ClassType* objectType,
			Type* type,
			TypeCompound typeCompound,
			InstancePropertyEnumerate enumerate,
			InstancePropertyGet get,
			InstancePropertySet set);

		InstanceProperty(
			const char* name, 
			Attributes* attributes, 
			ClassType* objectType,
			Type* type,
			TypeCompound typeCompound,
			InstancePropertyEnumerate enumerate,
			InstancePropertyArrayGet arrayGet,
			InstancePropertyArraySet arraySet,
			InstancePropertyArraySize arraySize,
			InstancePropertyArrayResize arrayResize);


		InstanceProperty(
			const char* name,
			Attributes* attributes,
			ClassType* objectType,
			Type* type,
			TypeCompound typeCompound,
			InstancePropertyEnumerate enumerate,
			InstancePropertyListGet listGet,
			InstancePropertyListSet listSet,
			InstancePropertyListIterate listIterate,
			InstancePropertyListInsert listInsert,
			InstancePropertyListErase listErase);

	public:
		ClassType * m_objectType;
		InstancePropertyEnumerate m_enumerate;
		union
		{
			struct
			{
				InstancePropertyGet m_get;
				InstancePropertySet m_set;
			};
			struct
			{
				InstancePropertyArrayGet m_arrayGet;
				InstancePropertyArraySet m_arraySet;
				InstancePropertyArraySize m_arraySize;
				InstancePropertyArrayResize m_arrayResize;
			};
			struct
			{
				InstancePropertyListGet m_listGet;
				InstancePropertyListSet m_listSet;
				InstancePropertyListIterate m_listIterate;
				InstancePropertyListInsert m_listInsert;
				InstancePropertyListErase m_listErase;
			};
		};
		Type* m_type;
		TypeCompound m_typeCompound;
		PropertyKind m_kind;
		bool m_serializable;
#}
	};
#{


#}

}
