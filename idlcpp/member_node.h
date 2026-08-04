#pragma once

#include "entity_node.h"
#include "type_name_node.h"
#include <vector>
#include <string>
#include <map>

struct ProgramNode;
struct IdentifierNode;
struct ScopeNode;
struct ClassNode;
struct NamespaceNode;
struct TypeNameNode;
struct TypeNode;
struct TemplateArguments;
struct AttributeListNode;
enum TypeKind;
class SourceFile;

enum TypeTreeFilter
{
	ttf_type = 1,
	ttf_template_instance = 2,
	ttf_typename = 4,
	ttf_typedef = 8,
	ttf_all = 0xF,
};

struct MemberNode : EntityNode
{
	ScopeNode* m_enclosing{ nullptr };
	IdentifierNode* m_nativeName{ nullptr };
	TokenNode* m_semicolon{ nullptr };
	int m_orderIndex;//for field & property syntax order
	TokenNode* m_filter{ nullptr };
public:
	bool isNoCode() const;
	bool isNoMeta() const;
	bool canGenerateMetaCode();
	bool canGenerateNativeCode();
	void getEnclosings(std::vector<ScopeNode*>& enclosings);
	bool isNamespace();
	bool isTemplateClass();
	bool isTypedef();
	void getFullName(std::string& fullName, TemplateArguments* templateArguments);
	void getNativeName(std::string& nativeName, TemplateArguments* templateArguments);
	ProgramNode* getProgramNode();
	SourceFile* getSourceFile();
	std::string getSourceFilePath();
	virtual TypeNode* getTypeNode();
	virtual void getLocalName(std::string& name, TemplateArguments* templateArguments);
	virtual void collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};


struct CompareMemberNodeByName
{
	bool operator()(const MemberNode* m1, const MemberNode* m2) const;
};