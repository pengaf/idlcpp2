#pragma once

#include "syntax_node_impl.h"
#include <string>
#include <vector>
#include <map>

struct TokenNode;
struct IdentifierNode;
struct ScopeNameListNode;
struct TypeNameListNode;
struct ScopeNode;
struct TypeInfo;
struct TypeNameNode;
struct TemplateClassInstanceNode;
struct TypeNode;
struct TemplateArguments;

enum TypeKind;

struct TypeNameNode : SyntaxNodeImpl
{
	TokenNode* m_keyword{ nullptr };
	ScopeNameListNode* m_scopeNameList{ nullptr };
	TypeNode* m_startTypeNode{ nullptr };
	TypeNode* m_typeNode{ nullptr };
	bool m_noCode{ false };
	bool m_noMeta{ false };
public:
	TypeNameNode(TokenNode* keyword, PredefinedType primitiveType);
	TypeNameNode(ScopeNameListNode* scopeNameList);
	bool isNoCode() const;
	bool isNoMeta() const;
	bool calcTypeNodes(TypeNode* enclosingTypeTreeNode, TemplateArguments* templateArguments);
	TypeNode* getTypeNode(TemplateArguments* templateArguments);
	TypeNode* getActualTypeNode(TemplateArguments* templateArguments);
	void getString(std::string& str);
	void getRelativeName(std::string& typeName, ScopeNode* scopeNode);
	bool underTemplateParameter();
};

