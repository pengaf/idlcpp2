#include "type_declaration_node.h"
#include "type_name_node.h"
#include "raise_error.h"
#include "compiler.h"
#include "type_tree.h"
#include <assert.h>

TypeDeclarationNode::TypeDeclarationNode(IdentifierNode* name, TypeKind typeKind)
{
	m_nodeType = snt_type_declaration;
	m_name = name;
	m_typeKind = typeKind;
	m_typeNode = 0;
}

TypeNode* TypeDeclarationNode::getTypeNode()
{
	return m_typeNode;
}

void TypeDeclarationNode::collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	//assert(enclosingTypeNode->isNamespace() || enclosingTypeNode->isClass());
	assert(0 == m_typeNode);

	switch (enclosingTypeNode->m_kind)
	{
	case tc_namespace:
		m_typeNode = static_cast<NamespaceTypeNode*>(enclosingTypeNode)->addTypeDeclaration(this);
		break;
	case tc_class_type:
		m_typeNode = static_cast<ClassTypeNode*>(enclosingTypeNode)->addTypeDeclaration(this);
		break;
	default:
		assert(false);
	}
}
