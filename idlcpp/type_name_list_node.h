#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct TokenNode;
struct TypeNameNode;

struct TypeNameListNode : SyntaxNodeImpl
{
	TypeNameListNode* m_typeNameList;
	TokenNode* m_comma;
	TypeNameNode* m_typeName;
public:
	TypeNameListNode(TypeNameListNode* typeNameList, TokenNode* comma, TypeNameNode* typeName);
	void collectTypeNameNodes(std::vector<TypeNameNode*>& typeNameNodes);
	void collectTypeNameNodes(std::vector<std::pair<TokenNode*, TypeNameNode*>>& typeNameNodes);
	void collectTypeNameNodesNotNoCode(std::vector<std::pair<TokenNode*, TypeNameNode*>>& typeNameNodes);
	void collectTypeNameNodesNotNoMeta(std::vector<std::pair<TokenNode*, TypeNameNode*>>& typeNameNodes);
};