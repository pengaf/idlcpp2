#include "property_node.h"
#include "compound_type_node.h"
#include "token_node.h"
#include "identifier_node.h"
#include "property_accessor_node.h"
#include "property_accessor_list_node.h"
#include "class_node.h"
#include "type_tree.h"
#include "raise_error.h"
#include "compiler.h"
#include <assert.h>


PropertyNode::PropertyNode(CompoundTypeNode* compoundType, IdentifierNode* name, PropertyAccessorListNode* accessorList, PropertyKind propertyKind)
{
	m_nodeType = snt_property;
	m_compoundType = compoundType;
	m_name = name;
	m_accessorList = accessorList;
	m_propertyKind = propertyKind;
}

PropertyKind PropertyNode::getKind()
{
	return m_propertyKind;
}

bool PropertyNode::isStatic()
{
	return (0 != m_modifier && snt_keyword_static == m_modifier->m_nodeType);
}

bool PropertyNode::isSimple()
{
	return simple_property == m_propertyKind;
}

bool PropertyNode::isFixedArray()
{
	return fixed_array_property == m_propertyKind;
}

bool PropertyNode::isDynamicArray()
{
	return dynamic_array_property == m_propertyKind;
}

bool PropertyNode::isList()
{
	return list_property == m_propertyKind;
}

void PropertyNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	m_compoundType->m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
}

void PropertyNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);
	std::vector<PropertyAccessorNode*> propertyAccessors;
	m_accessorList->collectPropertyAccessors(propertyAccessors);
	for (PropertyAccessorNode* accessor : propertyAccessors)
	{
		switch (accessor->m_keyword->m_nodeType)
		{
		case snt_keyword_get:
			if (m_get)
			{
				RaiseError_PropertyAccessorAlreadyExist(accessor, m_get);
			}
			else
			{
				m_get = accessor;
			}
			break;
		case snt_keyword_set:
			if (m_set)
			{
				RaiseError_PropertyAccessorAlreadyExist(accessor, m_set);
			}
			else
			{
				m_set = accessor;
			}
			break;
		case snt_keyword_enum:
			if (m_enumerate)
			{
				RaiseError_PropertyAccessorAlreadyExist(accessor, m_enumerate);
			}
			else
			{
				m_enumerate = accessor;
			}
			break;
		}
	}
	if (m_enumerate)
	{
		if (!m_compoundType->isNotPtr())
		{
			RaiseError_PropertyEnumNotAllowPtr(m_enumerate);
		}
	}
	TypeNode* typeNode = m_compoundType->m_typeName->getTypeNode(templateArguments);
	if (typeNode)
	{
		g_compiler.useType(typeNode, templateArguments, m_compoundType->isNotPtr() ? tu_use_definition : tu_use_declaration, m_compoundType->m_typeName);
	}
}
