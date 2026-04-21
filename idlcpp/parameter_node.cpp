#include "parameter_node.h"
#include "type_name_node.h"
#include "identify_node.h"
#include "method_node.h"
#include "class_node.h"
#include "raise_error.h"
#include "type_tree.h"
#include "compiler.h"
#include <assert.h>

ParameterNode::ParameterNode(TypeNameNode* typeName, TokenNode* out, TokenNode* typeCompound, TokenNode* byRef, IdentifyNode* name)
{
	m_nodeType = snt_parameter;
	m_constant = 0;
	m_typeName = typeName;
	m_out = out;
	m_typeCompound = typeCompound;
	m_byRef = byRef;
	m_name = name;
	m_array = false;
	m_allowNull = false;
}

bool ParameterNode::isConstant()
{
	return (0 != m_constant && snt_keyword_const == m_constant->m_nodeType);
}

bool ParameterNode::isByValue()
{
	return 0 == m_typeCompound && 0 == m_byRef;
}

bool ParameterNode::isByRef()
{
	return 0 != m_byRef;
}

bool ParameterNode::isByPtr()
{
	return isByObserverPtr() || isByUniquePtr() || isBySharedPtr();
}

bool ParameterNode::isByObserverPtr()
{
	return (0 != m_typeCompound && '*' == m_typeCompound->m_nodeType);
}

bool ParameterNode::isByUniquePtr()
{
	return (0 != m_typeCompound && '!' == m_typeCompound->m_nodeType);
}

bool ParameterNode::isBySharedPtr()
{
	return (0 != m_typeCompound && '^' == m_typeCompound->m_nodeType);
}

bool ParameterNode::isArray()
{
	return m_array;
}

bool ParameterNode::isInput()
{
	return 0 == m_out;
}

bool ParameterNode::isOutput()
{
	return 0 != m_out;
}

bool ParameterNode::isOutputPtr()
{
	return (0 != m_out && '*' == m_out->m_nodeType);
}

bool ParameterNode::isOutputRef()
{
	return (0 != m_out && '&' == m_out->m_nodeType);
}

bool ParameterNode::isAllowNull()
{
	return m_allowNull;
}

void ParameterNode::checkSemantic(TemplateArguments* templateArguments)
{
	TypeNode* typeNode = m_typeName->getTypeNode(templateArguments);
	if(0 == typeNode)
	{
		return;
	}
	if(void_type == typeNode->getTypeCategory(templateArguments))
	{ 
		if ((isInput() && !isByObserverPtr()) || isOutput())
		{
			RaiseError_InvalidParameterType(this);
		}
	}
	//else if (primitive_type == typeNode->getTypeCategory(templateArguments))
	//{
	//	if (isByRef() && !isConstant())
	//	{
	//		RaiseError_InvalidParameterType(this);
	//	}
	//}

	g_compiler.useType(typeNode, templateArguments, isByValue() ? tu_use_definition : tu_use_declaration, m_typeName);
}
