#include "program_node.h"
#include "member_list_node.h"
#include "namespace_node.h"
#include "identify_node.h"
#include "class_node.h"
#include "type_tree.h"

#include "options.h"
#include "platform.h"
#include <assert.h>
#include <vector>

const char g_pragmaOnce[] = {"#pragma once\n\n"};

ProgramNode::ProgramNode(MemberListNode* memberList)
: NamespaceNode(0, 0, 0, memberList, 0)
{
	m_sourceFile = 0;
}

void ProgramNode::getLocalName(std::string& name, TemplateArguments* templateArguments)
{
	name = "";
}

void ProgramNode::collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	assert(0 == enclosingTypeNode && 0 == m_typeNode && 0 == templateArguments);
	m_typeNode = g_typeTree.getRootNamespaceTypeNode();
	m_memberList->collectTypes(m_typeNode, templateArguments);
}

