#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;
struct EnumeratorNode;

struct EnumeratorListNode : SyntaxNodeImpl
{
	EnumeratorListNode* m_enumeratorList{ nullptr };
	TokenNode* m_comma{ nullptr };
	EnumeratorNode* m_enumerator{ nullptr };
public:
	EnumeratorListNode(EnumeratorListNode* enumeratorList, TokenNode* comma, EnumeratorNode* enumerator);
	void collectEnumeratorNodes(std::vector<std::pair<TokenNode*, EnumeratorNode*>>& enumeratorNodes);
	void collectEnumeratorNodes(std::vector<EnumeratorNode*>& enumeratorNodes);
};

