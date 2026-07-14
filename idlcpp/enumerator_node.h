#pragma once
#include "entity_node.h"

struct TokenNode;

struct EnumeratorNode : EntityNode
{
	TokenNode* m_equalSign{ nullptr };
public:
	EnumeratorNode(IdentifierNode* name, TokenNode* equalSign);
	void checkSemantic();
};

struct CompareEnumeratorPtr
{
	bool operator()(const EnumeratorNode* arg1, const EnumeratorNode* arg2) const;
};
