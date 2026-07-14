#include "property_accessor_list_node.h"

PropertyAccessorListNode::PropertyAccessorListNode(PropertyAccessorListNode* propertyAccessorList, PropertyAccessorNode* propertyAccessor)
{
	m_propertyAccessorList = propertyAccessorList;
	m_propertyAccessor = propertyAccessor;
}

void PropertyAccessorListNode::collectPropertyAccessors(std::vector<PropertyAccessorNode*>& propertyAccessors)
{
	PropertyAccessorListNode* list = this;
	while (0 != list)
	{
		propertyAccessors.push_back(list->m_propertyAccessor);
		list = list->m_propertyAccessorList;
	}
	std::reverse(propertyAccessors.begin(), propertyAccessors.end());
}
