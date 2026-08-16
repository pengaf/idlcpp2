#include "paflua.h"
#include "../pafcore/variant.h"
#include "../pafcore/name_space.h"
#include "../pafcore/name_space.mh"
#include "../pafcore/metadata.mh"
#include "../pafcore/type.mh"
#include "../pafcore/class_type.h"
#include "../pafcore/enum_type.h"
#include "../pafcore/enum_type.mh"
#include "../pafcore/instance_field.h"
#include "../pafcore/static_field.h"
#include "../pafcore/instance_property.h"
#include "../pafcore/static_property.h"
#include "../pafcore/instance_method.h"
#include "../pafcore/static_method.h"
#include "../pafcore/enum_member.h"
#include "../pafcore/primitive_type.h"
#include "../pafcore/iterator.mh"
#include "../pafcore/reflection.h"
#include "common_func.h"
#include "instance_field.h"
#include "static_field.h"
#include "instance_property.h"
#include "static_property.h"
#include "script_invoker.h"

#include <new>
#include <cstdint>
#include <string.h>
#include <assert.h>


BEGIN_PAFLUA


const uint32_t max_param_count = 20;
const char* g_variantMetatableName = "idlcpp.Variant";

int Variant_GC(lua_State *L) 
{
	pafcore::Variant* variant = (pafcore::Variant*)lua_touserdata(L, 1);
	variant->~Variant();
	return 0;
}

pafcore::ErrorCode GetPrimitive(lua_State *L, pafcore::Variant* variant)
{
	assert(variant->type()->isPrimitive());
	if (variant->typeCompound() == pafcore::TypeCompound::none)
	{
		pafcore::PrimitiveType* primitiveType = static_cast<pafcore::PrimitiveType*>(variant->type());
		switch (primitiveType->primitiveKind())
		{
		case pafcore::string_type: {
			string_t value;
			variant->castToPrimitive(value);
			lua_pushstring(L, value.c_str());
			break;}
		case pafcore::float_type:
		case pafcore::double_type: {
			lua_Number value;
			variant->castToPrimitive(value);
			lua_pushnumber(L, value);
			break;}
		case pafcore::bool_type: {
			bool value;
			variant->castToPrimitive(value);
			lua_pushboolean(L, value ? 1 : 0);
			break;}
		default: {
			lua_Integer value;
			variant->castToPrimitive(value);
			lua_pushinteger(L, value);
			break;}
		}
		return pafcore::ErrorCode::s_ok;
	}
	return pafcore::ErrorCode::e_invalid_type;
}

pafcore::ErrorCode SetPrimitive(lua_State* L, pafcore::Variant* variant)
{
	assert(!variant->isNull() && variant->type()->isPrimitive());
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	if (value->castToPrimitive(variant->type(), (void*)variant->pointer()))
	{
		return pafcore::ErrorCode::s_ok;
	}
	return pafcore::ErrorCode::e_invalid_property_type;
}

pafcore::ErrorCode GetEnum(lua_State* L, pafcore::Variant* variant)
{
	assert(variant->type()->isEnum());
	if (variant->typeCompound() == pafcore::TypeCompound::none)
	{
		lua_Integer value;
		variant->castToPrimitive(value);
		lua_pushinteger(L, value);
		return pafcore::ErrorCode::s_ok;
	}	
	return pafcore::ErrorCode::e_invalid_type;
}

pafcore::ErrorCode SetEnum(lua_State* L, pafcore::Variant* variant)
{
	assert(!variant->isNull() && variant->type()->isEnum());
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);
	if (value->castToEnum(variant->type(), (void*)variant->pointer()))
	{
		return pafcore::ErrorCode::s_ok;
	}
	return pafcore::ErrorCode::e_invalid_property_type;
}

