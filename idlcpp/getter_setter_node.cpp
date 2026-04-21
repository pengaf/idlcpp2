#include "getter_setter_node.h"
#include "token_node.h"

GetterSetterNode::GetterSetterNode(TokenNode* keyword)
{
	m_nodeType = snt_getter_setter;
	m_keyword = keyword;
	m_nativeName = 0;
	m_setterAllowNull = false;
}

bool GetterSetterNode::isSetterAllowNull()
{
	return m_setterAllowNull;
}
