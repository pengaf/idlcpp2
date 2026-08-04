#pragma once

#include "syntax_node_impl.h"

struct TypeNameNode;

struct CompoundTypeNode : SyntaxNodeImpl
{
	TypeNameNode* m_typeName {nullptr};
	TypeCompound m_typeCompound { tc_none };
public:
	CompoundTypeNode(TypeNameNode* typeName, TypeCompound typeCompound);
	bool isNotPtr() const;
	bool isRawPtr() const;
	bool isSharedPtr() const;
	bool isObserverPtr() const;
	bool isSharedArray() const;
	bool isObserverArray() const;
};
