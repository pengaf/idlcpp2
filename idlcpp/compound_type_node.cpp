#include "compound_type_node.h"
#include "type_name_node.h"

CompoundTypeNode::CompoundTypeNode(TypeNameNode* typeName, TypeCompound typeCompound)
{
	m_nodeType = snt_compound_type;
	m_typeName = typeName;
	m_typeCompound = typeCompound;
}

bool CompoundTypeNode::isObserverPtr() const
{
	return tc_observer_ptr == m_typeCompound;
}

bool CompoundTypeNode::isSharedPtr() const
{
	return tc_shared_ptr == m_typeCompound;
}

bool CompoundTypeNode::isObserverArray() const
{
	return tc_observer_array == m_typeCompound;
}

bool CompoundTypeNode::isSharedArray() const
{
	return tc_shared_array == m_typeCompound;
}

bool CompoundTypeNode::isSmartPtr() const
{
	return tc_none != m_typeCompound;
}
