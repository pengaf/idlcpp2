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
public:
	MethodNode(VariableListNode* resultList, IdentifierNode* name, TokenNode* leftParenthesis, VariableListNode* parameterList, TokenNode* rightParenthesis);
	bool isStatic();
	bool isVirtual();
	bool isAbstract();
	bool byValue();
	bool byRef();
	bool byObserverPtr();
	bool byUniquePtr();
	bool bySharedPtr();
	bool byPtr();
	bool returnsOwning();
	void setResultOwning(bool resultOwning = true);
	size_t getParameterCount() const;
	void calcManglingName(std::string& name, TemplateArguments* templateArguments);
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
