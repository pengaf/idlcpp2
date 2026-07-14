#include "property_accessor_node.h"
#include "token_node.h"

PropertyAccessorNode::PropertyAccessorNode(TokenNode* keyword, TokenNode* byRef, IdentifierNode* nativeName)
{
	m_nodeType = snt_property_accessor;
	m_keyword = keyword;
	m_byRef = byRef;
	m_nativeName = nativeName;
}

bool PropertyAccessorNode::isByRef() const
{
	return nullptr != m_byRef;
}
