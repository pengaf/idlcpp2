#include "property_node.h"
#include "compound_type_node.h"
#include "token_node.h"
#include "identifier_node.h"
#include "property_accessor_node.h"
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

	assert(snt_class == m_enclosing->m_nodeType);
	ClassNode* classNode = static_cast<ClassNode*>(m_enclosing);

	if ((0 != m_typeCompound
		&& '*' != m_typeCompound->m_nodeType
		&& '!' != m_typeCompound->m_nodeType
		&& '^' != m_typeCompound->m_nodeType)
		|| (0 != m_byRef && 0 != m_typeCompound))
	{
		RaiseError_InvalidPropertyType(this);
		return;
	}

	if ((0 != m_keyTypeCompound
		&& '*' != m_keyTypeCompound->m_nodeType
		&& '!' != m_keyTypeCompound->m_nodeType
		&& '^' != m_keyTypeCompound->m_nodeType)
		|| (0 != m_keyByRef && 0 != m_keyTypeCompound))
	{
		RaiseError_InvalidPropertyType(this);
		return;
	}

	TypeNode* typeNode = m_typeName->getTypeNode(templateArguments);
	if (0 == typeNode)
	{
		return;
	}
	if (void_type == typeNode->getTypeKind(templateArguments) && !isByPtr())
	{
		RaiseError_InvalidPropertyType(this);
	}
	if (isByUniquePtr() && rc_object_type == typeNode->getTypeKind(templateArguments))
	{
		RaiseError_InvalidPropertyType(this);
	}
	g_compiler.useType(typeNode, templateArguments, isByValue() ? tu_use_definition : tu_use_declaration, m_typeName);
}
