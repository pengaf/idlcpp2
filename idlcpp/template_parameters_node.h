#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;
struct IdentifierNode;
struct IdentifierListNode;

struct TemplateParametersNode : SyntaxNodeImpl
{
	TokenNode* m_keyword;
	TokenNode* m_leftBracket;
	IdentifierListNode* m_parameterList;
	TokenNode* m_rightBracket;
public:
	TemplateParametersNode(TokenNode* keyword, TokenNode* leftBracket, IdentifierListNode* parameterList, TokenNode* rightBracket);
	void collectParameterNodes(std::vector<std::pair<TokenNode*, IdentifierNode*>>& parameterNodes);
	void collectParameterNodes(std::vector<IdentifierNode*>& parameterNodes);
	size_t getParameterCount();
	bool checkSemantic();
};
