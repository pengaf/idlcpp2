#pragma once

#include "syntax_node_impl.h"

struct TokenNode;
struct TypeNameNode;
struct IdentifyNode;

struct GetterSetterNode : SyntaxNodeImpl
{
	TokenNode* m_keyword;
	IdentifyNode* m_nativeName;
	bool m_setterAllowNull;
public:
	GetterSetterNode(TokenNode* keyword);
	bool isSetterAllowNull();
};
