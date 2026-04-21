#include "../../paf/pafcore/class_type.h"
#include "../../paf/pafcore/enum_type.h"
#include "../../paf/pafcore/enumerator.h"
#include "../../paf/pafcore/metadata.h"
#include "../../paf/pafcore/name_space.h"
#include "../../paf/pafcore/primitive_type.h"
#include "../../paf/pafcore/reflection.h"
#include "../../paf/pafcore/std_string.h"
#include "../../paf/pafcore/type.h"
#include "../../paf/pafcore/type_alias.h"

#include <iostream>

namespace metatest
{
const char* CategoryToString(pafcore::Category category)
{
    switch (category)
    {
    case pafcore::void_object:
        return "void_object";
    case pafcore::primitive_object:
        return "primitive_object";
    case pafcore::enum_object:
        return "enum_object";
    case pafcore::value_object:
        return "value_object";
    case pafcore::rc_object:
        return "rc_object";
    case pafcore::atomic_rc_object:
        return "atomic_rc_object";
    case pafcore::enumerator:
        return "enumerator";
    case pafcore::instance_field:
        return "instance_field";
    case pafcore::static_field:
        return "static_field";
    case pafcore::instance_property:
        return "instance_property";
    case pafcore::static_property:
        return "static_property";
    case pafcore::instance_method:
        return "instance_method";
    case pafcore::static_method:
        return "static_method";
    case pafcore::function_argument:
        return "function_argument";
    case pafcore::function_result:
        return "function_result";
    case pafcore::void_type:
        return "void_type";
    case pafcore::primitive_type:
        return "primitive_type";
    case pafcore::enum_type:
        return "enum_type";
    case pafcore::class_type:
        return "class_type";
    case pafcore::type_alias:
        return "type_alias";
    case pafcore::name_space:
        return "name_space";
    default:
        return "unknown";
    }
}

pafcore::string Indent(int level)
{
    return pafcore::string(static_cast<size_t>(level) * 2U, ' ');
}

pafcore::string SafeName(const char* name)
{
    if (0 == name || 0 == *name)
    {
        return "<global>";
    }
    return name;
}

pafcore::string GetTypeName(const pafcore::Type* type)
{
    if (0 == type)
    {
        return "<null>";
    }

    pafcore::String fullName = pafcore::Reflection::GetTypeFullName(const_cast<pafcore::Type*>(type));
    if (fullName.empty())
    {
        return SafeName(type->m_name);
    }
    return fullName.c_str();
}

pafcore::string GetMetadataName(pafcore::Metadata* metadata)
{
    if (0 == metadata)
    {
        return "<null>";
    }

    if (metadata->isTypeOf<pafcore::Type>())
    {
        return GetTypeName(static_cast<pafcore::Type*>(metadata));
    }

    if (metadata->isTypeOf<pafcore::TypeAlias>())
    {
        pafcore::String fullName = pafcore::Reflection::GetTypeAliasFullName(static_cast<pafcore::TypeAlias*>(metadata));
        if (fullName.empty())
        {
            return SafeName(metadata->m_name);
        }
        return fullName.c_str();
    }

    return SafeName(metadata->m_name);
}

void DumpMetadata(std::ostream& os, pafcore::Metadata* metadata, int indentLevel);

void DumpNamespace(std::ostream& os, pafcore::NameSpace* nameSpace, int indentLevel)
{
    size_t memberCount = nameSpace->_getMemberCount_();
    for (size_t i = 0; i < memberCount; ++i)
    {
        DumpMetadata(os, nameSpace->_getMember_(i), indentLevel + 1);
    }
}

void DumpEnum(std::ostream& os, pafcore::EnumType* enumType, int indentLevel)
{
    size_t enumeratorCount = enumType->_getEnumeratorCount_();
    if (0 == enumeratorCount)
    {
        return;
    }

    os << Indent(indentLevel + 1) << "enumerators:" << std::endl;
    for (size_t i = 0; i < enumeratorCount; ++i)
    {
        pafcore::Enumerator* enumerator = enumType->_getEnumerator_(i);
        os << Indent(indentLevel + 2)
           << "- "
           << SafeName(enumerator->m_name)
           << " = "
           << enumerator->m_value
           << std::endl;
    }
}

void DumpClass(std::ostream& os, pafcore::ClassType* classType, int indentLevel)
{
    os << Indent(indentLevel + 1)
       << "bases: "
       << classType->m_baseClassCount
       << ", members: "
       << classType->_getMemberCount_(false)
       << std::endl;

    size_t memberCount = classType->_getMemberCount_(false);
    if (0 == memberCount)
    {
        return;
    }

    os << Indent(indentLevel + 1) << "class members:" << std::endl;
    for (size_t i = 0; i < memberCount; ++i)
    {
        DumpMetadata(os, classType->_getMember_(i, false), indentLevel + 2);
    }
}

void DumpPrimitive(std::ostream& os, pafcore::PrimitiveType* primitiveType, int indentLevel)
{
    os << Indent(indentLevel + 1)
       << "primitive_type_category: "
       << primitiveType->getPrimitiveTypeCategory()
       << std::endl;
}

void DumpTypeAlias(std::ostream& os, pafcore::TypeAlias* typeAlias, int indentLevel)
{
    os << Indent(indentLevel + 1)
       << "target: "
       << GetTypeName(typeAlias->m_type)
       << std::endl;
}

void DumpType(std::ostream& os, pafcore::Type* type, int indentLevel)
{
    os << Indent(indentLevel + 1)
       << "size: "
       << type->m_size
       << ", declaration: "
       << SafeName(type->m_declarationFile)
       << std::endl;
}

void DumpMetadata(std::ostream& os, pafcore::Metadata* metadata, int indentLevel)
{
    if (0 == metadata)
    {
        return;
    }

    os << Indent(indentLevel)
       << CategoryToString(metadata->_category_())
       << ": "
       << GetMetadataName(metadata)
       << std::endl;

    if (metadata->isTypeOf<pafcore::Type>())
    {
        DumpType(os, static_cast<pafcore::Type*>(metadata), indentLevel);
    }

    if (metadata->isTypeOf<pafcore::PrimitiveType>())
    {
        DumpPrimitive(os, static_cast<pafcore::PrimitiveType*>(metadata), indentLevel);
    }

    if (metadata->isTypeOf<pafcore::TypeAlias>())
    {
        DumpTypeAlias(os, static_cast<pafcore::TypeAlias*>(metadata), indentLevel);
    }

    if (metadata->isTypeOf<pafcore::EnumType>())
    {
        DumpEnum(os, static_cast<pafcore::EnumType*>(metadata), indentLevel);
    }

    if (metadata->isTypeOf<pafcore::ClassType>())
    {
        DumpClass(os, static_cast<pafcore::ClassType*>(metadata), indentLevel);
    }

    if (metadata->isTypeOf<pafcore::NameSpace>())
    {
        DumpNamespace(os, static_cast<pafcore::NameSpace*>(metadata), indentLevel);
    }
}
}

int main()
{
    pafcore::NameSpace* globalNameSpace = pafcore::NameSpace::GetGlobalNameSpace();
    std::cout << "registered metadata tree" << std::endl;
    metatest::DumpMetadata(std::cout, globalNameSpace, 0);
    return 0;
}