int InvokeFunction(lua_State *L, pafcore::FunctionInvoker invoker, uint32_t numResults, uint32_t numArguments, int startIndex)
{
	alignas(pafcore::Variant) char argumentsBuf[sizeof(pafcore::Variant) * max_param_count];
	pafcore::Variant* args[max_param_count]; 
	if(numArguments > max_param_count)
	{
		numArguments = max_param_count;
	}
	for (uint32_t i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant)*i];
		new(argument)pafcore::Variant;
		args[i] = LuaToVariant(argument, L, i + startIndex);
	}
	alignas(pafcore::Variant) char resultsBuf[sizeof(pafcore::Variant) * max_param_count];
	pafcore::Variant* ress[max_param_count];
	if (numResults > max_param_count)
	{
		numResults = max_param_count;
	}
	for (uint32_t i = 0; i < numResults; i++)
	{
		pafcore::Variant* result = (pafcore::Variant*)&resultsBuf[sizeof(pafcore::Variant) * i];
		new(result)pafcore::Variant;
		ress[i] = result;
	}

	pafcore::ErrorCode errorCode = (*invoker)(ress, numResults, args, numArguments);
	if(pafcore::ErrorCode::s_ok == errorCode)
	{
		for (uint32_t i = 0; i < numResults; i++)
		{
			VariantToLua(L, ress[i]);
		}
	}

	for (uint32_t i = 0; i < numResults; i++)
	{
		pafcore::Variant* result = (pafcore::Variant*)&resultsBuf[sizeof(pafcore::Variant) * i];
		result->~Variant();
	}
	for (uint32_t i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant) * i];
		argument->~Variant();
	}

	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		return numResults;
	}
	else
	{
		Variant_Error(L, "", errorCode);
		return 0;
	}
}

int InvokeFunction_Method(lua_State *L, pafcore::FunctionInvoker invoker, uint32_t maxNumResults)
{
	uint32_t numArguments = (uint32_t)lua_gettop(L) - 1;
	return InvokeFunction(L, invoker, maxNumResults, numArguments, 2);
}

int InstanceMethod_Closure(lua_State* L)
{
	const void* p = lua_topointer(L, lua_upvalueindex(1));
	const pafcore::InstanceMethod* instanceMethod = static_cast<const pafcore::InstanceMethod*>(p);
	pafcore::FunctionInvoker invoker = instanceMethod->invoker();
	uint32_t maxNumResults = instanceMethod->maxNumResults();
	uint32_t numArguments = (uint32_t)lua_gettop(L);
	return InvokeFunction(L, invoker, maxNumResults, numArguments, 1);
}

int StaticMethod_Closure(lua_State* L)
{
	const void* p = lua_topointer(L, lua_upvalueindex(1));
	const pafcore::StaticMethod* staticMethod = static_cast<const pafcore::StaticMethod*>(p);
	pafcore::FunctionInvoker invoker = staticMethod->invoker();
	uint32_t maxNumResults = staticMethod->maxNumResults();
	uint32_t numArguments = (uint32_t)lua_gettop(L);
	return InvokeFunction(L, invoker, maxNumResults, numArguments, 1);
}

pafcore::ErrorCode GetNestedType(lua_State *L, pafcore::Type* nestedType)
{
	pafcore::Variant value;
	value.assignRawPtr(nestedType);
	VariantToLua(L, &value);
	return pafcore::ErrorCode::s_ok;
}

int Construct_Call(lua_State* L, pafcore::Type* type)
{
	const int argStartIndex = 2;
	int numArguments = lua_gettop(L) + 1 - argStartIndex;
	PAF_ASSERT(numArguments >= 0);
	alignas(pafcore::Variant) char argumentsBuf[sizeof(pafcore::Variant) * max_param_count];
	pafcore::Variant* args[max_param_count];
	if (numArguments > max_param_count)
	{
		numArguments = max_param_count;
	}
	for (int i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant) * i];
		new(argument)pafcore::Variant;
		args[i] = LuaToVariant(argument, L, i + argStartIndex);
	}
	pafcore::Variant result;
	pafcore::ErrorCode errorCode = result.construct(type, args, numArguments);
	for (uint32_t i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant) * i];
		argument->~Variant();
	}
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &result);
		return 1;
	}
	else
	{
		Variant_Error(L, "Construct failed", errorCode);
		return 0;
	}
}

