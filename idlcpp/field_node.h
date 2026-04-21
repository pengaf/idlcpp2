#pragma once

#include "member_node.h"

struct TokenNode;
struct TypeNameNode;

struct FieldNode : MemberNode
{
	TokenNode* m_static;
	TokenNode* m_constant;
	TypeNameNode* m_typeName;
	TokenNode* m_typeCompound;
	TokenNode* m_leftBracket;
	TokenNode* m_rightBracket;
	TokenNode* m_semicolon;
	bool m_smartArray;
public:
	FieldNode(TypeNameNode* typeName, TokenNode* typeCompound, IdentifyNode* name, TokenNode* leftBracket, TokenNode* rightBracket);
	bool isStatic();
	bool isConstant();
	bool isPointer();
	bool isObserverPtr();
	bool isUniquePtr();
	bool isSharedPtr();
	bool isArray();
	bool isSmartArray();
	bool isObserverArray();
	bool isUniqueArray();
	bool isSharedArray();
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
