#include "code_generator.h"
#include "utility.h"
#include "token_node.h"
#include "identifier_node.h"
#include "type_name_node.h"
#include "scope_node.h"
#include "scope_name_list_node.h"
#include "scope_name_node.h"
#include "variable_node.h"
#include "compound_type_node.h"
#include "compiler.h"

struct TokenNode;
struct IdentifierNode;
struct TypeNameNode;
struct ScopeNode;
struct VariableNode;

void generateCode_outputEmbededCodes(FILE* file, TokenNode* tokenNode)
{
	g_compiler.outputEmbededCodes(file, tokenNode);
}

void generateCode_Token(FILE* file, TokenNode* tokenNode, int indentation, bool outputEmbededCode)
{
	if (outputEmbededCode)
	{
		g_compiler.outputEmbededCodes(file, tokenNode);
	}
	if (indentation > 0)
	{
		writeStringToFile("", 0, file, indentation);
	}
	if (tokenNode->m_nodeType < 256)
	{
		char ch = tokenNode->m_nodeType;
		writeStringToFile(&ch, 1, file);
	}
	else
	{
		if (isNumAlpha_(GetLastWrittenChar()))
		{
			writeSpaceToFile(file);
		}
		const char* str = KeywardTokenToString(tokenNode);
		writeStringToFile(str, file);
	}
}

void generateCode_Identifier(FILE* file, IdentifierNode* identifierNode, int indentation, bool outputEmbededCode)
{
	if (outputEmbededCode)
	{
		g_compiler.outputEmbededCodes(file, identifierNode);
	}
	if (indentation > 0)
	{
		writeStringToFile("", 0, file, indentation);
	}
	if (isNumAlpha_(GetLastWrittenChar()))
	{
		writeSpaceToFile(file);
	}
	writeStringToFile(identifierNode->m_str.c_str(), identifierNode->m_str.length(), file);
};

void generateCode_Identifier(FILE* file, const char* str, int indentation)
{
	if (indentation > 0)
	{
		writeStringToFile("", 0, file, indentation);
	}
	if (isNumAlpha_(GetLastWrittenChar()))
	{
		writeSpaceToFile(file);
	}
	writeStringToFile(str, file);
}

void generateCode_TypeName(FILE* file, TypeNameNode* typeNameNode, ScopeNode* scopeNode, bool addKeyword, int indentation, bool outputEmbededCode)
{
	if (outputEmbededCode)
	{
		if (typeNameNode->m_keyword)
		{
			typeNameNode->m_keyword->outputEmbededCodes(file, 0 == indentation);
		}
		if (typeNameNode->m_scopeNameList)
		{
			typeNameNode->m_scopeNameList->m_scopeName->m_name->outputEmbededCodes(file, 0 == indentation);
		}
	}
	if (addKeyword && typeNameNode->underTemplateParameter())
	{
		writeStringToFile("typename ", file, indentation);
		indentation = 0;
	}
	std::string typeName;
	typeNameNode->getRelativeName(typeName, scopeNode);
	writeStringToFile(typeName.c_str(), file, indentation);
}

void generateCode_CompoundType(FILE* file, CompoundTypeNode* compoundType, ScopeNode* scopeNode, int indentation, bool outputEmbededCode)
{
	if (outputEmbededCode)
	{
		TypeNameNode* typeNameNode = compoundType->m_typeName;
		if (typeNameNode->m_keyword)
		{
			typeNameNode->m_keyword->outputEmbededCodes(file, 0 == indentation);
		}
		if (typeNameNode->m_scopeNameList)
		{
			typeNameNode->m_scopeNameList->m_scopeName->m_name->outputEmbededCodes(file, 0 == indentation);
		}
	}
	const char* wrapperName = 0;
	if (compoundType->isObserverPtr())
	{
		wrapperName = "::pafcore::ObserverPtr";
	}
	else if (compoundType->isSharedPtr())
	{
		wrapperName = "::pafcore::SharedPtr";
	}
	else if (compoundType->isObserverArray())
	{
		wrapperName = "::pafcore::ObserverArray";
	}
	else if (compoundType->isSharedArray())
	{
		wrapperName = "::pafcore::SharedArray";
	}

	if (wrapperName)
	{
		writeStringToFile(wrapperName, file, indentation);
		writeStringToFile("<", file);
		generateCode_TypeName(file, compoundType->m_typeName, scopeNode, true, 0, outputEmbededCode);
		writeStringToFile("> ", file);
	}
	else
	{
		generateCode_TypeName(file, compoundType->m_typeName, scopeNode, true, indentation, outputEmbededCode);
		if(compoundType->isRawPtr())
		{
			writeStringToFile("*", file);
		}
	}
}

void generateCode_ResultType(FILE* file, CompoundTypeNode* resultType, TokenNode* byRef, ScopeNode* scopeNode, bool constant, int indentation, bool outputEmbededCode)
{
	generateCode_CompoundType(file, resultType, scopeNode, indentation, outputEmbededCode);
	if (byRef)
	{
		if (constant)
		{
			writeStringToFile(" const", file);
		}
		generateCode_Token(file, byRef, 0, outputEmbededCode);
	}
	writeSpaceToFile(file);
}

void generateCode_Parameter(FILE* file, VariableNode* parameter, bool output, ScopeNode* scopeNode, bool outputEmbededCode)
{
	generateCode_CompoundType(file, parameter->m_compoundType, scopeNode, 0, outputEmbededCode);
	if (output)
	{
		if (parameter->isByRef())
		{
			generateCode_Token(file, parameter->m_byRef, 0, outputEmbededCode);
		}
		else
		{
			writeStringToFile("&", file);
		}
	}
	else
	{
		if (parameter->isByRef())
		{
			writeStringToFile(" const", file);
			generateCode_Token(file, parameter->m_byRef, 0, outputEmbededCode);
		}
	}
	writeSpaceToFile(file);
	generateCode_Identifier(file, parameter->m_name, 0, outputEmbededCode);
};


