#include "enumerator_node.h"
#include "identifier_node.h"
#include "attribute_list_node.h"
#include <assert.h>

EnumeratorNode::EnumeratorNode(IdentifierNode* name, TokenNode* equalSign)
{
	m_nodeType = snt_enumerator;
	m_name = name;
	m_equalSign = equalSign;
}

void EnumeratorNode::checkSemantic()
{
	if (m_attributeList)
	{
		m_attributeList->checkSemantic();
	}
}

bool CompareEnumeratorPtr::operator()(const EnumeratorNode* arg1, const EnumeratorNode* arg2) const
{
	return arg1->m_name->m_str < arg2->m_name->m_str;
}

