#pragma once
#include "member_node.h"

struct TokenNode;
struct EnumeratorListNode;
struct EnumTypeNode;

struct EnumNode : MemberNode
{
	TokenNode* m_keyword{ nullptr };
	TokenNode* m_keyword2{ nullptr };
	TokenNode* m_leftBrace{ nullptr };
	EnumeratorListNode* m_enumeratorList{ nullptr };
	TokenNode* m_rightBrace{ nullptr };
	EnumTypeNode* m_typeNode{ nullptr };
public:
	EnumNode(TokenNode* keyword, TokenNode* keyword2, IdentifierNode* name, TokenNode* leftBrace, EnumeratorListNode* enumeratorList, TokenNode* rightBrace);
	virtual TypeNode* getTypeNode();
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
	bool isStronglyTypedEnum();
};

