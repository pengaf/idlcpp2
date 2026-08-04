#pragma once
#include "scope_node.h"
#include "template_arguments.h"
#include <vector>

struct TokenNode;
struct IdentifierListNode;
struct TypeNameListNode;
struct MemberListNode;
struct MethodNode;
struct TemplateParametersNode;
struct TypeNameListNode;
struct TemplateArguments;
struct ClassTypeNode;

struct ClassNode : ScopeNode
{
	enum LazyBool
	{
		lb_false,
		lb_true,
		lb_unknown,
	};
	TokenNode* m_keyword{ nullptr };
	IdentifierListNode* m_conceptList{ nullptr };
	IdentifierNode* m_metadataKind{ nullptr };
	TokenNode* m_colon{ nullptr };
	TypeNameListNode* m_baseList{ nullptr };
	TemplateParametersNode* m_templateParametersNode{ nullptr };
	ClassTypeNode* m_typeNode{ nullptr };
	TemplateArguments m_templateArguments{};
	std::vector<MethodNode*> m_additionalMethods;//New NewArray
	//LazyBool m_abstractFlag{ lb_unknown };
	//LazyBool m_copyableFlag{ lb_unknown };
	mutable LazyBool m_isDerivedFromObject{ lb_unknown };
	mutable LazyBool m_isDerivedFromInterface{ lb_unknown };
public:
	ClassNode(TokenNode* keyword, IdentifierListNode* conceptList, IdentifierNode* name);
	void setTemplateParameters(TemplateParametersNode* templateParametersNode);
	void setMemberList(TokenNode* leftBrace, MemberListNode* memberList, TokenNode* rightBrace);
	void buildAdditionalMethods();
	void extendInternalCode(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	bool isInterface() const;
	bool isStruct() const;
	bool isClass() const;
	bool isDerivedFromObject() const;
	bool isDerivedFromInterface() const;

	bool hasAdditionalMethods() const;
	bool needSubclassProxy(TemplateArguments* templateArguments);
	//bool isCopyableClass(TemplateArguments* templateArguments);
	bool hasOverrideMethod(TemplateArguments* templateArguments);

	bool isAdditionalMethod(MethodNode* methodNode);
	void collectOverrideMethods(std::vector<MethodNode*>& methodNodes, TemplateArguments* templateArguments);
	void generateNewMethod(const char* methodName, MethodNode* constructor);
	void generateNewArrayMethod(const char* methodName, MethodNode* constructor);

	virtual TypeNode* getTypeNode();
	virtual void getLocalName(std::string& name, TemplateArguments* templateArguments);
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
