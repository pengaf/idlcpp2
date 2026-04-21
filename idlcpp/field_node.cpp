#include "field_node.h"
#include "token_node.h"
#include "identify_node.h"
#include "class_node.h"
#include "type_name_node.h"
#include "type_tree.h"
#include "raise_error.h"
#include "compiler.h"
#include "source_file.h"
#include <assert.h>

FieldNode::FieldNode(TypeNameNode* typeName, TokenNode* typeCompound, IdentifyNode* name, TokenNode* leftBracket, TokenNode* rightBracket)
{
	m_nodeType = snt_field;
	m_static = 0;
	m_constant = 0;
	m_typeName = typeName;
	m_typeCompound = typeCompound;
	m_name = name;
	m_leftBracket = leftBracket;
	m_rightBracket = rightBracket;
	m_semicolon = 0;
	m_smartArray = false;
}

bool FieldNode::isStatic()
{
	return (0 != m_static);
}

bool FieldNode::isConstant()
{
	return 0 != m_constant;
}

bool FieldNode::isPointer()
{
	return 0 != m_typeCompound && 0 == m_leftBracket;
}

bool FieldNode::isObserverPtr()
{
	return isPointer() && '*' == m_typeCompound->m_nodeType;
}

bool FieldNode::isUniquePtr()
{
	return isPointer() && '!' == m_typeCompound->m_nodeType;
}

bool FieldNode::isSharedPtr()
{
	return isPointer() && '^' == m_typeCompound->m_nodeType;
}

bool FieldNode::isArray()
{
	return (0 != m_leftBracket);
}

bool FieldNode::isSmartArray()
{
	return m_smartArray;
}

bool FieldNode::isObserverArray()
{
	return isSmartArray() && (0 == m_typeCompound || '*' == m_typeCompound->m_nodeType);
}

bool FieldNode::isUniqueArray()
{
	return isSmartArray() && 0 != m_typeCompound && '!' == m_typeCompound->m_nodeType;
}

bool FieldNode::isSharedArray()
{
	return isSmartArray() && 0 != m_typeCompound && '^' == m_typeCompound->m_nodeType;
}

void FieldNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
}

void FieldNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);
	if (0 != m_typeCompound || m_smartArray)
	{
		g_compiler.m_currentSourceFile->m_useMemoryHeader = true;
	}
	if (0 != m_typeCompound && '&' == m_typeCompound->m_nodeType)
	{
		RaiseError_InvalidFieldType(this);
		return;
	}
	TypeNode* typeNode = m_typeName->getTypeNode(templateArguments);
	if (0 == typeNode)
	{
		return;
	}
	if (void_type == typeNode->getTypeCategory(templateArguments))
	{
		RaiseError_InvalidFieldType(this);
	}
	if ((isUniquePtr() || isUniqueArray()) && rc_object_type == typeNode->getTypeCategory(templateArguments))
	{
		RaiseError_InvalidFieldType(this);
	}
	g_compiler.useType(typeNode, templateArguments, isPointer() || isSmartArray() ? tu_use_declaration : tu_use_definition, m_typeName);
}

