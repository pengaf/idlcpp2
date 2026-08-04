#import "type.i"

namespace pafcore
{

	class(enum_member)#PAFCORE_EXPORT EnumMember : Metadata
	{
		Type* _type_ { get };
		int _value_ { get };
#{
	public:
		Type* m_type;
		int m_value;
	public:
		EnumMember(const char* name, Attributes* attributes, Type* type, int value);
#}
	};

}