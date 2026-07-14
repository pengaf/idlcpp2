#include "attribute_node.h"
#include "identifier_node.h"

AttributeNode::AttributeNode(IdentifierNode* name, IdentifierNode* content, bool u8content)
{
	m_nodeType = snt_attribute;
	m_name = name;
	m_content = content;
	m_u8content = u8content;
}

bool CompareAttributePtr::operator()(const AttributeNode* arg1, const AttributeNode* arg2) const
{
	return arg1->m_name->m_str < arg2->m_name->m_str;
}

