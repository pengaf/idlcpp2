#pragma once

#include "syntax_node_impl.h"

struct VariableTypeListNode;
struct IdentifyNode;

struct TypedMemberPrefixNode : SyntaxNodeImpl
{
	VariableTypeListNode* m_variableTypeList;
	IdentifyNode* m_name;
public:
	TypedMemberPrefixNode(VariableTypeListNode* variableTypeList, IdentifyNode* name);
};
