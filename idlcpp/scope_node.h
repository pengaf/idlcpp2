#pragma once

#include "member_node.h"

struct MemberListNode;

struct ScopeNode : MemberNode
{
	TokenNode* m_leftBrace{ nullptr };
	MemberListNode* m_memberList{ nullptr };
	TokenNode* m_rightBrace{ nullptr };
};
