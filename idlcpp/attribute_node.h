#pragma once

#include "syntax_node_impl.h"

struct IdentifierNode;

struct AttributeNode : SyntaxNodeImpl
{
	IdentifierNode* m_name;
	IdentifierNode* m_content;
	bool m_u8content;
public:
	AttributeNode(IdentifierNode* name, IdentifierNode* content, bool u8content);
};

struct CompareAttributePtr
{
	bool operator()(const AttributeNode* arg1, const AttributeNode* arg2) const;
};

