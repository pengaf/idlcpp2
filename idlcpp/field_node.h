#pragma once

#include "member_node.h"

struct CompoundTypeNode;

struct FieldNode : MemberNode
{
	TokenNode* m_modifier{ nullptr };
	CompoundTypeNode* m_compoundType{ nullptr };
	TokenNode* m_leftBracket{ nullptr };
	TokenNode* m_rightBracket{ nullptr };
public:
	FieldNode(CompoundTypeNode* compoundType, IdentifierNode* name, TokenNode* leftBracket, TokenNode* rightBracket);
	bool isStatic() const;
	bool isArray() const;
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
