#pragma once

#include "syntax_node_impl.h"

struct TokenNode;
struct IdentifierNode;

struct PropertyAccessorNode : SyntaxNodeImpl
{
	TokenNode* m_keyword { nullptr };
	TokenNode* m_byRef{ nullptr };
	IdentifierNode* m_nativeName{ nullptr };
public:
	PropertyAccessorNode(TokenNode* keyword, TokenNode* byRef, IdentifierNode* nativeName);
	bool isByRef() const;
};
