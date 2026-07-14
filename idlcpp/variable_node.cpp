#include "variable_node.h"
#include "token_node.h"
#include "compound_type_node.h"
#include "identifier_node.h"


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
