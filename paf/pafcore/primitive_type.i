#import "type.i"

#{
#include "variant.h"
#include "parameter.h"
#include "result.h"
#include "instance_method.h"
#include "static_method.h"
#include "name_space.h"

#pragma warning( push )
#pragma warning( disable : 4804 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4146 )
#}

namespace pafcore
{
#{
	enum PrimitiveKind
	{
		bool_type,
		char_type,
		signed_char_type,
		unsigned_char_type,
		wchar_type,
		short_type,
		unsigned_short_type,
		int_type,
		unsigned_int_type,
		long_type,
		unsigned_long_type,
		long_long_type,
		unsigned_long_long_type,
		float_type,
		double_type,
		long_double_type,
		string_type,
		primitive_type_count,
	};	
#}

	class(primitive_type)#PAFCORE_EXPORT PrimitiveType : Type
	{
		size_t _getMemberCount_() const;
		Metadata* _getMember_(size_t index) const;
		Metadata* _findMember_(string_t name) const;
#{
	public:
		PrimitiveType(const char* name, uint32_t size) : Type(name, MetadataKind::primitive_instance, RefCountPolicy::single_thread, size, "")
		{}
	public:
		virtual bool castTo(void* dst, const Type* dstType, const void* src) = 0;
	public:
		//InstanceMethod * findInstanceMethod(const char* name);
		StaticMethod* findStaticMethod(const char* name) const;
		Metadata* findTypeMember(const char* name) const;
		virtual Metadata* findMember(const char* name) const override;
	public:
		PrimitiveKind primitiveKind() const
		{
			return m_primitiveKind;
		}
		bool isString() const
		{
			return (string_type == m_primitiveKind);
		}
		StaticMethod* staticMethods() const
		{
			return m_staticMethods;
		}
	protected:
		PrimitiveKind m_primitiveKind;
		Metadata** m_members;
		size_t m_memberCount;
		StaticMethod* m_staticMethods;
		size_t m_staticMethodCount;
#}
	};
#{

	PrimitiveType* GetBoolRuntimeType();
	PrimitiveType* GetUnsignedIntRuntimeType();


