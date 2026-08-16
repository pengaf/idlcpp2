#include "instance_method.h"
#include "instance_method.mh"
#include "instance_method.ic"
#include "instance_method.mc"
#include "parameter.h"

BEGIN_PAFCORE

InstanceMethod::InstanceMethod(const char* name, Attributes* attributes, FunctionInvoker invoker, Overload* overloads, uint32_t overloadCount)
: Metadata(name, attributes)
{
	m_invoker = invoker;
	m_overloads = overloads;
	m_overloadCount = overloadCount;
	m_maxNumResults = 0;
	for (uint32_t i = 0; i < overloadCount; i++)
	{
		if (m_overloads[i].m_resultCount > m_maxNumResults)
		{
			m_maxNumResults = m_overloads[i].m_resultCount;
		}
	}
}

uint32_t InstanceMethod::getResultCount(uint32_t overloadIndex)
{
	if(overloadIndex < m_overloadCount)
	{
		return m_overloads[overloadIndex].m_resultCount;
	}
	return 0;
}

Result* InstanceMethod::getResult(uint32_t overloadIndex, uint32_t index)
{
	if(overloadIndex < m_overloadCount)
	{
		if(index < m_overloads[overloadIndex].m_resultCount)
		{
			return &m_overloads[overloadIndex].m_results[index];
		}
	}
	return nullptr;
}

uint32_t InstanceMethod::getParameterCount(uint32_t overloadIndex)
{
	if (overloadIndex < m_overloadCount)
	{
		return m_overloads[overloadIndex].m_parameterCount;
	}
	return 0;
}


Parameter* InstanceMethod::getParameter(uint32_t overloadIndex, uint32_t index)
{
	if(overloadIndex < m_overloadCount)
	{
		if(index < m_overloads[overloadIndex].m_parameterCount)
		{
			return &m_overloads[overloadIndex].m_parameters[index];
		}
	}
	return nullptr;
}


END_PAFCORE
