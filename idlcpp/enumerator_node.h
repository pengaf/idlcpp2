#pragma once

#include "member_node.h"

struct TokenNode;
struct TypeNameNode;

struct EnumeratorNode : SyntaxNodeImpl
{
	AttributeListNode* m_attributeList;
	IdentifyNode* m_name;
public:
	EnumeratorNode(AttributeListNode* attributeList, IdentifyNode* name);
	void checkSemantic();
};

struct CompareEnumeratorPtr
{
	bool operator()(const EnumeratorNode* arg1, const EnumeratorNode* arg2) const;
};
