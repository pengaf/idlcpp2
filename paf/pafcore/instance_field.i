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
		bool isArray{ get };
		size_t offset { get };
#{
	public:
		InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, size_t offset, size_t arraySize, TypeCompound typeCompound);
	public:
		size_t getArraySize() const
		{
			return m_arraySize;
		}
	public:
		Type* m_type;
		ClassType* m_objectType;
		size_t m_offset;
		size_t m_arraySize;
		TypeCompound m_typeCompound;
#}
	};

}