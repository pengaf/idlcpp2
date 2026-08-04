#pragma once

#include "entity_node.h"

struct CompoundTypeNode;
struct IdentifierNode;
struct TokenNode;
struct TemplateArguments;

struct VariableNode : EntityNode
{
	CompoundTypeNode* m_compoundType{ nullptr };
	TokenNode* m_byRef{ nullptr };
	bool m_allowNull{ false };
public:
	VariableNode(CompoundTypeNode* compoundType, TokenNode* byRef, IdentifierNode* name, bool allowNull);
	bool isByRef() const;
	bool isAllowNull() const;
	void checkSemantic(TemplateArguments* templateArguments);
};
