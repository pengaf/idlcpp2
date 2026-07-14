#pragma once
#include "syntax_node_impl.h"
#include <vector>
#include <string>


struct IdentifierNode;
struct TokenNode;
struct TypeNameListNode;
struct TypeNode;
struct TemplateArguments;

struct ScopeNameNode : SyntaxNodeImpl
{
	IdentifierNode* m_name;
	TokenNode* m_leftBracket;
	TypeNameListNode* m_parameterList;
	TokenNode* m_rightBracket;
public:
	ScopeNameNode(IdentifierNode* name, TokenNode* leftBracket, TypeNameListNode* parameterList, TokenNode* rightBracket);
	bool calcTemplateParametersTypeNodes(TypeNode* enclosingTypeTreeNode, TemplateArguments* templateArguments);
	void getString(std::string& str);
	bool isTemplateForm();
};

