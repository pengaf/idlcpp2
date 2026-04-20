#pragma once

#include "member_node.h"

struct MemberListNode;

struct ScopeNode : MemberNode
{
	MemberListNode* m_memberList;
public:
	ScopeNode()
	{
		m_memberList = 0;
	}
};