int NewSharedPtr_Closure(lua_State* L)
{
	const int argStartIndex = 1;
	int numArguments = lua_gettop(L) + 1 - argStartIndex;
	const pafcore::Type* type = (pafcore::Type*)lua_topointer(L, lua_upvalueindex(1));

	alignas(pafcore::Variant) char argumentsBuf[sizeof(pafcore::Variant) * max_param_count];
	pafcore::Variant* args[max_param_count];
	if (numArguments > max_param_count)
	{
		numArguments = max_param_count;
	}
	for (uint32_t i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant) * i];
		new(argument)pafcore::Variant;
		args[i] = LuaToVariant(argument, L, i + argStartIndex);
	}
	pafcore::Variant result;
	pafcore::ErrorCode errorCode = result.newSharedPtr(type, args, numArguments);
	for (uint32_t i = 0; i < numArguments; i++)
	{
		pafcore::Variant* argument = (pafcore::Variant*)&argumentsBuf[sizeof(pafcore::Variant) * i];
		argument->~Variant();
	}
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &result);
		return 1;
	}
	else
	{
		Variant_Error(L, "New failed", errorCode);
		return 0;
	}
}

int NewSharedArray_Closure(lua_State* L)
{
	const int argStartIndex = 1;
	int numArguments = lua_gettop(L) + 1 - argStartIndex;
	const pafcore::Type* type = (pafcore::Type*)lua_topointer(L, lua_upvalueindex(1));

	uint32_t count = 1;
	pafcore::Variant* args[1];
	pafcore::Variant argument;
	if (numArguments > 0)
	{
		args[0] = LuaToVariant(&argument, L, argStartIndex);
		args[0]->castToPrimitive(count);
	}
	pafcore::Variant result;
	pafcore::ErrorCode errorCode = result.newSharedArray(type, count);
	if (pafcore::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &result);
		return 1;
	}
	else
	{
		Variant_Error(L, "New failed", errorCode);
		return 0;
	}
}

int Subclassing(lua_State* L)
{
	const void* p = lua_topointer(L, lua_upvalueindex(1));
	pafcore::ClassType* classType = (pafcore::ClassType*)p;
	int numArguments = lua_gettop(L);
	if (1 == numArguments && lua_type(L, -1) == LUA_TTABLE)
	{
		ScriptInvoker* scriptInvoker = pafcore::New<ScriptInvoker>(L);
		pafcore::SharedPtr<pafcore::Interface> implementor = classType->createInterfaceProxy(scriptInvoker);
		pafcore::Variant impVar;
		impVar.assignSharedPtr(std::move(implementor));
		VariantToLua(L, &impVar);
		return 1;
	}
	Variant_Error(L, "the argument of _Derive_ must be a table", pafcore::ErrorCode::e_invalid_arg_type_1);
	return 0;
}

int Variant_Call(lua_State *L)
{
	pafcore::Variant* variant = (pafcore::Variant*)lua_touserdata(L, 1);
	switch(variant->type()->kind())
	{
	case pafcore::MetadataKind::instance_method:
		{
			pafcore::InstanceMethod* method = (pafcore::InstanceMethod*)variant->pointer();
			return InvokeFunction_Method(L, method->invoker(), method->maxNumResults());
		}
		break;
	case pafcore::MetadataKind::static_method:
		{
			pafcore::StaticMethod* method = (pafcore::StaticMethod*)variant->pointer();
			return InvokeFunction_Method(L, method->invoker(), method->maxNumResults());
		}
		break;
	case pafcore::MetadataKind::primitive_type:
		{
			pafcore::PrimitiveType* type = (pafcore::PrimitiveType*)variant->pointer();
			return Construct_Call(L, type);
			//assert(strcmp(type->staticMethods()[0].m_name, "New") == 0 && type->staticMethods()[0].maxNumResults() == 1);
			//return InvokeFunction_Method(L, type->staticMethods()[0].invoker(), 1);
		}
		break;
	case pafcore::MetadataKind::class_type:
		{
			pafcore::ClassType* type = (pafcore::ClassType*)variant->pointer();
			return Construct_Call(L, type);
			//pafcore::StaticMethod* method = type->findStaticMethod("New", false);
			//if(0 != method)
			//{
			//	return InvokeFunction_Method(L, method->invoker(), method->maxNumResults());
			//}
		}
		break;
	}
	Variant_Error(L, "call", pafcore::ErrorCode::e_is_not_type);
	return 0;
}

