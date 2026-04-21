#import "type.i"

namespace pafcore
{
	abstract class(void_type)#PAFCORE_EXPORT VoidType : Type
	{
		static void* AddressToPtr(size_t address);
		static void* NullPtr { get };
#{
	public:
		VoidType();
	public:
		virtual Metadata* findMember(const char* name);
	public:
		static VoidType s_instance;
		static VoidType* GetSingleton()
		{
			return &s_instance;
		}
#}
	};
#{
	inline ObserverPtr<void> VoidType::AddressToPtr(size_t address)
	{
		return ObserverPtr<void>((void*)address);
	}

inline ObserverPtr<void> VoidType::NullPtr()
	{
		return nullptr;
	}
#}
}

#{
template<>
struct RuntimeTypeOf<void>
{
	typedef ::pafcore::VoidType RuntimeType;
	enum { type_category = ::pafcore::void_object };
};
#}
