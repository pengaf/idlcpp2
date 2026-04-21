#import "type.i"

namespace pafcore
{
	abstract class(function_argument)#PAFCORE_EXPORT Argument : Metadata
	{
		Type* type { get };
		byte_t typeCompound { get };
		bool byValue { get };
		bool byRef { get };
		bool byPtr { get };
		bool isOutput { get };
		bool isConstant { get };
#{
	public:
		Argument(const char* name, Type* type, Passing passing, TypeCompound typeCompound, byte_t out, bool constant);
	public:
		Type* m_type;
		Passing m_passing;
		byte_t m_typeCompound;
		byte_t m_out;
		bool m_constant;
#}
	};
}
