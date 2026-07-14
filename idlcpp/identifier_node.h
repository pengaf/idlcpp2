#pragma once

#include "token_node.h"
#include <string>

struct IdentifierNode : TokenNode
{
	std::string m_str;
public:
	IdentifierNode(const char* str, int tokenNo, int lineNo, int columnNo);
	IdentifierNode(const char* str, size_t len, int tokenNo, int lineNo, int columnNo);
};

struct CompareIdentifierPtr
{
	bool operator()(const IdentifierNode* arg1, const IdentifierNode* arg2) const
	{
		return arg1->m_str < arg2->m_str;
	}
};

