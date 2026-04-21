#import "typedef.i"
#import "class_type.i"

#{
#include "utility.h"
#}

namespace pafcore
{
#{
	class ClassType;

	template<typename T>
	void DeleteObject(T* p);

	template<typename T>
	void DestroyArray(T* p);
#}

	nometa class #PAFCORE_EXPORT Interface
	{
#{
	public:
		virtual size_t getAddress() = 0;
	protected:
		virtual ~Interface()
		{}
#}
	};

#{
	template<>
	struct is_interface<Interface> : std::true_type
	{
	};
#}

	override class(value_object) #PAFCORE_EXPORT ObjectDebugTraveler
	{
		override abstract void onDebugTravel(Object* object);
	};

	class(noncopyable) #PAFCORE_EXPORT Object
	{
		nocode bool isTypeOf(ClassType* classType);
		nocode bool isStrictTypeOf(ClassType* classType);
		static void DebugTravel(ObjectDebugTraveler* traveler);
#{
#ifdef _DEBUG
	public:
		Object();
		Object(const Object&);
#endif
		template<typename T>
		friend void DeleteObject(T* p);
		template<typename T>
		friend void DestroyArray(T* p);
	protected:
		virtual ~Object();
	public:
		bool isTypeOf(ClassType* classType);

		template<typename T>
		bool isTypeOf()
		{
			return isTypeOf(T::GetType());
		}

		bool isStrictTypeOf(ClassType* classType)
		{
			return getType() == classType;
		}

		template<typename T>
		bool isStrictTypeOf()
		{
			return isStrictTypeOf(T::GetType());
		}

		void* castTo(ClassType* classType);

		template<typename T>
		T* castTo()
		{
			return reinterpret_cast<T*>(castTo(T::GetType()));
		}

	public:
		virtual bool serializable() const
		{
			return true;
		}

		virtual bool editable() const
		{
			return true;
		}
		
		virtual bool propertySerializable(const char* propertyName) const
		{
			return true;
		}

		virtual bool propertyEditable(const char* propertyName) const
		{
			return true;
		}

#}
	};
}
