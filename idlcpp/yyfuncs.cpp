#include "yyfuncs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <set>
#include <string>
#include <algorithm>
#include <cstring>
#include "error_list.h"
#include "source_file.h"

#include "token_node.h"
#include "identifier_node.h"
#include "identifier_list_node.h"
#include "attribute_node.h"
#include "attribute_list_node.h"
#include "scope_name_node.h"
#include "scope_name_list_node.h"
#include "enumerator_node.h"
#include "enumerator_list_node.h"
#include "type_name_node.h"
#include "type_name_list_node.h"
#include "compound_type_node.h"
#include "variable_node.h"
#include "variable_list_node.h"
#include "field_node.h"
#include "property_accessor_node.h"
#include "property_accessor_list_node.h"
#include "property_node.h"
#include "method_node.h"
#include "member_list_node.h"
#include "class_node.h"
#include "enum_node.h"
#include "typedef_node.h"
#include "type_declaration_node.h"
#include "template_parameters_node.h"
#include "template_class_instance_node.h"
#include "namespace_node.h"
#include "program_node.h"
#include "compiler.h"

extern int yylineno;
extern int yycolumnno;
extern int yytokenno;
extern int yyHasListProperty;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
void yy_switch_to_buffer( YY_BUFFER_STATE new_buffer );
void yy_delete_buffer( YY_BUFFER_STATE b );
#define YY_BUF_SIZE 16384

std::vector<SyntaxNodeImpl*> g_syntaxNodes;

