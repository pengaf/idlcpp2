#import "type.i"

namespace pafcore
{
	##class Variant;

	class(function_result)#PAFCORE_EXPORT Result : Metadata
	{
		Type* type { get };
		TypeCompound typeCompound { get };
		bool byRef { get };
#{
	public:
		Result(const char* name, Type* type, TypeCompound typeCompound, bool byRef);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
		bool m_byRef;
#}
	};


#{

	inline Type* Result::type() const
	{
		return m_type;
	}

	inline TypeCompound Result::typeCompound() const
	{
		return m_typeCompound;
	}

	inline bool Result::byRef() const
	{
		return m_byRef;
	}
#}

}
