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
		bool isScalar{ get };
		bool isArray{ get };
		size_t arraySize{ get };
		size_t address { get };
#{	
	public:
		StaticField(const char* name, Attributes* attributes, Type* type, size_t address, size_t arraySize, TypeCompound typeCompound);
	public:
		size_t getArraySize() const
		{
			return m_arraySize;
		}
	protected:
		Type* m_type;
		size_t m_address;
		size_t m_arraySize;
		TypeCompound m_typeCompound;
#}
	};

#{
	inline Type* StaticField::type() const
	{
		return m_type;
	}

	inline TypeCompound StaticField::typeCompound() const
	{
		return m_typeCompound;
	}

	inline bool StaticField::isScalar() const
	{
		return 0 == m_arraySize;
	}

	inline bool StaticField::isArray() const
	{
		return m_arraySize > 0;
	}

	inline size_t StaticField::arraySize() const
	{
		return m_arraySize;
	}

	inline size_t StaticField::address() const
	{
		return m_address;
	}
#}

}