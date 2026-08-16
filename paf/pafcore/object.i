#{
#include "utility.h"
#}

#import "typedef.i"
#import "class_type.i"

namespace pafcore
{
#{
	class ClassType;
#}

	class(noncopyable) #PAFCORE_EXPORT Introspectable
	{
		nocode bool isTypeOf(ClassType * classType);
		nocode bool isStrictTypeOf(ClassType* classType);
#{
	public:
		virtual ~Introspectable() = default;
		virtual ClassType* getType() = 0;
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
#}
	};


	nometa class #PAFCORE_EXPORT Interface : Introspectable
	{
#{
	public:
		virtual size_t getAddress() = 0;
	public:
		uint32_t incStrongRefCount() noexcept;
		uint32_t incWeakRefCount() noexcept;
		uint32_t decStrongRefCount() noexcept;
		uint32_t decWeakRefCount() noexcept;
		uint32_t getStrongRefCount() noexcept;
		uint32_t getWeakRefCount() noexcept;
	protected:
		virtual ~Interface()
		{}
#}
	};

//#{
//	template<>
//	struct is_interface<Interface> : std::true_type
//	{};
//#}

	class #PAFCORE_EXPORT Object : Introspectable
	{
		nocode bool isTypeOf(ClassType* classType);
		nocode bool isStrictTypeOf(ClassType* classType);
#{
	protected:
		enum class RefCountOp
		{
			inc_strong,
			inc_weak,
			dec_strong,
			dec_weak,
			get_strong,
			get_weak,
		};
		virtual uint32_t refCountOperation(RefCountOp op) noexcept { return 0; }
	public:
		uint32_t incStrongRefCount() noexcept
		{
			return refCountOperation(RefCountOp::inc_strong);
		}

		uint32_t decStrongRefCount() noexcept
		{
			return refCountOperation(RefCountOp::dec_strong);
		}

		uint32_t incWeakRefCount() noexcept
		{
			return refCountOperation(RefCountOp::inc_weak);
		}

		uint32_t decWeakRefCount() noexcept
		{
			return refCountOperation(RefCountOp::dec_weak);
		}

		uint32_t getStrongRefCount() noexcept
		{
			return refCountOperation(RefCountOp::get_strong);
		}

		uint32_t getWeakRefCount() noexcept
		{
			return refCountOperation(RefCountOp::get_weak);
		}
#}
	};

	class #PAFCORE_EXPORT STRCObject : Object
	{
#{
	public:
		uint32_t incStrongRefCount() noexcept;
		uint32_t incWeakRefCount() noexcept;
		uint32_t decStrongRefCount() noexcept;
		uint32_t decWeakRefCount() noexcept;
		uint32_t getStrongRefCount() noexcept;
		uint32_t getWeakRefCount() noexcept;
	protected:
		virtual uint32_t refCountOperation(RefCountOp op) noexcept override;
#}
	};

	class #PAFCORE_EXPORT MTRCObject : Object
	{
#{
	public:
		uint32_t incStrongRefCount() noexcept;
		uint32_t incWeakRefCount() noexcept;
		uint32_t decStrongRefCount() noexcept;
		uint32_t decWeakRefCount() noexcept;
		uint32_t getStrongRefCount() noexcept;
		uint32_t getWeakRefCount() noexcept;
	protected:
		virtual uint32_t refCountOperation(RefCountOp op) noexcept override;
#}
	};


#{
	inline uint32_t Interface::incStrongRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->incStrongRefCount();
	}

	inline uint32_t Interface::incWeakRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->incWeakRefCount();
	}

	inline uint32_t Interface::decStrongRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->decStrongRefCount();
	}

	inline uint32_t Interface::decWeakRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->decWeakRefCount();
	}

	inline uint32_t Interface::getStrongRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->getStrongRefCount();
	}

	inline uint32_t Interface::getWeakRefCount() noexcept
	{
		Object* object = reinterpret_cast<Object*>(getAddress());
		return object->getWeakRefCount();
	}
#}

}
