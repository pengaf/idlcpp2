#import "object.i"
#import "class_type.i"

#{
#include "utility.h"
#}

namespace pafcore
{

#{
	class Object;
	class InstanceMethod;
	class StaticMethod;
	class Variant;
#}

	class #PAFCORE_EXPORT CallBack : Object
	{
#{
		friend struct Delegate;
	public:
		virtual void invoke(Variant* result, Variant** args, int_t numArgs) = 0;
		virtual bool equal(CallBack* arg) = 0;
		CallBack* getNext()
		{
			return m_next;
		}
	protected:
		CallBack* m_next;
#}
	};

	class #PAFCORE_EXPORT FunctionCallBack : CallBack
	{
#{
		friend struct Delegate;
	public:
		static FunctionCallBack* New(void* function, void* userData);
	public:
		virtual void invoke(Variant* result, Variant** args, int_t numArgs);
		virtual bool equal(CallBack* arg);
	public:
		void* m_function;
		void* m_userData;
#}
	};

	class #PAFCORE_EXPORT InstanceMethodCallBack : CallBack
	{
#{
		friend struct Delegate;
	public:
		static InstanceMethodCallBack* New(InstanceMethod* instanceMethod, Object* object);
	public:
		virtual void invoke(Variant* result, Variant** args, int_t numArgs);
		virtual bool equal(CallBack* arg);
	protected:
		InstanceMethod* m_instanceMethod;
		Object* m_object;
#}
	};

	class #PAFCORE_EXPORT StaticMethodCallBack : CallBack
	{
#{
		friend struct Delegate;
	public:
		static StaticMethodCallBack* New(StaticMethod* staticMethod);
	public:
		virtual void invoke(Variant* result, Variant** args, int_t numArgs);
		virtual bool equal(CallBack* arg);
	protected:
		StaticMethod* m_staticMethod;
#}
	};

	struct #PAFCORE_EXPORT Delegate
	{
		InstanceMethodCallBack* addInstanceMethod(Object* object, const char* instanceMethodName);
		void removeInstanceMethod(Object* object, const char* instanceMethodName);
		StaticMethodCallBack* addStaticMethod(ClassType* classType, const char* staticMethodName);
		void removeStaticMethod(ClassType* classType, const char* staticMethodName);
		void addCallBack(CallBack* callBack);
		void removeCallBack(CallBack* callBack);
#{
	public:
		Delegate();
		~Delegate();
	public:
		FunctionCallBack* addFunction(void* function, void* userData);
		void removeFunction(void* function, void* userData);
		CallBack* getCallBack()
		{
			return m_callBackList;
		}
	protected:
		CallBack* m_callBackList;
#} 
	};


	delegate #PAFCORE_EXPORT void EventHandler(Object* sender);
}