SyntaxNode* newToken(int nodeType)
{
	TokenNode* res = new TokenNode(nodeType, yytokenno, yylineno, yycolumnno);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newIdentifier(const char* str)
{
	IdentifierNode* res = new IdentifierNode(str, yytokenno, yylineno, yycolumnno);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newString(const char* str)
{
	IdentifierNode* res = new IdentifierNode(str, strlen(str) - 1, yytokenno, yylineno, yycolumnno);
	g_syntaxNodes.push_back(res);
	return res;
}

void newCodeBlock(const char* str)
{
	g_compiler.m_currentSourceFile->addEmbededCodeBlock(str, yytokenno);
}

SyntaxNode* newPrimitiveType(SyntaxNode* keyword, PredefinedType type)
{
	assert(snt_keyword_begin_primitive <= keyword->m_nodeType && keyword->m_nodeType <= snt_keyword_end_primitive);
	TypeNameNode* res = new TypeNameNode((TokenNode*)keyword, type);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newIdentifierList(SyntaxNode* identifierList, SyntaxNode* comma, SyntaxNode* identifier)
{
	assert(0 == identifierList || snt_identifier_list == identifierList->m_nodeType);
	assert(0 == comma || ',' == comma->m_nodeType);
	assert((0 == comma && 0 == identifierList) || (0 != comma && 0 != identifierList));
	assert(snt_identifier == identifier->m_nodeType);
	IdentifierListNode* res = new IdentifierListNode((IdentifierListNode*)identifierList, (TokenNode*)comma, (IdentifierNode*)identifier);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newAttribute(SyntaxNode* name, SyntaxNode* content, int u8content)
{
	assert(0 != name && snt_identifier == name->m_nodeType);
	assert(0 != content && snt_identifier == content->m_nodeType);
	AttributeNode* res = new AttributeNode((IdentifierNode*)name, (IdentifierNode*)content, u8content);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newAttributeList(SyntaxNode* attributeList, SyntaxNode* attribute)
{
	assert(0 == attributeList || snt_attribute_list == attributeList->m_nodeType);
	assert(0 != attribute && snt_attribute == attribute->m_nodeType);
	AttributeListNode* res = new AttributeListNode((AttributeListNode*)attributeList, (AttributeNode*)attribute);
	g_syntaxNodes.push_back(res);
	return res;
}

void setEntityAttributeListOpt(SyntaxNode* entity, SyntaxNode* attributeList)
{
	if (attributeList)
	{
		assert(0 != entity && 0 != attributeList);
		assert(snt_field == entity->m_nodeType
			|| snt_property == entity->m_nodeType
			|| snt_method == entity->m_nodeType
			|| snt_class == entity->m_nodeType
			|| snt_enumerator == entity->m_nodeType
			|| snt_enum == entity->m_nodeType
			|| snt_template_class_instance == entity->m_nodeType
			|| snt_typedef == entity->m_nodeType
			|| snt_type_declaration == entity->m_nodeType
			|| snt_namespace == entity->m_nodeType);
		assert(snt_attribute_list == attributeList->m_nodeType);
		((EntityNode*)entity)->m_attributeList = (AttributeListNode*)attributeList;
	}
}

SyntaxNode* newScopeName(SyntaxNode* identifier, SyntaxNode* lts, SyntaxNode* parameterList, SyntaxNode* gts)
{
	assert(0 != identifier && snt_identifier == identifier->m_nodeType);
	assert(0 == lts || '<' == lts->m_nodeType);
	assert(0 == parameterList || snt_type_name_list == parameterList->m_nodeType);
	assert(0 == gts || '>' == gts->m_nodeType);
	ScopeNameNode* res = new ScopeNameNode((IdentifierNode*)identifier,
		(TokenNode*)lts, (TypeNameListNode*)parameterList, (TokenNode*)gts);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newScopeNameList(SyntaxNode* scopeNameList, SyntaxNode* scopeName)
{
	assert(0 == scopeNameList || snt_scope_name_list == scopeNameList->m_nodeType);
	assert(snt_scope_name == scopeName->m_nodeType);
	ScopeNameListNode* res = new ScopeNameListNode((ScopeNameListNode*)scopeNameList, (ScopeNameNode*)scopeName);
	g_syntaxNodes.push_back(res);
	return res;
}

void setScopeNameListGlobal(SyntaxNode* scopeNameList)
{
	assert(snt_scope_name_list == scopeNameList->m_nodeType);
	static_cast<ScopeNameListNode*>(scopeNameList)->m_global = true;
}

SyntaxNode* newTypeName(SyntaxNode* scopeNameList)
{
	assert(0 != scopeNameList && snt_scope_name_list == scopeNameList->m_nodeType);
	TypeNameNode* res = new TypeNameNode((ScopeNameListNode*)scopeNameList);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newTypeNameList(SyntaxNode* typeNameList, SyntaxNode* comma, SyntaxNode* typeName)
{
	assert(0 == typeNameList || snt_type_name_list == typeNameList->m_nodeType);
	assert(0 == comma || ',' == comma->m_nodeType);
	assert((0 == comma && 0 == typeNameList) || (0 != comma && 0 != typeNameList));	
	assert(snt_type_name == typeName->m_nodeType);
	TypeNameListNode* res = new TypeNameListNode((TypeNameListNode*)typeNameList, (TokenNode*)comma, (TypeNameNode*)typeName);
	g_syntaxNodes.push_back(res);
	return res;
}

void setTypeNameNoCode(SyntaxNode* syntaxNode)
{
	((TypeNameNode*)syntaxNode)->m_noCode = true;
}

void setTypeNameNoMeta(SyntaxNode* syntaxNode)
{
	((TypeNameNode*)syntaxNode)->m_noMeta = true;
}

SyntaxNode* newCompoundType(SyntaxNode* typeName, TypeCompound typeCompound)
{
	assert(snt_type_name == typeName->m_nodeType);
	CompoundTypeNode* res = new CompoundTypeNode((TypeNameNode*)typeName, typeCompound);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newTypedef(SyntaxNode* keyword, SyntaxNode* typeName, SyntaxNode* name)
{
	assert(snt_keyword_typedef == keyword->m_nodeType);
	assert(snt_type_name == typeName->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	TypedefNode* res;
	res = new TypedefNode((TokenNode*)keyword, (TypeNameNode*)typeName, (IdentifierNode*)name);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newTypeDeclaration(SyntaxNode* name, TypeKind typeKind)
{
	assert(snt_identifier == name->m_nodeType);
	TypeDeclarationNode* res;
	res = new TypeDeclarationNode((IdentifierNode*)name, typeKind);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newEnumerator(SyntaxNode* name, SyntaxNode* equalSign)
{
	assert(snt_identifier == name->m_nodeType);
	assert(0 == equalSign || '=' == equalSign->m_nodeType);
	EnumeratorNode* res = new EnumeratorNode((IdentifierNode*)name, (TokenNode*)equalSign);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newEnumeratorList(SyntaxNode* enumeratorList, SyntaxNode* comma, SyntaxNode* enum_member)
{
	assert(0 == enumeratorList || snt_enumerator_list == enumeratorList->m_nodeType);
	assert(0 == comma || ',' == comma->m_nodeType);
	assert((0 == comma && 0 == enumeratorList) || (0 != comma && 0 != enumeratorList));
	assert(snt_enumerator == enum_member->m_nodeType);
	EnumeratorListNode* res = new EnumeratorListNode((EnumeratorListNode*)enumeratorList, (TokenNode*)comma, (EnumeratorNode*)enum_member);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newEnum(SyntaxNode* keyword, SyntaxNode* keyword2, SyntaxNode* name, SyntaxNode* leftBrace, SyntaxNode* enumeratorList, SyntaxNode* rightBrace)
{
	assert(snt_keyword_enum == keyword->m_nodeType);
	assert(0 == keyword2 || snt_keyword_class == keyword2->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert('{' == leftBrace->m_nodeType);
	assert(0 == enumeratorList || snt_enumerator_list == enumeratorList->m_nodeType);
	assert('}' == rightBrace->m_nodeType);
	EnumNode* res = new EnumNode((TokenNode*)keyword, (TokenNode*)keyword2, (IdentifierNode*)name, (TokenNode*)leftBrace, (EnumeratorListNode*)enumeratorList, (TokenNode*)rightBrace);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newField(SyntaxNode* compoundType, SyntaxNode* name, SyntaxNode* leftBracket, SyntaxNode* rightBracket)
{
	assert(snt_compound_type == compoundType->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert((0 == leftBracket || '[' == leftBracket->m_nodeType) && (0 == rightBracket || ']' == rightBracket->m_nodeType));
	FieldNode* res = new FieldNode((CompoundTypeNode*)compoundType, (IdentifierNode*)name, (TokenNode*)leftBracket, (TokenNode*)rightBracket);
	g_syntaxNodes.push_back(res);
	return res;
}

void setFieldModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier)
{
	assert(snt_field == syntaxNode->m_nodeType);
	assert(snt_keyword_static == modifier->m_nodeType);
	FieldNode* fieldNode = (FieldNode*)syntaxNode;
	fieldNode->m_modifier = (TokenNode*)modifier;
}

SyntaxNode* newPropertyAccessor(SyntaxNode* keyword, SyntaxNode* byRef, SyntaxNode* nativeName)
{
	assert(snt_keyword_get == keyword->m_nodeType || snt_keyword_set == keyword->m_nodeType || snt_keyword_enum == keyword->m_nodeType);
	assert(nullptr == byRef || '&' == byRef->m_nodeType);
	assert(nullptr == nativeName || snt_identifier == nativeName->m_nodeType);
	PropertyAccessorNode* res = new PropertyAccessorNode((TokenNode*)keyword, (TokenNode*)byRef, (IdentifierNode*)nativeName);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newPropertyAccessorList(SyntaxNode* propertyAccessorList, SyntaxNode* propertyAccessor)
{
	assert(0 == propertyAccessorList || snt_property_accessor_list == propertyAccessorList->m_nodeType);
	assert(0 != propertyAccessor && snt_property_accessor == propertyAccessor->m_nodeType);
	PropertyAccessorListNode* res = new PropertyAccessorListNode((PropertyAccessorListNode*)propertyAccessorList, (PropertyAccessorNode*)propertyAccessor);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newProperty(SyntaxNode* compoundType, SyntaxNode* name, SyntaxNode* accessorList, PropertyKind propertyKind)
{
	assert(snt_compound_type == compoundType->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert(snt_property_accessor_list == accessorList->m_nodeType);
	PropertyNode* res = new PropertyNode((CompoundTypeNode*)compoundType, (IdentifierNode*)name, (PropertyAccessorListNode*)accessorList, propertyKind);
	g_syntaxNodes.push_back(res);

	if (list_property == propertyKind)
	{
		yyHasListProperty = 1;
	}
	return res;
}

void setPropertyModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier)
{
	assert(snt_property == syntaxNode->m_nodeType);
	assert(snt_keyword_static == modifier->m_nodeType);
	PropertyNode* propertyNode = (PropertyNode*)syntaxNode;
	propertyNode->m_modifier = (TokenNode*)modifier;
}

SyntaxNode* newVariable(SyntaxNode* compoundType, SyntaxNode* byRef, SyntaxNode* name, SyntaxNode* allowNull)
{
	assert(snt_compound_type == compoundType->m_nodeType && snt_identifier == name->m_nodeType);
	assert(nullptr == byRef || '&' == byRef->m_nodeType);
	assert(nullptr == allowNull || '?' == allowNull->m_nodeType);
	VariableNode* res = new VariableNode((CompoundTypeNode*)compoundType, (TokenNode*)byRef, (IdentifierNode*)name, nullptr != allowNull);
	g_syntaxNodes.push_back(res);
	return res;
}

void setVariableAllowNull(SyntaxNode* variable)
{
	assert(snt_variable == variable->m_nodeType);
	VariableNode* variableNode = (VariableNode*)variable;
	variableNode->m_allowNull = true;
}

SyntaxNode* newVariableList(SyntaxNode* variableList, SyntaxNode* comma, SyntaxNode* variable)
{
	assert(0 == variableList || snt_variable_list == variableList->m_nodeType);
	assert(0 == comma || ',' == comma->m_nodeType);
	assert(snt_variable == variable->m_nodeType);
	VariableListNode* res = new VariableListNode((VariableListNode*)variableList, (TokenNode*)comma, (VariableNode*)variable);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newMethod(SyntaxNode* resultList, SyntaxNode* name, SyntaxNode* leftParenthesis, SyntaxNode* parameterList, SyntaxNode* rightParenthesis)
{
	assert(nullptr == resultList || snt_variable_list == resultList->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert('(' == leftParenthesis->m_nodeType && ')' == rightParenthesis->m_nodeType);
	assert(nullptr == parameterList || snt_variable_list == parameterList->m_nodeType);
	MethodNode* res = new MethodNode((VariableListNode*)resultList, (IdentifierNode*)name, (TokenNode*)leftParenthesis, (VariableListNode*)parameterList, (TokenNode*)rightParenthesis);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newMethod2(SyntaxNode* compoundType, SyntaxNode* byRefOpt, SyntaxNode* name, SyntaxNode* leftParenthesis, SyntaxNode* parameterList, SyntaxNode* rightParenthesis)
{
	assert(snt_compound_type == compoundType->m_nodeType);
	assert(nullptr == byRefOpt || '&' == byRefOpt->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert('(' == leftParenthesis->m_nodeType && ')' == rightParenthesis->m_nodeType);
	assert(nullptr == parameterList || snt_variable_list == parameterList->m_nodeType);
	IdentifierNode* resName = (IdentifierNode*)newIdentifier("__res__");
	VariableNode* variable = (VariableNode*)newVariable(compoundType, byRefOpt, resName, nullptr);
	VariableListNode* resultList = (VariableListNode*)newVariableList(nullptr, nullptr, variable);
	MethodNode* res = new MethodNode((VariableListNode*)resultList, (IdentifierNode*)name, (TokenNode*)leftParenthesis, (VariableListNode*)parameterList, (TokenNode*)rightParenthesis);
	g_syntaxNodes.push_back(res);
	return res;
}

void setMethodModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier)
{
	assert(snt_method == syntaxNode->m_nodeType);
	assert(snt_keyword_static == modifier->m_nodeType || snt_keyword_virtual == modifier->m_nodeType);
	MethodNode* methodNode = (MethodNode*)syntaxNode;
	methodNode->m_modifier = (TokenNode*)modifier;
}

void setMemberFilter(SyntaxNode* syntaxNode, SyntaxNode* filter)
{
	assert(snt_field == syntaxNode->m_nodeType
		|| snt_property == syntaxNode->m_nodeType
		|| snt_method == syntaxNode->m_nodeType
		|| snt_class == syntaxNode->m_nodeType
		|| snt_enum == syntaxNode->m_nodeType
		|| snt_typedef == syntaxNode->m_nodeType
		|| snt_type_declaration == syntaxNode->m_nodeType
		|| snt_template_class_instance == syntaxNode->m_nodeType
		|| snt_namespace == syntaxNode->m_nodeType);
	assert(snt_keyword_nometa == filter->m_nodeType || snt_keyword_nocode == filter->m_nodeType);	
	((MemberNode*)syntaxNode)->m_filter = (TokenNode*)filter;
}

void setMemberNativeNameOpt(SyntaxNode* member, SyntaxNode* nativeName)
{
	if (nativeName)
	{
		assert(snt_field == member->m_nodeType
			|| snt_property == member->m_nodeType
			|| snt_method == member->m_nodeType
			|| snt_class == member->m_nodeType
			|| snt_enum == member->m_nodeType
			|| snt_typedef == member->m_nodeType
			|| snt_type_declaration == member->m_nodeType
			|| snt_template_class_instance == member->m_nodeType
			|| snt_namespace == member->m_nodeType);
		assert(snt_identifier == nativeName->m_nodeType);
		static_cast<MemberNode*>(member)->m_nativeName = static_cast<IdentifierNode*>(nativeName);
	}
}

void setMemberSemicolon(SyntaxNode* member, SyntaxNode* semicolon)
{
	assert(snt_field == member->m_nodeType
		|| snt_property == member->m_nodeType
		|| snt_method == member->m_nodeType
		|| snt_class == member->m_nodeType
		|| snt_enum == member->m_nodeType
		|| snt_typedef == member->m_nodeType
		|| snt_type_declaration == member->m_nodeType
		|| snt_template_class_instance == member->m_nodeType);
	assert(';' == semicolon->m_nodeType);
	static_cast<MemberNode*>(member)->m_semicolon = static_cast<TokenNode*>(semicolon);
}

SyntaxNode* newMemberList(SyntaxNode* memberList, SyntaxNode* member)
{
	assert(0 == memberList || snt_member_list == memberList->m_nodeType);
	assert(snt_field == member->m_nodeType 
		|| snt_property == member->m_nodeType 
		|| snt_method == member->m_nodeType 
		|| snt_class == member->m_nodeType
		|| snt_enum == member->m_nodeType
		|| snt_typedef == member->m_nodeType 
		|| snt_type_declaration == member->m_nodeType
		|| snt_template_class_instance == member->m_nodeType
		|| snt_namespace == member->m_nodeType);
	MemberListNode* res = new MemberListNode((MemberListNode*)memberList, (MemberNode*)member);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newTemplateParameters(SyntaxNode* keyword, SyntaxNode* lts, SyntaxNode* parameterList, SyntaxNode* gts)
{
	assert(snt_keyword_template == keyword->m_nodeType);
	assert('<' == lts->m_nodeType);
	assert(snt_identifier_list == parameterList->m_nodeType);
	assert('>' == gts->m_nodeType);
	TemplateParametersNode* res = new TemplateParametersNode((TokenNode*)keyword,
		(TokenNode*)lts, (IdentifierListNode*)parameterList, (TokenNode*)gts);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newClass(SyntaxNode* keyword, SyntaxNode* conceptList, SyntaxNode* name)
{
	assert(snt_keyword_class == keyword->m_nodeType || snt_keyword_struct == keyword->m_nodeType || snt_keyword_interface == keyword->m_nodeType);
	assert(0 == conceptList || snt_identifier_list == conceptList->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	ClassNode* res = new ClassNode((TokenNode*)keyword, (IdentifierListNode*)conceptList, (IdentifierNode*)name);
	g_syntaxNodes.push_back(res);
	return res;	
}

void setClassBaseList(SyntaxNode* cls, SyntaxNode* colon, SyntaxNode* baseList)
{
	assert(snt_class == cls->m_nodeType);
	assert(0 == colon || ':' == colon->m_nodeType);
	assert(0 == baseList || snt_type_name_list == baseList->m_nodeType);
	((ClassNode*)cls)->m_colon = (TokenNode*)colon;
	((ClassNode*)cls)->m_baseList = (TypeNameListNode*)baseList;
}

void setClassMemberList(SyntaxNode* cls, SyntaxNode* leftBrace, SyntaxNode* memberList, SyntaxNode* rightBrace)
{
	assert(snt_class == cls->m_nodeType);
	assert('{' == leftBrace->m_nodeType);	
	assert(0 == memberList || snt_member_list == memberList->m_nodeType);
	assert('}' == rightBrace->m_nodeType);
	((ClassNode*)cls)->setMemberList((TokenNode*)leftBrace, (MemberListNode*)memberList, (TokenNode*)rightBrace);
}

void setClassTemplateParameters(SyntaxNode* cls, SyntaxNode* parameters)
{
	assert(snt_class == cls->m_nodeType);
	assert(snt_template_parameters == parameters->m_nodeType);
	((ClassNode*)cls)->setTemplateParameters((TemplateParametersNode*)parameters);
}

SyntaxNode* newTemplateClassInstance(SyntaxNode* name, SyntaxNode* typeNameList)
{
	assert(snt_identifier == name->m_nodeType);
	assert(snt_type_name_list == typeNameList->m_nodeType);
	TemplateClassInstanceNode* res = new TemplateClassInstanceNode((IdentifierNode*)name, (TypeNameListNode*)typeNameList);
	g_syntaxNodes.push_back(res);
	return res;
}

void setTemplateClassInstanceReservedMemberList(SyntaxNode* tci, SyntaxNode* identifierList)
{
	assert(snt_template_class_instance == tci->m_nodeType && snt_identifier_list == identifierList->m_nodeType);
	((TemplateClassInstanceNode*)tci)->m_reservedMemberList = (IdentifierListNode*)identifierList;
}

SyntaxNode* newNamespace(SyntaxNode* keyword, SyntaxNode* name, SyntaxNode* leftBrace, SyntaxNode* memberList, SyntaxNode* rightBrace)
{
	assert(snt_keyword_namespace == keyword->m_nodeType);
	assert(snt_identifier == name->m_nodeType);
	assert('{' == leftBrace->m_nodeType);
	assert(0 == memberList || snt_member_list == memberList->m_nodeType);
	assert('}' == rightBrace->m_nodeType);
	NamespaceNode* res = new NamespaceNode((TokenNode*)keyword, (IdentifierNode*)name, (TokenNode*)leftBrace, (MemberListNode*)memberList, (TokenNode*)rightBrace);
	g_syntaxNodes.push_back(res);
	return res;
}

SyntaxNode* newProgram(SyntaxNode* memberList)
{
	assert(0 == memberList || snt_member_list == memberList->m_nodeType);
	ProgramNode* res = new ProgramNode((MemberListNode*)memberList);
	g_syntaxNodes.push_back(res);
	return res;
}

void invalidString(char* s)
{
	ErrorList_AddItem_CurrentFile(syntax_error_invalid_string, s);
}

void unterminatedCode()
{
	ErrorList_AddItem_CurrentFile(syntax_error_unterminated_code, "unterminated cpp code block, cannot find \"*$\"");
}

void unterminatedComment()
{
	ErrorList_AddItem_CurrentFile(syntax_error_unterminated_comment, "unterminated comment block, cannot find \"*/\"");
}

void attachSyntaxTree(SyntaxNode* tree)
{
	assert(snt_namespace == tree->m_nodeType);
	g_compiler.attachSyntaxTree((ProgramNode*)tree);
}

void freetree()
{
	size_t count = g_syntaxNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		delete g_syntaxNodes[i];
	}
	g_syntaxNodes.clear();
}

void yyerror(const char* s)
{
	ErrorList_AddItem_CurrentFile(syntax_error_internal, s);
}
