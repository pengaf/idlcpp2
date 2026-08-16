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
	
	typedef ErrorCode(*InstancePropertyEnumerate)(Variant const& that, Variant& candidates);
	
	typedef ErrorCode(*InstancePropertyScalarGet)(Variant const& that, Variant& value);
	typedef ErrorCode(*InstancePropertyScalarSet)(Variant& that, Variant const& value);

	typedef ErrorCode(*InstancePropertyCollectionIterate)(Variant const& that, Variant& iterator);

	typedef ErrorCode(*InstancePropertyCollectionGet)(Variant const& that, Variant const& iterator, Variant& value);
	typedef ErrorCode(*InstancePropertyCollectionSet)(Variant& that, Variant const& iterator, Variant const& value);

	typedef ErrorCode(*InstancePropertyArraySize)(Variant const& that, Variant& size);
	typedef ErrorCode(*InstancePropertyArrayResize)(Variant& that, Variant const& size);

	typedef ErrorCode(*InstancePropertyListInsert)(Variant& that, Variant const& iterator, Variant const& value);
	typedef ErrorCode(*InstancePropertyListErase)(Variant& that, Variant const& iterator);
#}

	class(instance_property)#PAFCORE_EXPORT InstanceProperty : Metadata
	{
		ClassType* objectType { get };
		
		bool isScalar{ get };
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
			InstancePropertyScalarGet scalarGet,
			InstancePropertyScalarSet scalarSet);

		InstanceProperty(
			const char* name, 
			Attributes* attributes, 
			ClassType* objectType,
			Type* type,
			TypeCompound typeCompound,
			InstancePropertyEnumerate enumerate,
			InstancePropertyCollectionIterate collectionIterate,
			InstancePropertyCollectionGet collectionGet,
			InstancePropertyCollectionSet collectionSet,
			InstancePropertyArraySize arraySize,
			InstancePropertyArrayResize arrayResize);


		InstanceProperty(
			const char* name,
			Attributes* attributes,
			ClassType* objectType,
			Type* type,
			TypeCompound typeCompound,
			InstancePropertyEnumerate enumerate,
			InstancePropertyCollectionIterate collectionIterate,
			InstancePropertyCollectionGet collectionGet,
			InstancePropertyCollectionSet collectionSet,
			InstancePropertyListInsert listInsert,
			InstancePropertyListErase listErase);


		InstancePropertyEnumerate enumerate() const
		{
			return m_enumerate;
		}
		
		InstancePropertyCollectionIterate collectionIterate() const
		{
			return m_collectionIterate;
		}

		InstancePropertyScalarGet scalarGet() const
		{
			return m_scalarGet;
		}

		InstancePropertyScalarSet scalarSet() const
		{
			return m_scalarSet;
		}

		InstancePropertyCollectionGet collectionGet() const
		{
			return m_collectionGet;
		}

		InstancePropertyCollectionSet collectionSet() const
		{
			return m_collectionSet;
		}

		InstancePropertyArraySize arraySize() const
		{
			return m_arraySize;
		}

		InstancePropertyArrayResize arrayResize() const
		{
			return m_arrayResize;
		}

		InstancePropertyListInsert listInsert() const
		{
			return m_listInsert;
		}

		InstancePropertyListErase listErase() const
		{
			return m_listErase;
		}

	protected:
		ClassType * m_objectType;
		InstancePropertyEnumerate m_enumerate;
		InstancePropertyCollectionIterate m_collectionIterate;
		union
		{
			struct
			{
				InstancePropertyScalarGet m_scalarGet;
				InstancePropertyScalarSet m_scalarSet;
			};
			struct
			{
				InstancePropertyCollectionGet m_collectionGet;
				InstancePropertyCollectionSet m_collectionSet;
			};
		};
		union
		{
			struct
			{
				InstancePropertyArraySize m_arraySize;
				InstancePropertyArrayResize m_arrayResize;
			};
			struct
			{
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

	inline ClassType* InstanceProperty::objectType() const
	{
		return m_objectType;
	}

	inline bool InstanceProperty::isScalar() const
	{
		return PropertyKind::scalar_property == m_kind;
	}

	inline bool InstanceProperty::isFixedArray() const
	{
		return PropertyKind::fixed_array_property == m_kind;
	}

	inline bool InstanceProperty::isDynamicArray() const
	{
		return PropertyKind::dynamic_array_property == m_kind;
	}

	inline bool InstanceProperty::isList() const
	{
		return PropertyKind::list_property == m_kind;
	}

	inline bool InstanceProperty::hasEnumerate() const
	{
		return (0 != m_enumerate);
	}

	inline bool InstanceProperty::hasGet() const
	{
		return (0 != m_scalarGet);
	}

	inline bool InstanceProperty::hasSet() const
	{
		return (0 != m_scalarSet);
	}

	inline Type* InstanceProperty::type() const
	{
		return m_type;
	}

	inline TypeCompound InstanceProperty::typeCompound() const
	{
		return m_typeCompound;
	}

	inline bool InstanceProperty::serializable() const
	{
		return m_serializable;
	}

#}

}
