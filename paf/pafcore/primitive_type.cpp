#include "primitive_type.h"
#include "primitive_type.mh"
#include "primitive_type.ic"
#include "primitive_type.mc"
#include "type_alias.h"
#include "auto_run.h"
#include <assert.h>
#include <algorithm>

BEGIN_PAFCORE

size_t PrimitiveType::_getMemberCount_() const
{
	return m_memberCount;
}

Metadata* PrimitiveType::_getMember_(size_t index) const
{
	if (index < m_memberCount)
	{
		return m_members[index];
	}
	return nullptr;
}

Metadata* PrimitiveType::_findMember_(string_t name) const
{
	Metadata dummy(name);
	Metadata** it = std::lower_bound(m_members, m_members + m_memberCount, &dummy, CompareMetaDataPtrByName());
	if (m_members + m_memberCount != it && strcmp(name, (*it)->m_name) == 0)
	{
		return *it;
	}
	return nullptr;
}

Metadata* PrimitiveType::findMember(const char* name) const
{
	return _findMember_(name);
}

//InstanceMethod* PrimitiveType::findInstanceMethod(const char* name)
//{
//	Metadata dummy(name);
//	InstanceMethod* res = std::lower_bound(m_instanceMethods, m_instanceMethods + m_instanceMethodCount, dummy);
//	if (m_instanceMethods + m_instanceMethodCount != res && strcmp(name, res->m_name) == 0)
//	{
//		return res;
//	}
//	return 0;
//}

StaticMethod* PrimitiveType::findStaticMethod(const char* name) const
{
	Metadata dummy(name);
	StaticMethod* res = std::lower_bound(m_staticMethods, m_staticMethods + m_staticMethodCount, dummy);
	if (m_staticMethods + m_staticMethodCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}

Metadata* PrimitiveType::findTypeMember(const char* name) const
{
	return findStaticMethod(name);
}

template<bool short_less_int>
struct TypePromoter
{
	static PrimitiveKind GetPromotedTypeKind(PrimitiveKind typeKind)
	{
		if (typeKind <= unsigned_short_type)
		{
			return int_type;
		}
		return typeKind;
	}
};

template<>
struct TypePromoter<false>
{
	static PrimitiveKind GetPromotedTypeKind(PrimitiveKind typeKind)
	{
		if (typeKind < unsigned_short_type)
		{
			return int_type;
		}
		else if (typeKind == unsigned_short_type)
		{
			return unsigned_int_type;
		}
		return typeKind;
	}
};

inline PrimitiveKind GetPromotedTypeKind(PrimitiveKind typeKind)
{
	return TypePromoter<sizeof(unsigned short) < sizeof(int)>::GetPromotedTypeKind(typeKind);
}

inline PrimitiveKind GetPromotedTypeKind(PrimitiveKind typeKind1, PrimitiveKind typeKind2)
{
	PrimitiveKind small, large; 
	if(typeKind1 < typeKind2)
	{
		small = typeKind1;
		large = typeKind2;
	}
	else
	{
		small = typeKind2;
		large = typeKind1;
	}
	if(unsigned_int_type == small && long_type == large)
	{
		return unsigned_long_type;
	}
	return GetPromotedTypeKind(large);
}

template<> BoolType				BoolType::				s_instance("bool");
template<> CharType				CharType::				s_instance("char");
template<> SignedCharType		SignedCharType::		s_instance("signed char");
template<> UnsignedCharType		UnsignedCharType::		s_instance("unsigned char");
template<> WcharType			WcharType::				s_instance("wchar_t");
template<> ShortType			ShortType::				s_instance("short");
template<> UnsignedShortType	UnsignedShortType::		s_instance("unsigned short");
template<> LongType				LongType::				s_instance("long");
template<> UnsignedLongType		UnsignedLongType::		s_instance("unsigned long");
template<> LongLongType			LongLongType::			s_instance("long long");
template<> UnsignedLongLongType	UnsignedLongLongType::	s_instance("unsigned long long");
template<> IntType				IntType::				s_instance("int");
template<> UnsignedIntType		UnsignedIntType::		s_instance("unsigned int");
template<> FloatType			FloatType::				s_instance("float");
template<> DoubleType			DoubleType::			s_instance("double");
template<> LongDoubleType		LongDoubleType::		s_instance("long double");
StringType						StringType::			s_instance("string_t");

END_PAFCORE

AUTO_REGISTER_TYPE(pafcore::BoolType)
AUTO_REGISTER_TYPE(pafcore::CharType)
AUTO_REGISTER_TYPE(pafcore::SignedCharType)
AUTO_REGISTER_TYPE(pafcore::UnsignedCharType)
AUTO_REGISTER_TYPE(pafcore::WcharType)
AUTO_REGISTER_TYPE(pafcore::ShortType)
AUTO_REGISTER_TYPE(pafcore::UnsignedShortType)
AUTO_REGISTER_TYPE(pafcore::LongType)
AUTO_REGISTER_TYPE(pafcore::UnsignedLongType)
AUTO_REGISTER_TYPE(pafcore::LongLongType)
AUTO_REGISTER_TYPE(pafcore::UnsignedLongLongType)
AUTO_REGISTER_TYPE(pafcore::IntType)
AUTO_REGISTER_TYPE(pafcore::UnsignedIntType)
AUTO_REGISTER_TYPE(pafcore::FloatType)
AUTO_REGISTER_TYPE(pafcore::DoubleType)
AUTO_REGISTER_TYPE(pafcore::StringType)
