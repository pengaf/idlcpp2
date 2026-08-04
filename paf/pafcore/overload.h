#pragma once

#include "utility.h"
#include "metadata.h"

BEGIN_PAFCORE

class Result;
class Parameter;
class Variant;

struct PAFCORE_EXPORT Overload
{
	Result* m_results;
	Parameter* m_parameters;
	uint32_t m_resultCount;
	uint32_t m_parameterCount;
public:
	Overload(Result* results, uint32_t resultCount, Parameter* parameters, uint32_t parameterCount);
public:
	enum class MatchKind
	{
		no_match,
		exact_match,
		compatible_match,
	};
	MatchKind matchArguments(Variant** arguments);
public:
	static uint32_t Resolve(Overload* overloads, uint32_t overloadCount, Variant** arguments, uint32_t argumentCount);
};

typedef ErrorCode(*FunctionInvoker)(Variant** results, int_t numResults, Variant** arguments, int_t numArguments);

END_PAFCORE