	template<typename T>
	struct PAFCORE_EXPORT PrimitiveTypeTraits
	{
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<bool>
	{
		enum { type_kind = bool_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<char>
	{
		enum { type_kind = char_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned char>
	{
		enum { type_kind = unsigned_char_type };
	};


	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<signed char>
	{
		enum { type_kind = signed_char_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<wchar_t>
	{
		enum { type_kind = wchar_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<short>
	{
		enum { type_kind = short_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned short>
	{
		enum { type_kind = unsigned_short_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<int>
	{
		enum { type_kind = int_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned int>
	{
		enum { type_kind = unsigned_int_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long>
	{
		enum { type_kind = long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long>
	{
		enum { type_kind = unsigned_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long long>
	{
		enum { type_kind = long_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long long>
	{
		enum { type_kind = unsigned_long_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<float>
	{
		enum { type_kind = float_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<double>
	{
		enum { type_kind = double_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long double>
	{
		enum { type_kind = long_double_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<string_t>
	{
		enum { type_kind = string_type };
	};


	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl : public PrimitiveType
	{
	public:
		PrimitiveTypeImpl(const char* name) : PrimitiveType(name, sizeof(T))
		{
			m_primitiveKind = (PrimitiveKind)PrimitiveTypeTraits<T>::type_kind;

			static ::pafcore::Parameter s_staticArguments[] =
			{
				::pafcore::Parameter("address", RuntimeTypeOf<::size_t>::RuntimeType::GetSingleton(), ::pafcore::TypeCompound::none, false, false),
				::pafcore::Parameter("arg", this, ::pafcore::TypeCompound::none, false, false),
				::pafcore::Parameter("address", RuntimeTypeOf<::size_t>::RuntimeType::GetSingleton(), ::pafcore::TypeCompound::none, false, false),
				::pafcore::Parameter("count", RuntimeTypeOf<::size_t>::RuntimeType::GetSingleton(), ::pafcore::TypeCompound::none, false, false),
			};
			static ::pafcore::Overload s_staticOverloads[] =
			{
				::pafcore::Overload(nullptr, 0, &s_staticArguments[0], 1),
				::pafcore::Overload(nullptr, 0, &s_staticArguments[0], 2),
				::pafcore::Overload(nullptr, 0, &s_staticArguments[2], 2),
			};
			static ::pafcore::StaticMethod s_staticMethods[] =
			{
				::pafcore::StaticMethod("Construct", nullptr, Primitive_Construct, &s_staticOverloads[0], 2),
				::pafcore::StaticMethod("ConstructArray", nullptr, Primitive_ConstructArray, &s_staticOverloads[2], 1),
			};
			m_staticMethods = s_staticMethods;
			m_staticMethodCount = paf_array_size_of(s_staticMethods);
			static Metadata* s_members[] =
			{
				&s_staticMethods[0],
				&s_staticMethods[1],
			};
			m_members = s_members;
			m_memberCount = paf_array_size_of(s_members);
			NameSpace::GetGlobalNameSpace()->registerMember(this);
		}

		static ErrorCode Primitive_Construct(Variant** results, uint32_t& numResults, Variant** arguments, uint32_t numArguments)
		{
			if(1 <= numArguments)
			{
				::size_t a0;
				if (!arguments[0]->castToPrimitive<::size_t>(a0))
				{
					return ::pafcore::ErrorCode::e_invalid_arg_type_1;
				}
				T a1 = 0;
				if (1 < numArguments)
				{
					if (!arguments[1]->castToPrimitive(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &a1))
					{
						return ErrorCode::e_invalid_arg_type_2;
					}
				}
				::pafcore::Construct<T>((void*)a0, a1);
				numResults = 0;
				return ::pafcore::ErrorCode::s_ok;
			}
			return ErrorCode::e_invalid_arg_num;
		}

		static ErrorCode Primitive_ConstructArray(Variant** results, uint32_t& numResults, Variant** arguments, uint32_t numArguments)
		{
			if (2 <= numArguments)
			{
				::size_t a0;
				if (!arguments[0]->castToPrimitive<::size_t>(a0))
				{
					return ::pafcore::ErrorCode::e_invalid_arg_type_1;
				}
				::size_t a1;
				if (!arguments[1]->castToPrimitive<::size_t>(a1))
				{
					return ::pafcore::ErrorCode::e_invalid_arg_type_2;
				}
				::pafcore::ConstructArray<T>((void*)a0, a1);
				numResults = 0;
				return ::pafcore::ErrorCode::s_ok;
			}
			return ::pafcore::ErrorCode::e_invalid_arg_num;
		}

		virtual bool copyConstruct(void* dst, const void* src, size_t count) const override
		{
			return CopyConstruct(reinterpret_cast<T*>(dst), reinterpret_cast<const T*>(src), count);
		}

		virtual bool moveConstruct(void* dst, void* src, size_t count) const override
		{
			return MoveConstruct(reinterpret_cast<T*>(dst), reinterpret_cast<T*>(src), count);
		}

		virtual bool copyAssign(void* dst, const void* src, size_t count) const override
		{
			return CopyAssign(reinterpret_cast<T*>(dst), reinterpret_cast<const T*>(src), count);
		}

		virtual bool moveAssign(void* dst, void* src, size_t count) const override
		{
			return MoveAssign(reinterpret_cast<T*>(dst), reinterpret_cast<T*>(src), count);
		}

		virtual bool destruct(void* ptr, size_t count) const override
		{
			return true;
		}

		virtual bool castTo(void* dst, const Type* dstType, const void* src)
		{
			if(!dstType->isPrimitive())
			{
				return false;
			}
			switch (static_cast<const PrimitiveType*>(dstType)->primitiveKind())
			{
			case bool_type:
				*reinterpret_cast<bool_t*>(dst) = *reinterpret_cast<const T*>(src) != 0;
				break;
			case char_type:
				*reinterpret_cast<char_t*>(dst) = static_cast<char_t>(*reinterpret_cast<const T*>(src));
				break;
			case signed_char_type:
				*reinterpret_cast<schar_t*>(dst) = static_cast<char_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_char_type:
				*reinterpret_cast<uchar_t*>(dst) = static_cast<uchar_t>(*reinterpret_cast<const T*>(src));
				break;
			case wchar_type:
				*reinterpret_cast<wchar_t*>(dst) = static_cast<wchar_t>(*reinterpret_cast<const T*>(src));
				break;
			case short_type:
				*reinterpret_cast<short_t*>(dst) = static_cast<short_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_short_type:
				*reinterpret_cast<ushort_t*>(dst) = static_cast<ushort_t>(*reinterpret_cast<const T*>(src));
				break;
			case int_type:
				*reinterpret_cast<int_t*>(dst) = static_cast<int_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_int_type:
				*reinterpret_cast<uint_t*>(dst) = static_cast<uint_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_type:
				*reinterpret_cast<long_t*>(dst) = static_cast<long_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_long_type:
				*reinterpret_cast<ulong_t*>(dst) = static_cast<ulong_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_long_type:
				*reinterpret_cast<longlong_t*>(dst) = static_cast<longlong_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_long_long_type:
				*reinterpret_cast<ulonglong_t*>(dst) = static_cast<ulonglong_t>(*reinterpret_cast<const T*>(src));
				break;
			case float_type:
				*reinterpret_cast<float_t*>(dst) = static_cast<float_t>(*reinterpret_cast<const T*>(src));
				break;
			case double_type:
				*reinterpret_cast<double_t*>(dst) = static_cast<double_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_double_type:
				*reinterpret_cast<longdouble_t*>(dst) = static_cast<longdouble_t>(*reinterpret_cast<const T*>(src));
				break;
			default:
				return false;
			}
			return true;
		}
		static PrimitiveTypeImpl s_instance;
		static PrimitiveTypeImpl* GetSingleton()
		{
			return &s_instance;
		}
	};

	typedef PrimitiveTypeImpl<bool>					BoolType;
	typedef PrimitiveTypeImpl<char>					CharType;
	typedef PrimitiveTypeImpl<signed char>			SignedCharType;
	typedef PrimitiveTypeImpl<unsigned char>		UnsignedCharType;
	typedef PrimitiveTypeImpl<wchar_t>				WcharType;
	typedef PrimitiveTypeImpl<short>				ShortType;
	typedef PrimitiveTypeImpl<unsigned short>		UnsignedShortType;
	typedef PrimitiveTypeImpl<long>					LongType;
	typedef PrimitiveTypeImpl<unsigned long>		UnsignedLongType;
	typedef PrimitiveTypeImpl<long long>			LongLongType;
	typedef PrimitiveTypeImpl<unsigned long long>	UnsignedLongLongType;
	typedef PrimitiveTypeImpl<int>					IntType;
	typedef PrimitiveTypeImpl<unsigned int>			UnsignedIntType;
	typedef PrimitiveTypeImpl<float>				FloatType;
	typedef PrimitiveTypeImpl<double>				DoubleType;
	typedef PrimitiveTypeImpl<long double>			LongDoubleType;

	inline PrimitiveType* GetBoolRuntimeType()
	{
		return BoolType::GetSingleton();
	}

	inline PrimitiveType* GetUnsignedIntRuntimeType()
	{
		return UnsignedIntType::GetSingleton();
	}



	class PAFCORE_EXPORT StringType : public PrimitiveType
	{
	public:
		StringType(const char* name) : PrimitiveType(name, sizeof(::string_t))
		{
			m_primitiveKind = (PrimitiveKind)PrimitiveTypeTraits<::string_t>::type_kind;

			static ::pafcore::Result s_staticResults[] =
			{
				::pafcore::Result("result", this, pafcore::TypeCompound::shared_ptr, false),
				::pafcore::Result("result", this, pafcore::TypeCompound::shared_ptr, false),
			};
			static ::pafcore::Parameter s_staticArguments[] =
			{
				::pafcore::Parameter("str", this, pafcore::TypeCompound::observer_ptr, false, false),
			};
			static ::pafcore::Overload s_staticOverloads[] =
			{
				::pafcore::Overload(&s_staticResults[0], 1, nullptr, 0),
				::pafcore::Overload(&s_staticResults[1], 1, &s_staticArguments[0], 1),
			};
			static ::pafcore::StaticMethod s_staticMethods[] =
			{
				::pafcore::StaticMethod("New", nullptr, string_t_New, &s_staticOverloads[0], 2),
			};
			m_staticMethods = s_staticMethods;
			m_staticMethodCount = paf_array_size_of(s_staticMethods);
			static Metadata* s_members[] =
			{
				&s_staticMethods[0],
			};
			m_members = s_members;
			m_memberCount = paf_array_size_of(s_members);
			::pafcore::NameSpace::GetGlobalNameSpace()->registerMember(this);
		}
		static ErrorCode string_t_New(::pafcore::Variant** results, uint32_t& numResults, ::pafcore::Variant** arguments, uint32_t numArguments)
		{
			if (0 == numArguments)
			{
				string_t str;
				results[0]->assignPrimitive(str);
				numResults = 1;
				return ::pafcore::ErrorCode::s_ok;
			}
			if (1 <= numArguments)
			{
				string_t a0;
				if (!arguments[0]->castToPrimitive(a0))
				{
					return ::pafcore::ErrorCode::e_invalid_arg_type_1;
				}
				results[0]->assignPrimitive(a0);
				numResults = 1;
				return ::pafcore::ErrorCode::s_ok;
			}
			return ::pafcore::ErrorCode::e_invalid_arg_num;
		}

		virtual bool copyConstruct(void* dst, const void* src, size_t count) const override
		{
			return CopyConstruct(reinterpret_cast<string_t*>(dst), reinterpret_cast<const string_t*>(src), count);
		}

		virtual bool moveConstruct(void* dst, void* src, size_t count) const override
		{
			return MoveConstruct(reinterpret_cast<string_t*>(dst), reinterpret_cast<string_t*>(src), count);
		}

		virtual bool copyAssign(void* dst, const void* src, size_t count) const override
		{
			return CopyAssign(reinterpret_cast<string_t*>(dst), reinterpret_cast<const string_t*>(src), count);
		}

		virtual bool moveAssign(void* dst, void* src, size_t count) const override
		{
			return MoveAssign(reinterpret_cast<string_t*>(dst), reinterpret_cast<string_t*>(src), count);
		}

		virtual bool destruct(void* ptr, size_t count) const override
		{
			return true;
		}

		virtual bool castTo(void* dst, const Type* dstType, const void* src)
		{
			if (!dstType->isPrimitive())
			{
				return false;
			}
			if (string_type != static_cast<const PrimitiveType*>(dstType)->primitiveKind())
			{
				return false;
			}
			*reinterpret_cast<::string_t*>(dst) = (*reinterpret_cast<const ::string_t*>(src));
			return true;
		}

	public:
		static StringType s_instance;
		static StringType* GetSingleton()
		{
			return &s_instance;
		}
	};


#}
}

#{
template<>
struct RuntimeTypeOf<bool>
{
	typedef ::pafcore::BoolType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<char>
{
	typedef ::pafcore::CharType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<signed char>
{
	typedef ::pafcore::SignedCharType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<unsigned char>
{
	typedef ::pafcore::UnsignedCharType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<wchar_t>
{
	typedef ::pafcore::WcharType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<short>
{
	typedef ::pafcore::ShortType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<unsigned short>
{
	typedef ::pafcore::UnsignedShortType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<long>
{
	typedef ::pafcore::LongType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<unsigned long>
{
	typedef ::pafcore::UnsignedLongType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<long long>
{
	typedef ::pafcore::LongLongType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<unsigned long long>
{
	typedef ::pafcore::UnsignedLongLongType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<int>
{
	typedef ::pafcore::IntType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<unsigned int>
{
	typedef ::pafcore::UnsignedIntType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<float>
{
	typedef ::pafcore::FloatType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<double>
{
	typedef ::pafcore::DoubleType RuntimeType;
	enum {type_kind = ::pafcore::MetadataKind::primitive_instance};
};

template<>
struct RuntimeTypeOf<long double>
{
	typedef ::pafcore::LongDoubleType RuntimeType;
	enum { type_kind = ::pafcore::MetadataKind::primitive_instance };
};

template<>
struct RuntimeTypeOf<string_t>
{
	typedef ::pafcore::StringType RuntimeType;
	enum { type_kind = ::pafcore::MetadataKind::primitive_instance };
};

template<typename T>
struct RuntimeTypeOf<T*>
{
	typedef RuntimeTypeOf<size_t>::RuntimeType RuntimeType;
	enum { type_kind = ::pafcore::MetadataKind::primitive_instance };
};

#pragma warning( pop ) 
#}
