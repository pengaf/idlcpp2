#import "typedef.i"
#import "class_type.i"

#{
#include "ref_count.h"
#}

namespace pafcore
{
#{
	class ClassType;
#}

	override class(value_object) #PAFCORE_EXPORT ReferenceDebugTraveler
	{
		override abstract void onDebugTravel(Reference* reference);
	};

	class(noncopyable) #PAFCORE_EXPORT Reference ## : public RefCount
	{ 
		nocode bool isTypeOf(ClassType* classType);
		nocode bool isStrictTypeOf(ClassType* classType);
		static void DebugTravel(ReferenceDebugTraveler* traveler);
#{
#ifdef _DEBUG
	public:
		Reference();
		Reference(const Reference&);
		~Reference();
#endif
	//	Reference() = default;
	//	Reference(const Reference&) = delete;
	//	Reference& operator=(const Reference&) = delete;
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

		void deleteThis()
		{
			paf_delete(this);
		}

#}
	};
}
