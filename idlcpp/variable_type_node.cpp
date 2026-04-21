#include "variable_type_node.h"
#include "token_node.h"
#include "type_name_node.h"

VariableTypeNode::VariableTypeNode(TypeNameNode* typeName, TokenNode* constant, TokenNode* typeCompound)
{
	m_nodeType = snt_variable_type;
	m_typeName = typeName;
	m_constant = constant;
	m_typeCompound = typeCompound;
	m_byRef = 0;
}

bool VariableTypeNode::isByValue() const
{
	return 0 == m_typeCompound && 0 == m_byRef;
}

bool VariableTypeNode::isByRef() const
{
	return 0 != m_byRef;
}

bool VariableTypeNode::isByObserverPtr() const
{
	return 0 != m_typeCompound && '*' == m_typeCompound->m_nodeType;
}

bool VariableTypeNode::isByUniquePtr() const
{
	return 0 != m_typeCompound && '!' == m_typeCompound->m_nodeType;
}

bool VariableTypeNode::isBySharedPtr() const
{
	return 0 != m_typeCompound && '^' == m_typeCompound->m_nodeType;
}
