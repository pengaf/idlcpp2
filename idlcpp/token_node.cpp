#include "token_node.h"

#include "utility.h"
#include "compiler.h"
#include <assert.h>

const char* g_keywordTokens[] = 
{
	"::",
	"bool",
	"char",
	"wchar_t",
	"short",
	"long",
	"int",
	"signed",
	"unsigned",
	"float",
	"double",
	"string_t",
	"namespace",
	"enum",
	"class",
	"struct",
	"interface",
	"template",
	"virtual",
	"static",
	"get",
	"set",
	"typedef",
	"typename",
	"export",
};

const char* KeywardTokenToString(TokenNode* tokenNode)
{
	assert(sizeof(g_keywordTokens) / sizeof(g_keywordTokens[0]) == snt_end_output - snt_begin_output + 1);
	assert(snt_begin_output <= tokenNode->m_nodeType && tokenNode->m_nodeType <= snt_end_output);
	return g_keywordTokens[tokenNode->m_nodeType - snt_begin_output];
}

//const char* KeywardTokenToCppString(TokenNode* tokenNode)
//{
//}

TokenNode::TokenNode(int nodeType, int tokenNo, int lineNo, int columnNo)
{
	m_nodeType = nodeType;
	m_tokenNo = tokenNo;
	m_lineNo = lineNo;
	m_columnNo = columnNo;
}

void TokenNode::outputEmbededCodes(FILE* file, bool addSpace)
{
	g_compiler.outputEmbededCodes(file, this);
	if(isNumAlpha_(GetLastWrittenChar()) && addSpace)
	{
		writeSpaceToFile(file);
	}
}
