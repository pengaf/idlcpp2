#include "template_parameters_node.h"
#include "identifier_list_node.h"
#include "identifier_node.h"
#include "raise_error.h"
#include <set>
#include <algorithm>

TemplateParametersNode::TemplateParametersNode(TokenNode* keyword, TokenNode* leftBracket, IdentifierListNode* parameterList, TokenNode* rightBracket)
{
	m_nodeType = snt_template_parameters;
	m_keyword = keyword;
	m_leftBracket = leftBracket;
	m_parameterList = parameterList;
	m_rightBracket = rightBracket;
}

void TemplateParametersNode::collectParameterNodes(std::vector<std::pair<TokenNode*, IdentifierNode*>>& parameterNodes)
{
	IdentifierListNode* list = m_parameterList;
	while(0 != list)
	{
		parameterNodes.push_back(std::make_pair(list->m_comma, list->m_identifier));
		list = list->m_identifierList;
	}
	std::reverse(parameterNodes.begin(), parameterNodes.end());
}

void TemplateParametersNode::collectParameterNodes(std::vector<IdentifierNode*>& parameterNodes)
{
	IdentifierListNode* list = m_parameterList;
	while (0 != list)
	{
		parameterNodes.push_back(list->m_identifier);
		list = list->m_identifierList;
	}
	std::reverse(parameterNodes.begin(), parameterNodes.end());
}

size_t TemplateParametersNode::getParameterCount()
{
	size_t count = 0;
	IdentifierListNode* list = m_parameterList;
	while (0 != list)
	{
		++count;
		list = list->m_identifierList;
	}
	return count;
}

bool TemplateParametersNode::checkSemantic()
{
	bool res = true;
	std::set<std::string> paramNames;
	std::vector<IdentifierNode*> parameterNodes;
	collectParameterNodes(parameterNodes);
	size_t count = parameterNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		IdentifierNode* paramNode = parameterNodes[i];
		if(!paramNames.insert(paramNode->m_str).second)
		{
			RaiseError_TemplateParameterRedefinition(paramNode);
			res = false;
		}
	}
	return res;
}
