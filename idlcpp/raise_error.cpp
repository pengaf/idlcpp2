#include "raise_error.h"
#include "error_list.h"
#include "identifier_node.h"
#include "type_name_node.h"
#include "method_node.h"
#include "property_node.h"
#include "property_accessor_node.h"
#include "field_node.h"
#include "scope_name_list_node.h"
#include "scope_name_node.h"
#include "utility.h"
#include <assert.h>

const size_t error_info_buffer_size = 512;

void RaiseError_NestedTemplateClass(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template class can not as a nested type", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_nested_template_class, buf);
}

//void RaiseError_InvalidTypeName(IdentifierNode* node)
//{
//	char buf[error_info_buffer_size];
//	sprintf_s(buf, "\'%s\' : is not a type name", node->m_str.c_str());
//	ErrorList_AddItem_CurrentFile(node->m_lineNo,
//		node->m_columnNo, semantic_error_invalid_type_name, buf);
//}
//
//void RaiseError_InvalidTypeName(TypeNameNode* node)
//{
//	char buf[error_info_buffer_size];
//	std::string str;
//	node->getString(str);
//	IdentifierNode* identifierNode = node->m_scopeNameList->m_scopeName->m_name;
//	sprintf_s(buf, "\'%s\' : is not a type name", str.c_str());
//	ErrorList_AddItem_CurrentFile(identifierNode->m_lineNo,
//		identifierNode->m_columnNo, semantic_error_invalid_type_name, buf);
//}

void RaiseError_InvalidTypeName(ScopeNameListNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getString(str);
	IdentifierNode* identifierNode = node->m_scopeName->m_name;
	sprintf_s(buf, "\'%s\' : is not a type name", str.c_str());
	ErrorList_AddItem_CurrentFile(identifierNode->m_lineNo,
		identifierNode->m_columnNo, semantic_error_invalid_type_name, buf);
}
//
//void RaiseError_InvalidParameterType(ParameterNode* node)
//{
//	TypeNameNode* typeName = node->m_typeName;
//	TokenNode* out = node->m_out;
//	TokenNode* typeCompound = node->m_typeCompound;
//	TokenNode* byRef = node->m_byRef;
//
//	char buf[error_info_buffer_size];
//	std::string str;
//	typeName->getString(str);
//
//	const char* strOut = "";
//	if (out)
//	{
//		switch (out->m_nodeType)
//		{
//		case '+':
//			strOut = node->m_array ? "+[]" : "+";
//			break;
//		case '*':
//			strOut = "*";
//			break;
//		}
//	}
//	const char* strPassing = "";
//	if (typeCompound)
//	{
//		switch (typeCompound->m_nodeType)
//		{
//		case '*':
//			strPassing = "*";
//			break;
//		case '!':
//			strPassing = "!";
//			break;
//		case '^':
//			strPassing = "^";
//			break;
//		default:
//			assert(false);
//		}
//	}
//	else if (byRef)
//	{
//		switch (byRef->m_nodeType)
//		{
//		case '&':
//			strPassing = "&";
//			break;
//		default:
//			assert(false);
//		}
//	}
//	TokenNode* tokenNode = typeName->m_scopeNameList ? typeName->m_scopeNameList->m_scopeName->m_name : typeName->m_keyword;
//	sprintf_s(buf, "\'%s %s %s\' : is not a valid type as paramter", str.c_str(), strOut, strPassing);
//	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
//		tokenNode->m_columnNo, semantic_error_invalid_parameter, buf);
//}

void RaiseError_InvalidResultType(TypeNameNode* result, TokenNode* typeCompound, TokenNode* byRef, bool resultArray)
{
	char buf[error_info_buffer_size];
	std::string str;
	result->getString(str);

	const char* strPassing = "";
	if (typeCompound)
	{
		switch (typeCompound->m_nodeType)
		{
		case '*':
			strPassing = "*";
			break;
		case '!':
			strPassing = "!";
			break;
		case '^':
			strPassing = "^";
			break;
		}
	}
	else if (byRef)
	{
		switch (byRef->m_nodeType)
		{
		case '&':
			strPassing = "&";
			break;
		}
	}
	TokenNode* tokenNode = result->m_scopeNameList ? result->m_scopeNameList->m_scopeName->m_name : result->m_keyword;
	sprintf_s(buf, "\'%s %s\' : can not be a result type", str.c_str(), strPassing);
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_result, buf);

}
//
//void RaiseError_InvalidFieldType(FieldNode* node)
//{
//	char buf[error_info_buffer_size];
//	std::string str;
//	node->m_typeName->getString(str);
//
//	TokenNode* tokenNode = node->m_typeName->m_scopeNameList ? node->m_typeName->m_scopeNameList->m_scopeName->m_name : node->m_typeName->m_keyword;
//	sprintf_s(buf, "\'%s\' : can not be a field type", str.c_str());
//	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
//		tokenNode->m_columnNo, semantic_error_invalid_field, buf);
//}

