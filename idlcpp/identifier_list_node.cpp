#include "identifier_list_node.h"
#include <algorithm>

IdentifierListNode::IdentifierListNode(IdentifierListNode* identifierList, TokenNode* comma, IdentifierNode* identifier)
{
	m_nodeType = snt_identifier_list;
	m_identifierList = identifierList;
	m_comma = comma;
	m_identifier = identifier;
}

void IdentifierListNode::collectIdentifierNodes(std::vector<std::pair<TokenNode*, IdentifierNode*>>& identifierNodes)
{
	IdentifierListNode* list = this;
	while(0 != list)
	{
		identifierNodes.push_back(std::make_pair(list->m_comma, list->m_identifier));
		list = list->m_identifierList;
	}
	std::reverse(identifierNodes.begin(), identifierNodes.end());
}

void IdentifierListNode::collectIdentifierNodes(std::vector<IdentifierNode*>& identifierNodes)
{
	IdentifierListNode* list = this;
	while (0 != list)
	{
		identifierNodes.push_back(list->m_identifier);
		list = list->m_identifierList;
	}
	std::reverse(identifierNodes.begin(), identifierNodes.end());
}
