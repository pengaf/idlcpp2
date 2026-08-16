#pragma once
#include <stdio.h>

struct TokenNode;
struct IdentifierNode;
struct ScopeNode;
struct TypeNameNode;
struct CompoundTypeNode;
struct VariableNode;
struct VariableListNode;


void generateCode_outputEmbededCodes(FILE* file, TokenNode* tokenNode);
void generateCode_Token(FILE* file, TokenNode* tokenNode, int indentation, bool outputEmbededCode);
void generateCode_Identifier(FILE* file, IdentifierNode* identifierNode, int indentation, bool outputEmbededCode);
void generateCode_Identifier(FILE* file, const char* str, int indentation);
void generateCode_TypeName(FILE* file, TypeNameNode* typeNameNode, ScopeNode* scopeNode, bool addKeyword, int indentation, bool outputEmbededCode);
void generateCode_CompoundType(FILE* file, CompoundTypeNode* compoundType, ScopeNode* scopeNode, int indentation, bool outputEmbededCode);
void generateCode_ResultType(FILE* file, CompoundTypeNode* resultType, TokenNode* byRef, ScopeNode* scopeNode, bool constant, int indentation, bool outputEmbededCode);
void generateCode_Parameter(FILE* file, VariableNode* parameter, bool output, ScopeNode* scopeNode, bool outputEmbededCode);