void RaiseError_PropertyAccessorAlreadyExist(PropertyAccessorNode* node, PropertyAccessorNode* existNode)
{
	char buf[error_info_buffer_size];
	TokenNode* tokenNode = node->m_keyword;
	TokenNode* existTokenNode = existNode->m_keyword;
	sprintf_s(buf, "\'%s\' : already exist in line %d column %d", 
		KeywardTokenToString(tokenNode), existTokenNode->m_lineNo, existTokenNode->m_columnNo);
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_property, buf);
}

void RaiseError_PropertyEnumNotAllowPtr(PropertyAccessorNode* node)
{
	char buf[error_info_buffer_size];
	TokenNode* tokenNode = node->m_keyword;
	sprintf_s(buf, "\'%s\' : property enum not support pointer", KeywardTokenToString(tokenNode));
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_property, buf);
}

//void RaiseError_InvalidPropertyType(PropertyNode* node)
//{
//	TokenNode* typeCompound = node->m_typeCompound;
//	TokenNode* byRef = node->m_byRef;
//	char buf[error_info_buffer_size];
//	std::string str;
//	node->m_typeName->getString(str);
//
//	const char* strPassing = "";
//	if (typeCompound)
//	{
//		switch (typeCompound->m_nodeType)
//		{
//		case '*':
//			strPassing = "*";
//			break;
//		case '!':
//			strPassing = "!";
//			break;
//		case '^':
//			strPassing = "^";
//			break;
//		}
//	}
//	else if (byRef)
//	{
//		strPassing = "&";
//	}
//	TokenNode* tokenNode = node->m_typeName->m_scopeNameList ? node->m_typeName->m_scopeNameList->m_scopeName->m_name : node->m_typeName->m_keyword;
//	sprintf_s(buf, "\'%s %s\' : can not be a property type", str.c_str(), strPassing);
//	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
//		tokenNode->m_columnNo, semantic_error_invalid_property, buf);
//}

void RaiseError_InvalidClassTemplateName(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : is not a class template name", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_invalid_class_template_name, buf);
}

void RaiseError_InvalidClassTemplateName(TypeNameNode* node)
{
	char buf[error_info_buffer_size];
	std::string name;
	node->getString(name);
	IdentifierNode* identifierNode = node->m_scopeNameList->m_scopeName->m_name;
	sprintf_s(buf, "\'%s\' : is not a class template name", name.c_str());
	ErrorList_AddItem_CurrentFile(identifierNode->m_lineNo,
		identifierNode->m_columnNo, semantic_error_invalid_class_template_name, buf);
}

void RaiseError_TooManyTemplateArguments(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : too many template arguments", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_too_many_template_arguments, buf);
}

void RaiseError_TooFewTemplateArguments(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : too few template arguments", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_too_few_template_arguments, buf);
}

void RaiseError_InvalidTemplateArgument(TypeNameNode* node)
{
	char buf[error_info_buffer_size];
	std::string name;
	node->getString(name);
	sprintf_s(buf, "\'%s\' : is not a valid template argument", name.c_str());
	ErrorList_AddItem_CurrentFile(node->m_scopeNameList->m_scopeName->m_name->m_lineNo,
		node->m_scopeNameList->m_scopeName->m_name->m_columnNo, semantic_error_invalid_template_argument, buf);
}

void RaiseError_TemplateParameterRedefinition(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "redefinition of template argument \'%s\'", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_parameter_redefinition, buf);
}

void RaiseError_TemplateClassInstanceInvalidTypeName(const char* typeName, IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template class is not explicit instantition", typeName);
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_class_not_instantiton, buf);
}

void RaiseError_TemplateInterfaceNotSupported(IdentifierNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template interface is not supported", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_interface_not_supported, buf);
}

void RaiseError_InvalidBaseType(IdentifierNode* node, const char* kind, const char* baseTypeName)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : %s type must inherit from %s", node->m_str.c_str(), kind, baseTypeName);
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_invalid_base_type, buf);
}

//void RaiseError_InterfaceMethodIsNotVirtual(IdentifierNode* node)
//{
//	char buf[error_info_buffer_size];
//	sprintf_s(buf, "\'%s\' : override method must be virtual or abstract", node->m_str.c_str());
//	ErrorList_AddItem_CurrentFile(node->m_lineNo,
//		node->m_columnNo, semantic_error_override_method_must_be_virtual_or_abstract, buf);
//}
