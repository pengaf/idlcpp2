#include "overload.h"
#include "parameter.h"
#include "variant.h"
#include "type.h"
#include "primitive_type.h"
#include "class_type.h"
#include <assert.h>

BEGIN_PAFCORE


Overload::Overload(Result* results, uint32_t resultCount, Parameter* parameters, uint32_t parameterCount)
{
	m_results = results;
	m_parameters = parameters;
	m_resultCount = resultCount;
	m_parameterCount = parameterCount;
}

//const char nm = no_match;
//const char tc = type_conversion;
//const char tp = type_promotion;
//const char em = exact_match;
//const char m1 = sizeof(unsigned short) < sizeof(int) ? tp : tc;
//const char m2 = sizeof(unsigned short) < sizeof(int) ? tc : tp;
//
//char g_primitiveArgumentMatchTable[primitive_type_count][primitive_type_count] =
//{
//	{ em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//bool
//	{ tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//char
//	{ tc, tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//signed char
//	{ tc, tc, tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//unsigned char
//	{ tc, tc, tc, tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//wchar_t
//	{ tc, tc, tc, tc, tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//short
//	{ tc, tc, tc, tc, tc, tc, em, tc, tc, tc, tc, tc, tc, tc, tc, tc, nm},//unsigned short
//	{ tp, tp, tp, tp, tp, tp, m1, em, tc, tc, tc, tc, tc, tc, tc, tc, nm},//int
//	{ tc, tc, tc, tc, tc, tc, m2, tc, em, tc, tc, tc, tc, tc, tc, tc, nm},//unsigned int
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, em, tc, tc, tc, tc, tc, tc, nm},//long
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, em, tc, tc, tc, tc, tc, nm},//unsigned long
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, em, tc, tc, tc, tc, nm},//long long
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, em, tc, tc, tc, nm},//unsigned long long
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, em, tc, tc, nm},//float
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tp, em, tc, nm},//double
//	{ tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, tc, em, nm},//long double
//	{ nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, nm, em},//string_t
//};
//
//inline ArgumentMatch MatchPrimitive(PrimitiveKind dst, PrimitiveKind src)
//{
//	return static_cast<ArgumentMatch>(g_primitiveArgumentMatchTable[dst][src]);
//}

bool MatchPrimitive(PrimitiveKind dst, PrimitiveKind src)
{
	if (PrimitiveKind::bool_type < src && src < PrimitiveKind::float_type)
	{
		return PrimitiveKind::bool_type < dst && dst < PrimitiveKind::float_type;
	}
	else if (PrimitiveKind::float_type <= src && src <= PrimitiveKind::long_double_type)
	{
		return PrimitiveKind::float_type <= dst && dst <= PrimitiveKind::long_double_type;
	}
	else
	{
		return dst == src;
	}
}
/*
ArgumentMatch MatchArgumentByPassingValue(Type* dstType, Type* srcType)
{
	switch (dstType->m_kind)
	{
	case primitive_instance:
		switch (srcType->m_kind)
		{
		case primitive_instance:
			return MatchPrimitive(static_cast<PrimitiveType*>(dstType)->m_typeKind, static_cast<PrimitiveType*>(srcType)->m_typeKind);
		case enum_instance:
			return int_type == static_cast<PrimitiveType*>(dstType)->m_typeKind ? type_promotion : type_conversion;
		default:
			return no_match;
		}
	case enum_instance:
		switch (srcType->m_kind)
		{
		case primitive_instance:
			return type_conversion;
		case enum_instance:
			return dstType == srcType ? exact_match : no_match;
		default:
			return no_match;
		}
	case value_instance:
		if (dstType == srcType)
		{
			return exact_match;
		}
		else if (value_instance == srcType->m_kind &&
			static_cast<ClassType*>(srcType)->isType(static_cast<ClassType*>(dstType)))
		{
			return type_conversion;
		}
		else
		{
			return no_match;
		}
	default:
		assert(void_instance != dstType->m_kind);
		if (dstType == srcType)
		{
			return exact_match;
		}
		else if (dstType->m_kind == srcType->m_kind &&
			static_cast<ClassType*>(srcType)->isType(static_cast<ClassType*>(dstType)))
		{
			return type_conversion;
		}
		else
		{
			return no_match;
		}
	}
}

ArgumentMatch MatchArgumentByPassingPtr(Type* dstType, Type* srcType)
{
	switch (dstType->m_kind)
	{
	case void_instance:
		return dstType == srcType ? exact_match : type_conversion;
	case primitive_instance:
	case enum_instance:
		return dstType == srcType ? exact_match : no_match;
	case value_instance:
		if (dstType == srcType)
		{
			return exact_match;
		}
		else if(value_instance == srcType->m_kind &&
			static_cast<ClassType*>(srcType)->isType(static_cast<ClassType*>(dstType)))
		{
			return type_conversion;
		}
		else
		{
			return no_match;
		}
	default:
		if (dstType == srcType)
		{
			return exact_match;
		}
		else if (dstType->m_kind == srcType->m_kind &&
			static_cast<ClassType*>(srcType)->isType(static_cast<ClassType*>(dstType)))
		{
			return type_conversion;
		}
		else
		{
			return no_match;
		}
	}
};                                                                                                                     */                                                                                                                                

