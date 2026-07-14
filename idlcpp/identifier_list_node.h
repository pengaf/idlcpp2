#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;
struct IdentifierNode;

struct IdentifierListNode : SyntaxNodeImpl
{
	IdentifierListNode* m_identifierList;
	TokenNode* m_comma;
	IdentifierNode* m_identifier;
public:
	IdentifierListNode(IdentifierListNode* identifierList, TokenNode* comma, IdentifierNode* identifier);
	void collectIdentifierNodes(std::vector<std::pair<TokenNode*, IdentifierNode*>>& identifierNodes);
	void collectIdentifierNodes(std::vector<IdentifierNode*>& identifierNodes);
};

