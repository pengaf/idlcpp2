#include "field_node.h"
#include "variable_node.h"
#include "compound_type_node.h"
#include "class_node.h"
#include "type_name_node.h"
#include "type_tree.h"
#include "raise_error.h"
#include "compiler.h"
#include "source_file.h"
#include <assert.h>

FieldNode::FieldNode(CompoundTypeNode* compoundType, IdentifierNode* name, TokenNode* leftBracket, TokenNode* rightBracket)
{
	m_nodeType = snt_field;
	m_compoundType = compoundType;
	m_name = name;
	m_leftBracket = leftBracket;
	m_rightBracket = rightBracket;
}

bool FieldNode::isStatic() const
{
	return (nullptr != m_modifier);
}

bool FieldNode::isArray() const
{
	return (nullptr != m_leftBracket);
}

void FieldNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	m_compoundType->m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
}

void FieldNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);
	TypeNameNode* typeName = m_compoundType->m_typeName;

	if (m_compoundType->isSmartPtr())
	{
		g_compiler.m_currentSourceFile->m_useMemoryHeader = true;
	}
	//if (0 != m_typeCompound && '&' == m_typeCompound->m_nodeType)
	//{
	//	RaiseError_InvalidFieldType(this);
	//	return;
	//}
	TypeNode* typeNode = typeName->getTypeNode(templateArguments);
	if (0 == typeNode)
	{
		return;
	}
	if (void_type == typeNode->getTypeKind(templateArguments))
	{
		RaiseError_InvalidFieldType(this);
	}
	//if ((isUniquePtr() || isUniqueArray()) && rc_object_type == typeNode->getTypeKind(templateArguments))
	//{
	//	RaiseError_InvalidFieldType(this);
	//}
	g_compiler.useType(typeNode, templateArguments, m_compoundType->isSmartPtr() ? tu_use_declaration : tu_use_definition, typeName);
}

