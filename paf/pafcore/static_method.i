#import "parameter.i"
#import "result.i"

###include "overload.h"

namespace pafcore
{

#{
	struct Attributes;
#}

	class(static_method)#PAFCORE_EXPORT StaticMethod : Metadata
	{
		uint32_t overloadCount { get };
		uint32_t getResultCount(uint32_t overloadIndex);
		Result* getResult(uint32_t overloadIndex, uint32_t index);
		uint32_t getParameterCount(uint32_t overloadIndex);
		Parameter* getParameter(uint32_t overloadIndex, uint32_t index);
#{
	public:
		StaticMethod(const char* name, Attributes* attributes, FunctionInvoker invoker, Overload* overloads, uint32_t overloadCount);
	public:
		FunctionInvoker m_invoker;
		Overload* m_overloads;
		uint32_t m_overloadCount;
#}
	};

}