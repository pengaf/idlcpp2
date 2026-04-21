#pragma once

#include "syntax_node_impl.h"

struct TokenNode;
struct TypeNameNode;

struct VariableTypeNode : SyntaxNodeImpl
{
	TypeNameNode* m_typeName;
	TokenNode* m_constant;
	TokenNode* m_typeCompound;
	TokenNode* m_byRef;
public:
	VariableTypeNode(TypeNameNode* typeName, TokenNode* constant, TokenNode* typeCompound);
	bool isByValue() const;
	bool isByRef() const;
	bool isByObserverPtr() const;
	bool isByUniquePtr() const;
	bool isBySharedPtr() const;
};
