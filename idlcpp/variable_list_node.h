#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;
struct VariableNode;

struct VariableListNode : SyntaxNodeImpl
{
	VariableListNode* m_variableList{ nullptr };
	TokenNode* m_comma{ nullptr };
	VariableNode* m_variable{ nullptr };
public:
	VariableListNode(VariableListNode* variableList, TokenNode* comma, VariableNode* variable);
	void collectVariableNodes(std::vector<std::pair<TokenNode*, VariableNode*>>& variableNodes) const;
	void collectVariableNodes(std::vector<VariableNode*>& variableNodes) const;
	VariableNode* getFirstVariable() const;
public:
	static VariableListNode* MergeVariableList(VariableNode* first, VariableListNode* second);
	static VariableListNode* MergeVariableList(VariableNode* first, VariableNode* second);
};

VariableListNode* MergeVariableList(VariableNode* first, VariableListNode* second);
VariableListNode* MergeVariableList(VariableNode* first, VariableNode* second);
