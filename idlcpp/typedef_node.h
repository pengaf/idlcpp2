#pragma once
#include "member_node.h"

struct TokenNode;
struct TypeNameNode;
struct TypedefTypeNode;

struct TypedefNode : MemberNode
{
	TokenNode* m_keyword{ nullptr };
	TypeNameNode* m_typeName{ nullptr };
	TypedefTypeNode* m_typeNode{ nullptr };
	TypeNode* m_srcTypeNode{ nullptr };
public:
	TypedefNode(TokenNode* keyword, TypeNameNode* typeName, IdentifierNode* name);
	TypeNode* getActualTypeNode(TemplateArguments* templateArguments);
	virtual TypeNode* getTypeNode();
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};

