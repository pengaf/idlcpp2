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

	typedef ErrorCode(*StaticPropertyEnumerate)(Variant& candidates);
	typedef ErrorCode(*StaticPropertyCollectionIterate)(Variant& iterator);

	typedef ErrorCode(*StaticPropertyScalarGet)(Variant& value);
	typedef ErrorCode(*StaticPropertyScalarSet)(Variant const& value);

	typedef ErrorCode(*StaticPropertyCollectionGet)(Variant const& iterator, Variant& value);
	typedef ErrorCode(*StaticPropertyCollectionSet)(Variant const& iterator, Variant const& value);

	typedef ErrorCode(*StaticPropertyArraySize)(Variant& size);
	typedef ErrorCode(*StaticPropertyArrayResize)(Variant const& size);

	typedef ErrorCode(*StaticPropertyListInsert)(Variant const& iterator, Variant const& value);
	typedef ErrorCode(*StaticPropertyListErase)(Variant const& iterator);
#}

	class(static_property)#PAFCORE_EXPORT StaticProperty : Metadata
	{
		bool isScalar{ get };
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
			StaticPropertyScalarGet get,
			StaticPropertyScalarSet set);

		StaticProperty(
			const char* name,
			Attributes* attributes,
			Type* type,
			TypeCompound typeCompound,
			StaticPropertyEnumerate enumerate,
			StaticPropertyCollectionIterate arrayIterate,
			StaticPropertyCollectionGet arrayGet,
			StaticPropertyCollectionSet arraySet,
			StaticPropertyArraySize arraySize,
			StaticPropertyArrayResize arrayResize);

		StaticProperty(
			const char* name,
			Attributes* attributes,
			Type* type,
			TypeCompound typeCompound,
			StaticPropertyEnumerate enumerate,
			StaticPropertyCollectionIterate listIterate,
			StaticPropertyCollectionGet listGet,
			StaticPropertyCollectionSet listSet,
			StaticPropertyListInsert listInsert,
			StaticPropertyListErase listErase);
	public:
		StaticPropertyEnumerate enumerate() const
		{
			return m_enumerate;
		}

		StaticPropertyCollectionIterate collectionIterate() const
		{
			return m_collectionIterate;
		}

		StaticPropertyScalarGet scalarGet() const
		{
			return m_scalarGet;
		}

		StaticPropertyScalarSet scalarSet() const
		{
			return m_scalarSet;
		}

		StaticPropertyCollectionGet collectionGet() const
		{
			return m_collectionGet;
		}

		StaticPropertyCollectionSet collectionSet() const
		{
			return m_collectionSet;
		}

		StaticPropertyArraySize arraySize() const
		{
			return m_arraySize;
		}

		StaticPropertyArrayResize arrayResize() const
		{
			return m_arrayResize;
		}

		StaticPropertyListInsert listInsert() const
		{
			return m_listInsert;
		}

		StaticPropertyListErase listErase() const
		{
			return m_listErase;
		}
	protected:
		StaticPropertyEnumerate m_enumerate;
		StaticPropertyCollectionIterate m_collectionIterate;
		union
		{
			struct
			{
				StaticPropertyScalarGet m_scalarGet;
				StaticPropertyScalarSet m_scalarSet;
			};
			struct
			{
				StaticPropertyCollectionGet m_collectionGet;
				StaticPropertyCollectionSet m_collectionSet;
			};
		};
		union
		{
			struct
			{
				StaticPropertyArraySize m_arraySize;
				StaticPropertyArrayResize m_arrayResize;
			};
			struct
			{
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

	inline bool StaticProperty::isScalar() const
	{
		return PropertyKind::scalar_property == m_kind;
	}

	inline bool StaticProperty::isFixedArray() const
	{
		return PropertyKind::fixed_array_property == m_kind;
	}

	inline bool StaticProperty::isDynamicArray() const
	{
		return PropertyKind::dynamic_array_property == m_kind;
	}

	inline bool StaticProperty::isList() const
	{
		return PropertyKind::list_property == m_kind;
	}

	inline bool StaticProperty::hasEnumerate() const
	{
		return (0 != m_enumerate);
	}

	inline bool StaticProperty::hasGet() const
	{
		return (0 != m_scalarGet);
	}

	inline bool StaticProperty::hasSet() const
	{
		return (0 != m_scalarSet);
	}

	inline Type* StaticProperty::type() const
	{
		return m_type;
	}

	inline TypeCompound StaticProperty::typeCompound() const
	{
		return m_typeCompound;
	}

#}

}