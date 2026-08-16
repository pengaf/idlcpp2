#include "method_node.h"
#include "variable_list_node.h"
#include "variable_node.h"
#include "compound_type_node.h"
#include "type_name_node.h"
#include "identifier_node.h"
#include "class_node.h"
#include "error_list.h"
#include "raise_error.h"
#include "type_tree.h"
#include "compiler.h"

#include <set>
#include <assert.h>

const char g_strPure[] = {" = 0 "};

MethodNode::MethodNode(VariableListNode* resultList, IdentifierNode* name, TokenNode* leftParenthesis, VariableListNode* parameterList, TokenNode* rightParenthesis)
{
	m_nodeType = snt_method;
	m_resultList = resultList;
	m_name = name;
	m_leftParenthesis = leftParenthesis;
	m_parameterList = parameterList;
	m_rightParenthesis = rightParenthesis;
}

bool MethodNode::isStatic() const
{
	return (0 != m_modifier && snt_keyword_static == m_modifier->m_nodeType);
}

bool MethodNode::isVirtual() const
{
	return (0 != m_modifier && snt_keyword_virtual == m_modifier->m_nodeType);
}

bool MethodNode::isConstant() const
{
	return (0 != m_constant && snt_keyword_const == m_constant->m_nodeType);
}

uint32_t MethodNode::getResultCount() const
{
	if (uint32_t(-1) == m_resultCount)
	{
		uint32_t res = 0;
		VariableListNode* list = m_resultList;
		while (0 != list)
		{
			++res;
			list = list->m_variableList;
		}
		m_resultCount = res;
	}
	return m_resultCount;
}

uint32_t MethodNode::getParameterCount() const
{
	if (uint32_t(-1) == m_parameterCount)
	{
		uint32_t res = 0;
		VariableListNode* list = m_parameterList;
		while (0 != list)
		{
			++res;
			list = list->m_variableList;
		}
		m_parameterCount = res;
	}
	return m_parameterCount;
}

void MethodNode::calcManglingName(std::string& name, TemplateArguments* templateArguments)
{
	if (isStatic())
	{
		name = "";
	}
	else
	{
		name = ",";
	}

	std::vector<VariableNode*> resultNodes;
	m_resultList->collectVariableNodes(resultNodes);
	for (VariableNode* resultNode : resultNodes)
	{
		TypeNode* typeNode = resultNode->m_compoundType->m_typeName->getTypeNode(templateArguments);
		if (typeNode)
		{
			name += typeNode->m_name + ",";
		}
	}

	std::vector<VariableNode*> parameterNodes;
	m_parameterList->collectVariableNodes(parameterNodes);
	for (VariableNode* parameterNode : parameterNodes)
	{
		TypeNode* typeNode = parameterNode->m_compoundType->m_typeName->getTypeNode(templateArguments);
		if (typeNode)
		{
			name += typeNode->m_name + ",";
		}
	}
}

void MethodNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	std::vector<VariableNode*> resultNodes;
	m_resultList->collectVariableNodes(resultNodes);
	for (VariableNode* resultNode : resultNodes)
	{
		resultNode->m_compoundType->m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
	}

	std::vector<VariableNode*> parameterNodes;
	m_parameterList->collectVariableNodes(parameterNodes);
	for (VariableNode* parameterNode: parameterNodes)
	{
		parameterNode->m_compoundType->m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
	}
}

void CheckFullParameterNames(const std::vector<VariableNode*>& resultNodes, const std::vector<VariableNode*>& parameterNodes)
{
	std::vector<VariableNode*> allParams;
	if (resultNodes.size() > 1)
	{
		allParams.insert(allParams.end(), resultNodes.begin() + 1, resultNodes.end());
	}
	allParams.insert(allParams.end(), parameterNodes.begin(), parameterNodes.end());

	std::set<IdentifierNode*, CompareIdentifierPtr> items;
	for (VariableNode* param : allParams)
	{
		IdentifierNode* identifier = param->m_name;
		auto res = items.insert(identifier);
		if (!res.second)
		{
			char buf[4096];
			sprintf_s(buf, "\'%s\' : parameter already defined at line %d, column %d", identifier->m_str.c_str(),
				(*res.first)->m_lineNo, (*res.first)->m_columnNo);
			ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
				identifier->m_columnNo, semantic_error_member_redefined, buf);
		}
	}
}

void MethodNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);

	if (isStatic())
	{
		if (isVirtual())
		{
			ErrorList_AddItem_CurrentFile(m_constant->m_lineNo, m_constant->m_columnNo, semantic_error_method, "static virtual method is not allowed");
		}
		if (isConstant())
		{
			ErrorList_AddItem_CurrentFile(m_constant->m_lineNo, m_constant->m_columnNo, semantic_error_method, "static const method is not allowed");
		}
	}

	std::vector<VariableNode*> resultNodes;
	m_resultList->collectVariableNodes(resultNodes);
	for (VariableNode* resultNode : resultNodes)
	{
		resultNode->checkSemantic(templateArguments);
	}

	std::vector<VariableNode*> parameterNodes;
	m_parameterList->collectVariableNodes(parameterNodes);
	for (VariableNode* parameterNode : parameterNodes)
	{
		parameterNode->checkSemantic(templateArguments);
	}

	CheckFullParameterNames(resultNodes, parameterNodes);
}