int Variant_Len(lua_State *L)
{
	pafcore::ErrorCode errorCode;
	pafcore::Variant* variant = (pafcore::Variant*)lua_touserdata(L, 1);
	lua_pushinteger(L, variant->arraySize());
	return 1;
}

//int GetEnumerator(lua_State *L, pafcore::EnumMember* enum_member)
//{
//	pafcore::Variant value;
//	value.assignEnum(enum_member->m_type, &enum_member->m_value);
//	VariantToLua(L, &value);
//	return 1;
//}

pafcore::ErrorCode Variant_Index_Identifier(lua_State *L, pafcore::Variant* variant, const char *name)
{
	switch(variant->type()->kind())
	{
	case pafcore::MetadataKind::name_space:
		{
			pafcore::NameSpace* ns = (pafcore::NameSpace*)variant->pointer();
			pafcore::Metadata* member = ns->findMember(name);
			if(0 != member)
			{
				pafcore::Variant value;
				value.assignRawPtr(member);
				VariantToLua(L, &value);
				return pafcore::ErrorCode::s_ok;
			}
		}
		break;
	case pafcore::MetadataKind::class_type:
		{
			pafcore::ClassType* type = (pafcore::ClassType*)variant->pointer();
			pafcore::Metadata* member = type->findClassMember(name, true, true);
			if(0 != member)
			{
				pafcore::Type* memberType = member->getType();
				switch(memberType->kind())
				{
				case pafcore::MetadataKind::static_field:
					if (static_cast<pafcore::StaticField*>(member)->isScalar())
					{
						return ScalarStaticFieldGetRef(L, static_cast<pafcore::StaticField*>(member));
					}
					else
					{
						return MakeArrayStaticField(L, static_cast<pafcore::StaticField*>(member));
					}
				case pafcore::MetadataKind::static_property:
					if (static_cast<pafcore::StaticProperty*>(member)->isScalar())
					{
						return ScalarStaticPropertyGet(L, static_cast<pafcore::StaticProperty*>(member));
					}
					else
					{
						return MakeCollectionStaticProperty(L, static_cast<pafcore::StaticProperty*>(member));
					}
				case pafcore::MetadataKind::static_method:
					lua_pushlightuserdata(L, member);
					lua_pushcclosure(L, StaticMethod_Closure, 1);
					return pafcore::ErrorCode::s_ok;
				case pafcore::MetadataKind::enum_type:
				case pafcore::MetadataKind::class_type:
					return GetNestedType(L, static_cast<pafcore::Type*>(member));
				}
			}
		}
		break;
	case pafcore::MetadataKind::primitive_type:
		{
			pafcore::PrimitiveType* type = (pafcore::PrimitiveType*)variant->pointer();
			pafcore::Metadata* member = type->findTypeMember(name);
			if(0 != member)
			{
				pafcore::Type* memberType = member->getType();
				switch(memberType->kind())
				{
				case pafcore::MetadataKind::static_method:
					lua_pushlightuserdata(L, member);
					lua_pushcclosure(L, StaticMethod_Closure, 1);
					return pafcore::ErrorCode::s_ok;
				default:
					assert(false);
				}
			}
		}
		break;
	case pafcore::MetadataKind::enum_type:
		{
			pafcore::EnumType* et = (pafcore::EnumType*)variant->pointer();
			pafcore::EnumMember* enum_member = et->findEnumerator(name);
			if(0 != enum_member)
			{
				pafcore::Variant value;
				value.assignEnum(enum_member->m_type, &enum_member->m_value);
				VariantToLua(L, &value);
				return pafcore::ErrorCode::s_ok;
			}
		}
		break;
	}
	pafcore::Metadata* member;
	member = variant->type()->findMember(name);
	if(0 != member)
	{
		pafcore::Type* memberType = member->getType();
		switch(memberType->kind())
		{
		case pafcore::MetadataKind::instance_field:
			if (static_cast<pafcore::InstanceField*>(member)->isScalar())
			{
				return ScalarInstanceFieldGetRef(L, variant, static_cast<pafcore::InstanceField*>(member));
			}
			else
			{
				return MakeArrayInstanceField(L, variant, static_cast<pafcore::InstanceField*>(member));
			}
		case pafcore::MetadataKind::static_field:
			if (static_cast<pafcore::StaticField*>(member)->isScalar())
			{
				return ScalarStaticFieldGetRef(L, static_cast<pafcore::StaticField*>(member));
			}
			else
			{
				return MakeArrayStaticField(L, static_cast<pafcore::StaticField*>(member));
			}
		case pafcore::MetadataKind::instance_property:
			if (static_cast<pafcore::InstanceProperty*>(member)->isScalar())
			{
				return ScalarInstancePropertyGet(L, variant, static_cast<pafcore::InstanceProperty*>(member));
			}
			else
			{
				return MakeCollectionInstanceProperty(L, variant, static_cast<pafcore::InstanceProperty*>(member));
			}
		case pafcore::MetadataKind::static_property:
			if (static_cast<pafcore::StaticProperty*>(member)->isScalar())
			{
				return ScalarStaticPropertyGet(L, static_cast<pafcore::StaticProperty*>(member));
			}
			else
			{
				return MakeCollectionStaticProperty(L, static_cast<pafcore::StaticProperty*>(member));
			}
		case pafcore::MetadataKind::instance_method:
			lua_pushlightuserdata(L, member);
			lua_pushcclosure(L, InstanceMethod_Closure, 1);
			return pafcore::ErrorCode::s_ok;
		case pafcore::MetadataKind::static_method:
			lua_pushlightuserdata(L, member);
			lua_pushcclosure(L, StaticMethod_Closure, 1);
			return pafcore::ErrorCode::s_ok;
		//case pafcore::enum_type:
		//case pafcore::class_type:
		//	return GetNestedType(L, static_cast<pafcore::Type*>(member));
		default:
			assert(false);
		}
	}
	else if (name[0] == 'N')
	{
		if (strcmp(&name[1], "ew") == 0)//New
		{
			pafcore::MetadataKind kind = variant->type()->kind();
			if (pafcore::MetadataKind::primitive_type == kind ||
				pafcore::MetadataKind::class_type == kind)
			{
				pafcore::Type* type = (pafcore::Type*)variant->pointer();
				lua_pushlightuserdata(L, type);
				lua_pushcclosure(L, NewSharedPtr_Closure, 1);
				return pafcore::ErrorCode::s_ok;
			}
		}
		else if (strcmp(&name[1], "ewArray") == 0)//NewArray
		{
			pafcore::MetadataKind kind = variant->type()->kind();
			if (pafcore::MetadataKind::primitive_type == kind ||
				pafcore::MetadataKind::class_type == kind)
			{
				pafcore::Type* type = (pafcore::Type*)variant->pointer();
				lua_pushlightuserdata(L, type);
				lua_pushcclosure(L, NewSharedArray_Closure, 1);
				return pafcore::ErrorCode::s_ok;
			}
		}
	}
	else if (name[0] == '_')
	{
		switch (name[1])
		{
		case '\0':			
			if (variant->type()->isPrimitive())//_
			{
				return GetPrimitive(L, variant);
			}
			else if (variant->type()->isEnum())
			{
				return GetEnum(L, variant);
			}
			break;
		case 'D':
			if (strcmp(&name[2], "erive_") == 0)//_Derive_
			{
				if (pafcore::MetadataKind::class_type == variant->type()->kind())
				{
					pafcore::ClassType* classType = (pafcore::ClassType*)variant->pointer();
					lua_pushlightuserdata(L, classType);
					lua_pushcclosure(L, Subclassing, 1);
					return pafcore::ErrorCode::s_ok;
				}
				else
				{
					return pafcore::ErrorCode::e_is_not_class;
				}
			}
			break;
		case 'a':
			if (strcmp(&name[2], "ddress_") == 0)//_address_
			{
				lua_pushinteger(L, (size_t)variant->pointer());
				return pafcore::ErrorCode::s_ok;
			}
			break;
		case 'c':
			if (strcmp(&name[2], "ount_") == 0)//_count_
			{
				lua_pushinteger(L, variant->arraySize());
				return pafcore::ErrorCode::s_ok;
			}
			break;
		case 'i':
			if (strcmp(&name[2], "sNullPtr_") == 0)//_isNullPtr_
			{
				lua_pushboolean(L, variant->isNull() ? 1 : 0);
				return pafcore::ErrorCode::s_ok;
			}
			break;
		case 's':
			if (strcmp(&name[2], "ize_") == 0)//_size_
			{
				lua_pushinteger(L, variant->type()->size());
				return pafcore::ErrorCode::s_ok;
			}
			break;
		case 't':
			if (strcmp(&name[2], "ype_") == 0)//_type_
			{
				pafcore::Variant typeVar;
				typeVar.assignRawPtr(variant->type());
				VariantToLua(L, &typeVar);
				return pafcore::ErrorCode::s_ok;
			}
			break;
		}
	}
	return pafcore::ErrorCode::e_member_not_found;
}

