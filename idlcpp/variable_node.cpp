#include "variable_node.h"
#include "token_node.h"
#include "compound_type_node.h"
#include "type_name_node.h"
#include "identifier_node.h"
#include "type_tree.h"
#include "compiler.h"


VariableNode::VariableNode(CompoundTypeNode* compoundType, TokenNode* byRef, IdentifierNode* name, bool allowNull)
{
	m_nodeType = snt_variable;
	m_compoundType = compoundType;
	m_byRef = byRef;
	m_name = name;
	m_allowNull = allowNull;
}

bool VariableNode::isByRef() const
{
	return nullptr != m_byRef;
}

bool VariableNode::isAllowNull() const
{
	return m_allowNull;
}

void VariableNode::checkSemantic(TemplateArguments* templateArguments)
{
	TypeNode* typeNode = m_compoundType->m_typeName->getTypeNode(templateArguments);
	if (0 == typeNode)
	{
		return;
	}
	bool byValue = !isByRef() && m_compoundType->isNotPtr();
	g_compiler.useType(typeNode, templateArguments, byValue ? tu_use_definition : tu_use_declaration, m_compoundType->m_typeName);
}
