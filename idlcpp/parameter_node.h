#pragma once

#include "syntax_node_impl.h"

struct TokenNode;
struct TypeNameNode;
struct IdentifyNode;
struct MethodNode;
struct TemplateArguments;

struct ParameterNode : SyntaxNodeImpl
{
	TokenNode* m_constant;
	TypeNameNode* m_typeName;
	TokenNode* m_out;
	TokenNode* m_typeCompound;
	TokenNode* m_byRef;
	IdentifyNode* m_name;
	bool m_array;
	bool m_allowNull;
public:
	ParameterNode(TypeNameNode* typeName, TokenNode* out, TokenNode* typeCompound, TokenNode* byRef, IdentifyNode* name);
	bool isConstant();
	bool isByValue();
	bool isByRef();
	bool isByPtr();
	bool isByObserverPtr();
	bool isByUniquePtr();
	bool isBySharedPtr();
	bool isInput();
	bool isOutput();
	bool isOutputPtr();
	bool isOutputRef();
	bool isArray();
	bool isAllowNull();
	void checkSemantic(TemplateArguments* templateArguments);
};