pafcore::ErrorCode Variant_NewIndex_Identifier(lua_State *L, pafcore::Variant* variant, const char *name)
{
	switch(variant->type()->kind())
	{
	case pafcore::MetadataKind::class_type:
		{
			pafcore::ClassType* type = (pafcore::ClassType*)variant->pointer();
			pafcore::Metadata* member = type->findClassMember(name, true, true);
			if(0 != member)
			{
				pafcore::Type* memberType = member->getType();
				switch(memberType->kind())
				{
				case pafcore::MetadataKind::static_field:
					return ScalarStaticFieldSet(L, static_cast<pafcore::StaticField*>(member));
				case pafcore::MetadataKind::static_property:
					return ScalarStaticPropertySet(L, static_cast<pafcore::StaticProperty*>(member));
				}
			}
		}
		break;
	}
	pafcore::Metadata* member;
	member = variant->type()->findMember(name);
	if(0 != member)
	{
		pafcore::Type* memberType = member->getType();
		switch(memberType->kind())
		{
		case pafcore::MetadataKind::instance_field:
			return ScalarInstanceFieldSet(L, variant, static_cast<pafcore::InstanceField*>(member));
		case pafcore::MetadataKind::static_field:
			return ScalarStaticFieldSet(L, static_cast<pafcore::StaticField*>(member));
		case pafcore::MetadataKind::instance_property:
			return ScalarInstancePropertySet(L, variant, static_cast<pafcore::InstanceProperty*>(member));
		case pafcore::MetadataKind::static_property:
			return ScalarStaticPropertySet(L, static_cast<pafcore::StaticProperty*>(member));
		}
	}
	else if (name[0] == '_')
	{
		switch (name[1])
		{
		case '\0':
			if (variant->type()->isPrimitive())
			{
				return SetPrimitive(L, variant);
			}
			else if (variant->type()->isEnum())
			{
				return SetEnum(L, variant);

			}
			break;
		}
	}
	return pafcore::ErrorCode::e_member_not_found;
}

