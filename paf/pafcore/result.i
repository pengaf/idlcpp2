#import "type.i"

namespace pafcore
{
	##class Variant;

	abstract class(function_result)#PAFCORE_EXPORT Result : Metadata
	{
		Type* type { get };
		byte_t typeCompound { get };
		bool byValue { get };
		bool byRef { get };
		bool byPtr { get };
		bool isConstant { get };
#{
	public:
		Result(Type* type, bool constant, Passing passing, TypeCompound typeCompound);
	public:
		Type* m_type;
		bool m_constant;
		byte_t m_passing;
		byte_t m_typeCompound;
#}
	};

}
