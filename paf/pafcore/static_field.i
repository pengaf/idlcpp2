#import "type.i"

namespace pafcore
{

#{
	struct Attributes;
#}

	class(static_field)#PAFCORE_EXPORT StaticField : Metadata
	{
		Type* type { get };
		TypeCompound typeCompound{ get };
		bool isArray{ get };
		size_t address { get };
#{	
	public:
		StaticField(const char* name, Attributes* attributes, Type* type, size_t address, size_t arraySize, TypeCompound typeCompound);
	public:
		size_t getArraySize() const
		{
			return m_arraySize;
		}
	public:
		Type* m_type;
		size_t m_address;
		size_t m_arraySize;
		TypeCompound m_typeCompound;
#}
	};

}