pafcore::ErrorCode Variant_Index_Subscript(lua_State *L, pafcore::Variant* variant, size_t index)
{
	pafcore::Variant item;
	if(!variant->subscript(item, index))
	{
		return pafcore::ErrorCode::e_index_out_of_range;
	}
	VariantToLua(L, &item);
	return pafcore::ErrorCode::s_ok;
}

int Variant_Index(lua_State *L) 
{
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);

	pafcore::ErrorCode errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
	pafcore::Variant* variant = (pafcore::Variant*)lua_touserdata(L, 1);
	if(variant->isNull())
	{
		if (sc_string == sckind && strcmp(subscript.str, "_isNull_") == 0)//_isNull_
		{
			lua_pushboolean(L, variant->isNull()  ? 1 : 0);
			errorCode = pafcore::ErrorCode::s_ok;
		}
		else
		{
			errorCode = pafcore::ErrorCode::e_null_variant;
		}
	}
	else
	{
		if (sc_integer == sckind)
		{
			errorCode = Variant_Index_Subscript(L, variant, subscript.num);
		}
		else if (sc_string == sckind)
		{
			errorCode = Variant_Index_Identifier(L, variant, subscript.str);
		}
		else
		{
			errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
		}
	}
	if(pafcore::ErrorCode::s_ok != errorCode)
	{
		char buf[256];
		if (sc_integer == sckind)
		{
			sprintf_s(buf, "__index[%zd]", subscript.num);
			Variant_Error(L, buf, errorCode);
		}
		else if (sc_string == sckind)
		{
			sprintf_s(buf, "__index[\"%s\"]", subscript.str);
			Variant_Error(L, buf, errorCode);
		}
		else
		{
			Variant_Error(L, "__index", errorCode);
		}
		return 0;
	}
	return 1;
}

