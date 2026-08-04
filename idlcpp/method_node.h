#pragma once
#include "member_node.h"

struct VariableListNode;

struct MethodNode : MemberNode
{
public:
	TokenNode* m_modifier{ nullptr };
	VariableListNode* m_resultList{ nullptr };
	TokenNode* m_leftParenthesis{ nullptr };
	VariableListNode* m_parameterList{ nullptr };
	TokenNode* m_rightParenthesis{ nullptr };
	mutable uint32_t m_resultCount{ uint32_t(-1) };
	mutable uint32_t m_parameterCount{ uint32_t(-1) };
public:
	MethodNode(VariableListNode* resultList, IdentifierNode* name, TokenNode* leftParenthesis, VariableListNode* parameterList, TokenNode* rightParenthesis);
	bool isStatic();
	bool isVirtual();
	uint32_t getResultCount() const;
	uint32_t getParameterCount() const;
	void calcManglingName(std::string& name, TemplateArguments* templateArguments);
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
