#include "variable_list_node.h"
#include "variable_node.h"
#include <algorithm>

VariableListNode::VariableListNode(VariableListNode* variableList, TokenNode* comma, VariableNode* variable)
{
	m_nodeType = snt_variable_list;
	m_variableList = variableList;
	m_comma = comma;
	m_variable = variable;
}

void VariableListNode::collectVariableNodes(std::vector<std::pair<TokenNode*, VariableNode*>>& variableNodes) const
{
	VariableListNode const* list = this;
	while (0 != list)
	{
		variableNodes.push_back(std::make_pair(list->m_comma, list->m_variable));
		list = list->m_variableList;
	}
	std::reverse(variableNodes.begin(), variableNodes.end());
}

void VariableListNode::collectVariableNodes(std::vector<VariableNode*>& variableNodes) const
{
	VariableListNode const* list = this;
	while (0 != list)
	{
		variableNodes.push_back(list->m_variable);
		list = list->m_variableList;
	}
	std::reverse(variableNodes.begin(), variableNodes.end());
}

VariableNode* VariableListNode::getFirstVariable() const
{
	VariableListNode const* list = this;
	while (0 != list && 0 != list->m_variableList)
	{
		list = list->m_variableList;
	}
	return 0 == list ? 0 : list->m_variable;
}