pafcore::ErrorCode Variant_NewIndex_Subscript(lua_State *L, pafcore::Variant* variant, size_t index)
{
	pafcore::Variant item;
	if(!variant->subscript(item, index))
	{
		return pafcore::ErrorCode::e_index_out_of_range;
	}
	pafcore::Variant maybeValue;
	pafcore::Variant* value = LuaToVariant(&maybeValue, L, 3);

	pafcore::Type* type = item.type();
	if(type->isPrimitive())
	{
		if (value->castToPrimitive(type, item.pointer()))
		{
			return pafcore::ErrorCode::s_ok;
		}
	}
	else if (type->isEnum())
	{
		if (value->castToEnum(type, item.pointer()))
		{
			return pafcore::ErrorCode::s_ok;
		}
	}
	else if (type->isClass())
	{
		if (value->castToClass(type, item.pointer()))
		{
			return pafcore::ErrorCode::s_ok;
		}
	}
	return pafcore::ErrorCode::e_invalid_type;
}

int Variant_NewIndex(lua_State *L) 
{
	pafcore::Variant* variant = (pafcore::Variant*)lua_touserdata(L, 1);
	if(variant->isNull())
	{
		Variant_Error(L, "__newindex", pafcore::ErrorCode::e_null_variant);
		return 0;
	}
	Subscript subscript;
	SubscriptKind sckind = Variant_ParseSubscript(subscript, L);

	pafcore::ErrorCode errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
	if(sc_integer == sckind)
	{
		errorCode = Variant_NewIndex_Subscript(L, variant, subscript.num);
	}
	else if(sc_string == sckind)
	{
		errorCode = Variant_NewIndex_Identifier(L, variant, subscript.str);
	}
	else
	{
		errorCode = pafcore::ErrorCode::e_invalid_subscript_type;
	}
	if(pafcore::ErrorCode::s_ok != errorCode)
	{
		char buf[256];
		if (sc_integer == sckind)
		{
			sprintf_s(buf, "__newindex[%zd]", subscript.num);
			Variant_Error(L, buf, errorCode);
		}
		else if (sc_string == sckind)
		{
			sprintf_s(buf, "__newindex[\"%s\"]", subscript.str);
			Variant_Error(L, buf, errorCode);
		}
		else
		{
			Variant_Error(L, "__newindex", errorCode);
		}
		return 0;
	}
	return 1;
}

struct luaL_Reg g_variantReg[] =
{
	{ "__gc", Variant_GC },
	{ "__index", Variant_Index },
	{ "__newindex", Variant_NewIndex },
	{ "__call", Variant_Call },
	{ "__len", Variant_Len },
	{NULL, NULL}
};

void RegisterVariantMetatable(lua_State* L)
{
	luaL_newmetatable(L, g_variantMetatableName);
	luaL_setfuncs(L, g_variantReg, 0);
}

void InitGlobalNameSpaceVariant(lua_State* L)
{
	void* p = lua_newuserdata(L, sizeof(pafcore::Variant));
	pafcore::Variant* variant = new(p)pafcore::Variant;
	variant->assignRawPtr(RuntimeTypeOf<pafcore::NameSpace>::RuntimeType::GetSingleton(), pafcore::NameSpace::GetGlobalNameSpace());
	luaL_getmetatable(L, g_variantMetatableName);
	lua_setmetatable(L, -2);
	lua_setglobal(L, "idlcpp");
}

END_PAFLUA

