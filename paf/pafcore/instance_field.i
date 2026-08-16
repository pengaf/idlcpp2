#import "class_type.i"

namespace pafcore
{

#{
	struct Attributes;
#}

	class(instance_field)#PAFCORE_EXPORT InstanceField : Metadata
	{
		ClassType* objectType { get };
		Type* type { get };
		TypeCompound typeCompound{ get };
		bool isScalar{ get };
		bool isArray{ get };
		size_t arraySize{ get };
		size_t offset { get };
#{
	public:
		InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, size_t offset, size_t arraySize, TypeCompound typeCompound);
	protected:
		Type* m_type;
		ClassType* m_objectType;
		size_t m_offset;
		size_t m_arraySize;
		TypeCompound m_typeCompound;
#}
	};

#{

	inline ClassType* InstanceField::objectType() const
	{
		return m_objectType;
	}

	inline Type* InstanceField::type() const
	{
		return m_type;
	}

	inline TypeCompound InstanceField::typeCompound() const
	{
		return m_typeCompound;
	}

	inline bool InstanceField::isScalar() const
	{
		return 0 == m_arraySize;
	}

	inline bool InstanceField::isArray() const
	{
		return m_arraySize > 0;
	}

	inline size_t InstanceField::arraySize() const
	{
		return m_arraySize;
	}

	inline size_t InstanceField::offset() const
	{
		return m_offset;
	}

#}

}