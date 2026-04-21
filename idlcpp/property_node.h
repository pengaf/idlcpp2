#pragma once

#include "member_node.h"

struct GetterSetterNode;
struct TypeNameNode;

struct PropertyNode : MemberNode
{
	TokenNode* m_modifier;
	TokenNode* m_constant;
	TypeNameNode* m_typeName;
	TokenNode* m_typeCompound;//* ! ^
	TokenNode* m_byRef;
	GetterSetterNode* m_get;
	GetterSetterNode* m_set;
	PropertyCategory m_propertyCategory;
	bool m_candidate;
	TypeNameNode* m_keyTypeName;
	TokenNode* m_keyTypeCompound;//* ! ^
	TokenNode* m_keyByRef;
public:
	PropertyNode(IdentifyNode* name, PropertyCategory category);
	PropertyCategory getCategory();
	bool isStatic();
	bool isSimple();
	bool isFixedArray();
	bool isDynamicArray();
	bool isList();
	bool isMap();
	bool hasCandidate();
	bool isKeyByPtr();
	bool isKeyByObserverPtr();
	bool isKeyByUniquePtr();
	bool isKeyBySharedPtr();
	bool isKeyByValue();
	bool isKeyByRef();

	bool isByValue();
	bool isByObserverPtr();
	bool isByUniquePtr();
	bool isBySharedPtr();
	bool isByPtr();
	bool isByRef();

	void setGetter(GetterSetterNode* getter);
	void setSetter(GetterSetterNode* setter);
	void setCandidate();

	virtual void checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments);
	virtual void checkSemantic(TemplateArguments* templateArguments);
};
