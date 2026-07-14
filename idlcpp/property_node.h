#pragma once

#include "member_node.h"

struct CompoundTypeNode;
struct PropertyAccessorListNode;

struct PropertyNode : MemberNode
{
	TokenNode* m_modifier{ nullptr };
	CompoundTypeNode* m_compoundType{ nullptr };
	PropertyAccessorListNode* m_accessorList{ nullptr };
	PropertyKind m_propertyKind{ simple_property };
public:
	PropertyNode(CompoundTypeNode* compoundType, IdentifierNode* name, PropertyAccessorListNode* accessorList, PropertyKind propertyKind);
	PropertyKind getKind();
	bool isStatic();
	bool isSimple();
	bool isFixedArray();
	bool isDynamicArray();
	bool isList();
	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
