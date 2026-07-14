#pragma once
#include "member_node.h"

struct TokenNode;
struct ScopeNameListNode;
struct TypeNameNode;
struct TypeDeclarationTypeNode;

struct TypeDeclarationNode : MemberNode
{
	TypeKind m_typeKind;
	TypeDeclarationTypeNode* m_typeNode;
public:
	TypeDeclarationNode(IdentifierNode* name, TypeKind typeKind);
	virtual TypeNode* getTypeNode();
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
};

