#include "enumerator_list_node.h"
#include <algorithm>

EnumeratorListNode::EnumeratorListNode(EnumeratorListNode* enumeratorList, TokenNode* comma, EnumeratorNode* enum_member)
{
	m_nodeType = snt_enumerator_list;
	m_enumeratorList = enumeratorList;
	m_comma = comma;
	m_enumerator = enum_member;
}

void EnumeratorListNode::collectEnumeratorNodes(std::vector<std::pair<TokenNode*, EnumeratorNode*>>& enumeratorNodes)
{
	EnumeratorListNode* list = this;
	while(0 != list)
	{
		enumeratorNodes.push_back(std::make_pair(list->m_comma, list->m_enumerator));
		list = list->m_enumeratorList;
	}
	std::reverse(enumeratorNodes.begin(), enumeratorNodes.end());
}

void EnumeratorListNode::collectEnumeratorNodes(std::vector<EnumeratorNode*>& enumeratorNodes)
{
	EnumeratorListNode* list = this;
	while (0 != list)
	{
		enumeratorNodes.push_back(list->m_enumerator);
		list = list->m_enumeratorList;
	}
	std::reverse(enumeratorNodes.begin(), enumeratorNodes.end());
}
