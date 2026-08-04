#pragma once

#include "member_node.h"
#include "template_arguments.h"
#include <vector>

struct TypeNameListNode;
struct IdentifierListNode;
struct TemplateClassInstanceTypeNode;
struct ClassTypeNode;

struct TemplateClassInstanceNode : MemberNode
{
	TypeNameListNode* m_parameterList{ nullptr };
	IdentifierListNode* m_reservedMemberList{ nullptr };
	TemplateClassInstanceTypeNode* m_typeNode{ nullptr };
	ClassTypeNode* m_classTypeNode{ nullptr };
	TemplateArguments m_templateArguments;
public:
	TemplateClassInstanceNode(IdentifierNode* name, TypeNameListNode* parameterList);
	virtual TypeNode* getTypeNode();
	virtual void getLocalName(std::string& name, TemplateArguments* templateArguments);
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
	size_t getParameterCount();
	void getReservedMembers(std::vector<IdentifierNode*>& reservedMembers);
};
