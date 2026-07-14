#pragma once

#include "yyfuncs.h"
#include <string>
#include <set>
#include <vector>

struct IdentifierNode;
struct MemberNode;
struct NamespaceNode;
struct EnumNode;
struct ClassNode;
struct TypedefNode;
struct TypeDeclarationNode;
struct TemplateClassInstanceNode;
struct ScopeNameNode;
struct ScopeNameListNode;
class SourceFile;
struct TemplateArguments;
class TypeTreeNode;
enum TypeKind;

enum PredefinedType;

enum TypeNodeKind
{
	tc_predefined_type,
	tc_enum_type,
	tc_class_type,
	tc_template_paramter,
	tc_template_class_instance,
	tc_typedef,
	tc_type_declaration,
	tc_namespace,
};

struct TypeNode
{
	TypeNodeKind m_kind;
	TypeNode* m_enclosing;
	std::string m_name;
	IdentifierNode* m_identifierNode;
	SourceFile* m_sourceFile;
public:
	struct CompareTypeNodePtrByName
	{
		bool operator()(const TypeNode* lhs, const TypeNode* rhs) const;
	};
	struct TypeNodeContainer : public std::set<TypeNode*, CompareTypeNodePtrByName>
	{
		TypeNode* getTypeNodeByName(const std::string& name);
		template<typename T>
		T* addTypeNode(TypeNode* enclosing, const std::string& name, IdentifierNode* identifierNode);
	};
	TypeNode();
	virtual ~TypeNode();
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
	virtual TypeNode* getChildNode(const std::string& name);
	virtual TypeNode* getActualTypeNode(TemplateArguments* templateArguments);
	virtual void getActualTypeFullName(std::string& name);
	virtual void getLocalName(std::string& name);
	void getFullName(std::string& name);
	void getNativeName(std::string& name);
	TypeNode* getEnclosing();
	void getEnclosings(std::vector<TypeNode*>& enclosings);
	bool isNamespace();
	bool isPredefinedType();
	bool isEnum();
	bool isClass();
	bool isTemplateClass();
	bool isUnderTemplateClass();
	bool isTemplateParameter();
	bool isTemplateClassInstance();
	bool isTypedef();
	bool isTypeDeclaration();
};


struct PredefinedTypeNode : TypeNode
{
	PredefinedType m_type;
public:
	PredefinedTypeNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
};

struct EnumTypeNode : TypeNode
{
	EnumNode* m_enumNode;
public:
	EnumTypeNode();
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
};

struct TypedefTypeNode : TypeNode
{
	TypedefNode* m_typedefNode;
public:
	TypedefTypeNode();
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
	virtual TypeNode* getActualTypeNode(TemplateArguments* templateArguments);
	virtual void getActualTypeFullName(std::string& name);
};

struct TypeDeclarationTypeNode : TypeNode
{
	TypeDeclarationNode* m_typeDeclarationNode;
public:
	TypeDeclarationTypeNode();
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
};

struct TemplateParameterTypeNode : TypeNode
{
	TemplateParameterTypeNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
	virtual TypeNode* getActualTypeNode(TemplateArguments* templateArguments);
};

struct ClassTypeNode : TypeNode
{
	ClassNode* m_classNode;
	std::vector<TemplateParameterTypeNode*> m_parameterNodes;
	TypeNodeContainer m_children;
public:
	ClassTypeNode();
	EnumTypeNode* addEnum(EnumNode* node);
	ClassTypeNode* addClass(ClassNode* node);
	TypedefTypeNode* addTypedef(TypedefNode* node);
	TypeDeclarationTypeNode* addTypeDeclaration(TypeDeclarationNode* node);
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
	virtual TypeNode* getChildNode(const std::string& name);
};

struct TemplateClassInstanceTypeNode : ClassTypeNode
{
	std::string m_localName;
	TemplateClassInstanceNode* m_templateClassInstanceNode;
public:
	TemplateClassInstanceTypeNode();
	virtual MemberNode* getSyntaxNode();
	virtual TypeKind getTypeKind(TemplateArguments* templateArguments);
	virtual void getLocalName(std::string& name);
};

struct NamespaceTypeNode : TypeNode
{
	TypeNodeContainer m_children;
public:
	NamespaceTypeNode();
	NamespaceTypeNode* addNamespace(NamespaceNode* node);
	EnumTypeNode* addEnum(EnumNode* node);
	ClassTypeNode* addClass(ClassNode* node);
	TypedefTypeNode* addTypedef(TypedefNode* node);
	TypeDeclarationTypeNode* addTypeDeclaration(TypeDeclarationNode* node);
	TemplateClassInstanceTypeNode* addTemplateClassInstance(TemplateClassInstanceNode* node);
	virtual TypeNode* getChildNode(const std::string& name);
};

class TypeTree
{
public:
	TypeTree();
	~TypeTree();
	NamespaceTypeNode* getRootNamespaceTypeNode();
	PredefinedTypeNode* getPredefinedTypeNode(PredefinedType);

	bool checkTypeNameNode(TypeNode*& initialTypeTreeNode, TypeNode*& finalTypeTreeNode,
		ScopeNameListNode* scopeNameListNode, TypeNode* enclosingTypeTreeNode, TemplateArguments* templateArguments);

	bool findNodeByScopeNames(TypeNode*& initialTypeTreeNode, TypeNode*& finalTypeTreeNode,
		const std::vector<ScopeNameNode*>& scopeNameNodes, TypeNode* enclosingTypeTreeNode, TemplateArguments* templateArguments);

	TypeNode* findNodeByScopeName(ScopeNameNode* scopeNameNode, TypeNode* enclosingTypeTreeNode, TemplateArguments* templateArguments);

public:
	NamespaceTypeNode m_root;
	std::vector<PredefinedTypeNode> m_predefinedTypeNodes;
	std::vector<NamespaceTypeNode*> m_allNamespaces;
};

extern TypeTree g_typeTree;