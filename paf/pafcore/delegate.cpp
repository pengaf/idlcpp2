#include "delegate.h"
#include "delegate.mh"
#include "delegate.mc"
#include "delegate.ic"

#include "variant.h"
#include "instance_method.h"
#include "static_method.h"
#include "class_type.h"

BEGIN_PAFCORE

namespace
{
	void ReleaseCallBackStrong(CallBack* callBack)
	{
		if (0 == callBack)
		{
			return;
		}

		if (0 == DecStrong(callBack))
		{
			callBack->getType()->destroyInstance(callBack);
		}
	}
}

FunctionCallBack* FunctionCallBack::New(void* function, void* userData)
{
	FunctionCallBack* callBack = CreateObject<FunctionCallBack>();
	callBack->m_function = function;
	callBack->m_userData = userData;
	return callBack;
}

void FunctionCallBack::invoke(Variant* result, Variant** args, int_t numArgs)
{
	PAF_ASSERT(false);
}

bool FunctionCallBack::equal(CallBack* arg)
{
	PAF_ASSERT(arg->getType() == getType());
	FunctionCallBack* callBack = static_cast<FunctionCallBack*>(arg);
	return callBack->m_function == m_function && callBack->m_userData == m_userData;
}

InstanceMethodCallBack* InstanceMethodCallBack::New(InstanceMethod* instanceMethod, Object* object)
{
	InstanceMethodCallBack* callBack = CreateObject<InstanceMethodCallBack>();
	callBack->m_instanceMethod = instanceMethod;
	callBack->m_object = object;
	return callBack;
}

void InstanceMethodCallBack::invoke(Variant* result, Variant** args, int_t numArgs)
{
	Variant that;
	that.assignRcPtr(m_object, false, Variant::by_ptr);
	args[0] = &that;
	(*m_instanceMethod->m_invoker)(result, args, numArgs);
}

bool InstanceMethodCallBack::equal(CallBack* arg)
{
	PAF_ASSERT(arg->getType() == getType());
	InstanceMethodCallBack* callBack = static_cast<InstanceMethodCallBack*>(arg);
	return callBack->m_instanceMethod == m_instanceMethod && callBack->m_object == m_object;
}

StaticMethodCallBack* StaticMethodCallBack::New(StaticMethod* staticMethod)
{
	StaticMethodCallBack* callBack = CreateObject<StaticMethodCallBack>();
	callBack->m_staticMethod = staticMethod;
	return callBack;
}

void StaticMethodCallBack::invoke(Variant* result, Variant** args, int_t numArgs)
{
	(*m_staticMethod->m_invoker)(result, args + 1, numArgs - 1);
}

bool StaticMethodCallBack::equal(CallBack* arg)
{
	PAF_ASSERT(arg->getType() == getType());
	StaticMethodCallBack* callBack = static_cast<StaticMethodCallBack*>(arg);
	return callBack->m_staticMethod == m_staticMethod;
}


Delegate::Delegate() : 
	m_callBackList(0)
{
}

Delegate::~Delegate()
{
	CallBack* callBack = m_callBackList;
	while (callBack)
	{
		CallBack* next = callBack->m_next;
		ReleaseCallBackStrong(callBack);
		callBack = next;
	}
}

void Delegate::addCallBack(ObserverPtr<CallBack> callBack)
{
	if (callBack)
	{
		callBack->m_next = m_callBackList;
		IncStrong(callBack.get());
		m_callBackList = callBack;
	}
}

void Delegate::removeCallBack(ObserverPtr<CallBack> callBack)
{
	CallBack* current = m_callBackList;
	CallBack** prev = &m_callBackList;
	while (current)
	{
		if (current == callBack || current->equal(callBack))
		{
			*prev = current->m_next;
			ReleaseCallBackStrong(current);
			return;
		}
		prev = &current->m_next;
		current = current->m_next;
	}
}

ObserverPtr<InstanceMethodCallBack> Delegate::addInstanceMethod(ObserverPtr<Object> object, ObserverPtr<const char> instanceMethodName)
{
	ClassType* classType = object->getType();
	InstanceMethod* instanceMethod = classType->findInstanceMethod(instanceMethodName, true);
	if (instanceMethod)
	{
		InstanceMethodCallBack* callBack = InstanceMethodCallBack::New(instanceMethod, object);
		addCallBack(callBack);
		return callBack;
	}
	return nullptr;
}

void Delegate::removeInstanceMethod(ObserverPtr<Object> object, ObserverPtr<const char> instanceMethodName)
{
	ClassType* classType = object->getType();
	InstanceMethod* instanceMethod = classType->findInstanceMethod(instanceMethodName, true);
	if (instanceMethod)
	{
		InstanceMethodCallBack callBack;
		callBack.m_instanceMethod = instanceMethod;
		callBack.m_object = object;
		removeCallBack(&callBack);
	}
}

ObserverPtr<StaticMethodCallBack> Delegate::addStaticMethod(ObserverPtr<ClassType> classType, ObserverPtr<const char> staticMethodName)
{
	StaticMethod* staticMethod = classType->findStaticMethod(staticMethodName, true);
	if (staticMethod)
	{
		StaticMethodCallBack* callBack = StaticMethodCallBack::New(staticMethod);
		addCallBack(callBack);
		return callBack;
	}
	return nullptr;
}

void Delegate::removeStaticMethod(ObserverPtr<ClassType> classType, ObserverPtr<const char> staticMethodName)
{
	StaticMethod* staticMethod = classType->findStaticMethod(staticMethodName, true);
	if (staticMethod)
	{
		StaticMethodCallBack callBack;
		callBack.m_staticMethod = staticMethod;
		removeCallBack(&callBack);
	}
}

FunctionCallBack* Delegate::addFunction(void* function, void* userData)
{
	if (function)
	{
		FunctionCallBack* callBack = FunctionCallBack::New(function, userData);
		addCallBack(callBack);
		return callBack;
	}
	return 0;
}

void Delegate::removeFunction(void* function, void* userData)
{
	if (function)
	{
		FunctionCallBack callBack;
		callBack.m_function = function;
		callBack.m_userData = userData;
		removeCallBack(&callBack);
	}
}

END_PAFCORE

