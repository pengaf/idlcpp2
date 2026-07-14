#pragma once

#include "syntax_node_impl.h"

struct TypeNameNode;

struct CompoundTypeNode : SyntaxNodeImpl
{
	TypeNameNode* m_typeName {nullptr};
	TypeCompound m_typeCompound { tc_none };
public:
	CompoundTypeNode(TypeNameNode* typeName, TypeCompound typeCompound);
	bool isObserverPtr() const;
	bool isSharedPtr() const;
	bool isObserverArray() const;
	bool isSharedArray() const;
	bool isSmartPtr() const;
};
