#include "identifier_node.h"


IdentifierNode::IdentifierNode(const char* str, int tokenNo, int lineNo, int columnNo)
	: TokenNode(snt_identifier, tokenNo, lineNo, columnNo), m_str(str)
{}


IdentifierNode::IdentifierNode(const char* str, size_t len, int tokenNo, int lineNo, int columnNo)
	: TokenNode(snt_identifier, tokenNo, lineNo, columnNo), m_str(str, len)
{}
