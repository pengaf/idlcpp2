#pragma once

#include "syntax_node_impl.h"

struct IdentifierNode;
struct AttributeListNode;

struct EntityNode : SyntaxNodeImpl
{
	IdentifierNode* m_name{ nullptr };
	AttributeListNode* m_attributeList{ nullptr };
};

