#include "attribute_list_node.h"
#include "attribute_node.h"
#include "identifier_node.h"
#include "error_list.h"
#include "utility.h"


AttributeListNode::AttributeListNode(AttributeListNode* attributeList, AttributeNode* attribute)
{
	m_nodeType = snt_attribute_list;
	m_attributeList = attributeList;
	m_attribute = attribute;
}

void AttributeListNode::collectAttributeNodes(std::vector<AttributeNode*>& attributeNodes)
{
	AttributeListNode* list = this;
	while (0 != list)
	{
		attributeNodes.push_back(list->m_attribute);
		list = list->m_attributeList;
	}
}

void AttributeListNode::checkSemantic()
{
	std::vector<AttributeNode*> attributeNodes;
	collectAttributeNodes(attributeNodes);
	checkAttributeNames(attributeNodes);
}

void checkAttributeNames(std::vector<AttributeNode*>& attributeNodes)
{
	std::set<IdentifierNode*, CompareIdentifierPtr> items;
	size_t count = attributeNodes.size();
	for (size_t i = 0; i < count; ++i)
	{
		IdentifierNode* identifier = attributeNodes[i]->m_name;
		auto res = items.insert(identifier);
		if (!res.second)
		{
			char buf[4096];
			sprintf_s(buf, "\'%s\' : attribute already defined at line %d, column %d", identifier->m_str.c_str(),
				(*res.first)->m_lineNo, (*res.first)->m_columnNo);
			ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
				identifier->m_columnNo, semantic_error_attribute_redefined, buf);
		}
	}
}