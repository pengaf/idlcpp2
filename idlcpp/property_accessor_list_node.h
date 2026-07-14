#pragma once

#include "syntax_node_impl.h"
#include <vector>

struct PropertyAccessorNode;

struct PropertyAccessorListNode : SyntaxNodeImpl
{
	PropertyAccessorListNode* m_propertyAccessorList{ nullptr };
	PropertyAccessorNode* m_propertyAccessor{ nullptr };
public:
	PropertyAccessorListNode(PropertyAccessorListNode* propertyAccessorList, PropertyAccessorNode* propertyAccessor);
	void collectPropertyAccessors(std::vector<PropertyAccessorNode*>& propertyAccessors);
};


