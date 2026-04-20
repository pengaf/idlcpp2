#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;

struct TokenListNode : SyntaxNodeImpl
{
	TokenListNode* m_tokenList;
	TokenNode* m_token;
public:
	TokenListNode(TokenListNode* tokenList, TokenNode* token);
	void collectTokenNodes(std::vector<TokenNode*>& tokenNodes);
};

