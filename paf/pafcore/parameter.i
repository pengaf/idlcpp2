#import "type.i"

namespace pafcore
{
	class(function_parameter)#PAFCORE_EXPORT Parameter : Metadata
	{
		Type* type { get };
		TypeCompound typeCompound { get };
		bool byRef { get };
		bool allowNull { get };
#{
	public:
		Parameter(const char* name, Type* type, TypeCompound typeCompound, bool byRef, bool allowNull);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
		bool m_byRef;
		bool m_allowNull;
#}
	};

#{

	inline Type* Parameter::type() const
	{
		return m_type;
	}

	inline TypeCompound Parameter::typeCompound() const
	{
		return m_typeCompound;
	}

	inline bool Parameter::byRef() const
	{
		return m_byRef;
	}

	inline bool Parameter::allowNull() const
	{
		return m_allowNull;
	}

#}
}
