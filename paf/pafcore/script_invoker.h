#pragma once

#include "variant.h"
#include "metadata.h"

BEGIN_PAFCORE

class ScriptInvoker
{
public:
	virtual ~ScriptInvoker(){}
	virtual ErrorCode invoke(const char* name, Variant* self, Variant* results, uint32_t numResults, Variant* arguments, uint32_t numArguments) = 0;
};

END_PAFCORE
