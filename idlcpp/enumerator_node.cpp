#include "enumerator_node.h"
#include "identify_node.h"
#include "attribute_list_node.h"
#include <assert.h>

EnumeratorNode::EnumeratorNode(AttributeListNode* attributeList, IdentifyNode* name)
{
	m_nodeType = snt_enumerator;
	m_attributeList = attributeList;
	m_name = name;
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

