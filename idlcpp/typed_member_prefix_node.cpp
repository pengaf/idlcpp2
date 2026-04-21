#include "typed_member_prefix_node.h"
#include "variable_type_list_node.h"
#include "identify_node.h"

TypedMemberPrefixNode::TypedMemberPrefixNode(VariableTypeListNode* variableTypeList, IdentifyNode* name)
{
	m_nodeType = snt_typed_member_prefix;
	m_variableTypeList = variableTypeList;
	m_name = name;
}