Overload::MatchKind MatchValue(const Parameter& parameter, Variant* argument)
{
	Type* paramType = parameter.type();
	Type* argType = argument->type();
	if (paramType->isPrimitive())
	{
		if (argType->isPrimitive())
		{
			PrimitiveKind paramKind = static_cast<PrimitiveType*>(paramType)->primitiveKind();
			PrimitiveKind argKind = static_cast<PrimitiveType*>(argType)->primitiveKind();
			return MatchPrimitive(paramKind, argKind) ? Overload::MatchKind::exact_match : Overload::MatchKind::compatible_match;
		}
		else if(argType->isEnum())
		{
			return Overload::MatchKind::compatible_match;
		}
		else
		{
			return Overload::MatchKind::no_match;
		}
	}
	else if (paramType->isEnum())
	{
		return paramType == argType ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
	}
	else
	{
		assert(paramType->isClass());
		if (argType->isClass())
		{
			return static_cast<ClassType*>(argType)->isType(static_cast<ClassType*>(paramType)) ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
		}
		else
		{
			return Overload::MatchKind::no_match;
		}
	}
}

Overload::MatchKind MatchObserverPtr(const Parameter& parameter, Variant* argument)
{
	TypeCompound typeCompound = argument->typeCompound();
	if (TypeCompound::observer_ptr == typeCompound || TypeCompound::shared_ptr == typeCompound)
	{
		Type* paramType = parameter.type();
		Type* argType = argument->type();
		if (paramType->isClass())
		{
			return static_cast<ClassType*>(argType)->isType(static_cast<ClassType*>(paramType)) ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
		}
		else
		{
			assert(paramType->isPrimitive() || paramType->isEnum());
			return paramType == argType ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
		}
	}
	else
	{
		return Overload::MatchKind::no_match;
	}
}

Overload::MatchKind MatchSharedPtr(const Parameter& parameter, Variant* argument)
{
	TypeCompound typeCompound = argument->typeCompound();
	if (TypeCompound::observer_ptr == typeCompound || TypeCompound::shared_ptr == typeCompound)
	{
		Type* paramType = parameter.type();
		Type* argType = argument->type();
		if (paramType->isClass())
		{
			return static_cast<ClassType*>(argType)->isType(static_cast<ClassType*>(paramType)) ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
		}
		else
		{
			assert(paramType->isPrimitive() || paramType->isEnum());
			return paramType == argType ? Overload::MatchKind::exact_match : Overload::MatchKind::no_match;
		}
	}
	else
	{
		return Overload::MatchKind::no_match;
	}
}

Overload::MatchKind MatchObserverArray(const Parameter& parameter, Variant* argument)
{
	Type* type = argument->type();
	TypeCompound typeCompound = argument->typeCompound();
	if (parameter.type() == type &&
		(TypeCompound::observer_array == typeCompound || TypeCompound::shared_array == typeCompound))
	{
		return Overload::MatchKind::exact_match;
	}
	else
	{
		return Overload::MatchKind::no_match;
	}
}

Overload::MatchKind MatchSharedArray(const Parameter& parameter, Variant* argument)
{
	Type* type = argument->type();
	TypeCompound typeCompound = argument->typeCompound();
	if (parameter.type() == type &&
		TypeCompound::shared_array == typeCompound)
	{
		return Overload::MatchKind::exact_match;
	}
	else
	{
		return Overload::MatchKind::no_match;
	}
}

Overload::MatchKind MatchArgument(const Parameter& parameter, Variant* argument)
{
	Overload::MatchKind matchKind = Overload::MatchKind::no_match;
	switch (parameter.m_typeCompound)
	{
	case TypeCompound::none:
		matchKind = MatchValue(parameter, argument);
		break;
	case TypeCompound::observer_ptr:
		matchKind = MatchObserverPtr(parameter, argument);
		break;
	case TypeCompound::shared_ptr:
		matchKind = MatchSharedPtr(parameter, argument);
		break;
	case TypeCompound::observer_array:
		matchKind = MatchObserverArray(parameter, argument);
		break;
	case TypeCompound::shared_array:
		matchKind = MatchSharedArray(parameter, argument);
		break;
	}
	return matchKind;
}

Overload::MatchKind Overload::matchArguments(Variant** arguments)
{
	bool exactMatch = true;
	for (size_t i = 0; i < m_parameterCount; ++i)
	{
		Overload::MatchKind matchKind = MatchArgument(m_parameters[i], arguments[i]);
		if (Overload::MatchKind::no_match == matchKind)
		{
			return Overload::MatchKind::no_match;
		}
		else if (Overload::MatchKind::compatible_match == matchKind)
		{
			exactMatch = false;
		}
	}
	return exactMatch ? Overload::MatchKind::exact_match : Overload::MatchKind::compatible_match;
}

uint32_t Overload::Resolve(Overload* overloads, uint32_t overloadCount, Variant** arguments, uint32_t argumentCount)
{
	uint32_t compatibleIndex = overloadCount;
	for (uint32_t i = 0; i < overloadCount; ++i)
	{
		assert(argumentCount == overloads[i].m_parameterCount);
		MatchKind matchKind = overloads[i].matchArguments(arguments);
		switch(matchKind)
		{
		case MatchKind::exact_match:
			return i;
			break;
		case MatchKind::compatible_match:
			if (compatibleIndex == overloadCount)
			{
				compatibleIndex = i;
			}
			break;
		}
	}
	return compatibleIndex;
}

END_PAFCORE
