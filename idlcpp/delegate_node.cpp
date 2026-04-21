#include "delegate_node.h"
#include "parameter_list_node.h"
#include "parameter_node.h"
#include "type_name_node.h"
#include "identify_node.h"
#include "class_node.h"
#include "error_list.h"
#include "raise_error.h"
#include "type_tree.h"
#include "compiler.h"
#include "scope_name_list_node.h"
#include "type_name_list_node.h"
#include "scope_name_node.h"
#include "method_node.h"


#include <set>
#include <assert.h>

const char g_strPure[] = {" = 0 "};

DelegateNode::DelegateNode(IdentifyNode* name, TokenNode* leftParenthesis, 
	ParameterListNode* parameterList, TokenNode* rightParenthesis, TokenNode* semicolon)
{
	m_nodeType = snt_delegate;
	m_keyword = 0;
	m_resultConst = 0;
	m_resultTypeName = 0;
	m_typeCompound = 0;
	m_byRef = 0;
	m_name = name;
	m_leftParenthesis = leftParenthesis;
	m_parameterList = parameterList;
	m_rightParenthesis = rightParenthesis;
	m_semicolon = semicolon;
	m_resultArray = false;
	m_resultOwning = false;
	m_parameterCount = size_t(-1);
	m_classNode = 0;
}

bool DelegateNode::byValue()
{
	return 0 == m_typeCompound && 0 == m_byRef && !m_resultOwning;
}

bool DelegateNode::byRef()
{
	return 0 != m_byRef;
}

bool DelegateNode::byPtr()
{
	return byObserverPtr() || byUniquePtr() || bySharedPtr();
}

bool DelegateNode::byObserverPtr()
{
	return (0 != m_typeCompound && '*' == m_typeCompound->m_nodeType);
}

bool DelegateNode::byUniquePtr()
{
	return (0 != m_typeCompound && '!' == m_typeCompound->m_nodeType);
}

bool DelegateNode::bySharedPtr()
{
	return (0 != m_typeCompound && '^' == m_typeCompound->m_nodeType);
}

bool DelegateNode::returnsOwning()
{
	return m_resultOwning;
}

void DelegateNode::setResultOwning(bool resultOwning)
{
	m_resultOwning = resultOwning;
}

size_t DelegateNode::getParameterCount() const
{
	if (size_t(-1) == m_parameterCount)
	{
		size_t res = 0;
		ParameterListNode* list = m_parameterList;
		while(0 != list)
		{
			++res;
			list = list->m_parameterList;
		}
		m_parameterCount = res;
	}
	return m_parameterCount;
}

void DelegateNode::collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	m_classNode->collectTypes(enclosingTypeNode, templateArguments);
}

void DelegateNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	//if (0 != m_resultTypeName)
	//{
	//	m_resultTypeName->calcTypeNodes(enclosingTypeNode, templateArguments);
	//}

	//std::vector<ParameterNode*> parameterNodes;
	//m_parameterList->collectParameterNodes(parameterNodes);
	//auto it = parameterNodes.begin();
	//auto end = parameterNodes.end();
	//for (; it != end; ++it)
	//{
	//	ParameterNode* parameterNode = *it;
	//	parameterNode->m_typeName->calcTypeNodes(enclosingTypeNode, templateArguments);
	//}

	m_classNode->checkTypeNames(enclosingTypeNode, templateArguments);
}

void DelegateNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);

	if(0 != m_resultTypeName)
	{
		TypeNode* typeNode = m_resultTypeName->getTypeNode(templateArguments);
		if (0 == typeNode)
		{
			return;
		}
		if (void_type == typeNode->getTypeCategory(templateArguments))
		{
			if ((0 != m_typeCompound && '*' != m_typeCompound->m_nodeType) || 0 != m_byRef)
			{
				RaiseError_InvalidResultType(this);
			}
		}
		g_compiler.useType(typeNode, templateArguments, byValue() ? tu_use_definition : tu_use_declaration, m_resultTypeName);
	}

	std::vector<ParameterNode*> parameterNodes;
	m_parameterList->collectParameterNodes(parameterNodes);
	checkParameterNames(parameterNodes);

	size_t parameterCount = parameterNodes.size();
	for(size_t i = 0; i < parameterCount; ++i)
	{
		parameterNodes[i]->checkSemantic(templateArguments);
	}

	m_classNode->checkSemantic(templateArguments);
}

void DelegateNode::extendInternalCode(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	ClassNode* classNode = (ClassNode*)newClass(m_keyword, 0, m_name);

	IdentifyNode* nsIdentifyNode = (IdentifyNode*)newIdentify("pafcore");
	ScopeNameNode* nsScopeNameNode = (ScopeNameNode*)newScopeName(nsIdentifyNode, 0, 0, 0);

	IdentifyNode* baseIdentifyNode = (IdentifyNode*)newIdentify("Delegate");
	ScopeNameNode* baseScopeNameNode = (ScopeNameNode*)newScopeName(baseIdentifyNode, 0, 0, 0);

	ScopeNameListNode* baseScopeNameListNode = (ScopeNameListNode*)newScopeNameList(newScopeNameList(0, nsScopeNameNode), baseScopeNameNode);

	TypeNameNode* baseTypeNameNode = (TypeNameNode*)newTypeName(baseScopeNameListNode);
	TypeNameListNode* baseTypeNameListNode = (TypeNameListNode*)newTypeNameList(0, 0, baseTypeNameNode);
	setClassBaseList(classNode, 0, baseTypeNameListNode);

	IdentifyNode* invokeIdentifyNode = (IdentifyNode*)newIdentify("invoke");
	MethodNode* invokeMethodNode = (MethodNode*)newMethod(invokeIdentifyNode, m_leftParenthesis, m_parameterList, m_rightParenthesis, NULL);
	SyntaxNode* invokeResultType = newVariableType(m_resultTypeName, m_typeCompound);
	if (m_byRef)
	{
		setVariableTypeRef(invokeResultType, m_byRef);
	}
	setMethodResult(invokeMethodNode, invokeResultType);
	if (m_resultConst)
	{
		setMethodResultConst(invokeMethodNode, m_resultConst);
	}
	MemberListNode* classMemberListNode  = (MemberListNode*)newClassMemberList(0, invokeMethodNode);
	classNode->setMemberList(0, classMemberListNode, 0);

	invokeMethodNode->m_enclosing = classNode;
	classNode->m_enclosing = m_enclosing;
	m_classNode = classNode;
}
