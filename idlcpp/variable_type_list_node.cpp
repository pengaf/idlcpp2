#include "variable_type_list_node.h"
#include "variable_type_node.h"
#include <algorithm>

VariableTypeListNode::VariableTypeListNode(VariableTypeListNode* variableTypeList, TokenNode* delimiter, VariableTypeNode* variableType)
{
	m_nodeType = snt_variable_type_list;
	m_variableTypeList = variableTypeList;
	m_delimiter = delimiter;
	m_variableType = variableType;
}

void VariableTypeListNode::collectVariableTypeNodes(std::vector<std::pair<TokenNode*, VariableTypeNode*>>& variableTypeNodes)
{
	VariableTypeListNode* list = this;
	while (0 != list)
	{
		variableTypeNodes.push_back(std::make_pair(list->m_delimiter, list->m_variableType));
		list = list->m_variableTypeList;
	}
	std::reverse(variableTypeNodes.begin(), variableTypeNodes.end());
}

void VariableTypeListNode::collectVariableTypeNodes(std::vector<VariableTypeNode*>& variableTypeNodes)
{
	VariableTypeListNode* list = this;
	while (0 != list)
	{
		variableTypeNodes.push_back(list->m_variableType);
		list = list->m_variableTypeList;
	}
	std::reverse(variableTypeNodes.begin(), variableTypeNodes.end());
}

VariableTypeNode* VariableTypeListNode::getFirstVariableType()
{
	VariableTypeListNode* list = this;
	while (0 != list && 0 != list->m_variableTypeList)
	{
		list = list->m_variableTypeList;
	}
	return 0 == list ? 0 : list->m_variableType;
}
