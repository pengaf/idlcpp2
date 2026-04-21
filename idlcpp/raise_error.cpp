#include "raise_error.h"
#include "error_list.h"
#include "identify_node.h"
#include "type_name_node.h"
#include "parameter_node.h"
#include "method_node.h"
#include "delegate_node.h"
#include "operator_node.h"
#include "property_node.h"
#include "field_node.h"
#include "scope_name_list_node.h"
#include "scope_name_node.h"
#include "utility.h"
#include <assert.h>

const size_t error_info_buffer_size = 512;

void RaiseError_NestedTemplateClass(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template class can not as a nested type", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_nested_template_class, buf);
}

void RaiseError_InvalidTypeName(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : is not a type name", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_invalid_type_name, buf);
}

void RaiseError_InvalidTypeName(ScopeNameListNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getString(str);
	IdentifyNode* identifyNode = node->m_scopeName->m_name;
	sprintf_s(buf, "\'%s\' : is not a type name", str.c_str());
	ErrorList_AddItem_CurrentFile(identifyNode->m_lineNo,
		identifyNode->m_columnNo, semantic_error_invalid_type_name, buf);
}

void RaiseError_InvalidTypeName(TypeNameNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getString(str);
	IdentifyNode* identifyNode = node->m_scopeNameList->m_scopeName->m_name;
	sprintf_s(buf, "\'%s\' : is not a type name", str.c_str());
	ErrorList_AddItem_CurrentFile(identifyNode->m_lineNo,
		identifyNode->m_columnNo, semantic_error_invalid_type_name, buf);
}

void RaiseError_InvalidParameterType(ParameterNode* node)
{
	TypeNameNode* typeName = node->m_typeName;
	TokenNode* out = node->m_out;
	TokenNode* typeCompound = node->m_typeCompound;
	TokenNode* byRef = node->m_byRef;

	char buf[error_info_buffer_size];
	std::string str;
	typeName->getString(str);

	const char* strOut = "";
	if (out)
	{
		switch (out->m_nodeType)
		{
		case '+':
			strOut = node->m_array ? "+[]" : "+";
			break;
		case '*':
			strOut = "*";
			break;
		}
	}
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
		default:
			assert(false);
		}
	}
	else if (byRef)
	{
		switch (byRef->m_nodeType)
		{
		case '&':
			strPassing = "&";
			break;
		default:
			assert(false);
		}
	}
	TokenNode* tokenNode = typeName->m_scopeNameList ? typeName->m_scopeNameList->m_scopeName->m_name : typeName->m_keyword;
	sprintf_s(buf, "\'%s %s %s\' : is not a valid type as paramter", str.c_str(), strOut, strPassing);
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_parameter, buf);
}

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

void RaiseError_InvalidResultType(MethodNode* node)
{
	RaiseError_InvalidResultType(node->m_resultTypeName, node->m_typeCompound, node->m_byRef, node->m_resultArray);
}

void RaiseError_InvalidResultType(OperatorNode* node)
{
	RaiseError_InvalidResultType(node->m_resultTypeName, node->m_typeCompound, node->m_byRef, node->m_resultArray);
}

void RaiseError_InvalidResultType(DelegateNode* node)
{
	RaiseError_InvalidResultType(node->m_resultTypeName, node->m_typeCompound, node->m_byRef, node->m_resultArray);
}


void RaiseError_InvalidFieldType(FieldNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->m_typeName->getString(str);

	TokenNode* tokenNode = node->m_typeName->m_scopeNameList ? node->m_typeName->m_scopeNameList->m_scopeName->m_name : node->m_typeName->m_keyword;
	sprintf_s(buf, "\'%s\' : can not be a field type", str.c_str());
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_field, buf);
}

void RaiseError_InvalidPropertyType(PropertyNode* node)
{
	TokenNode* typeCompound = node->m_typeCompound;
	TokenNode* byRef = node->m_byRef;
	char buf[error_info_buffer_size];
	std::string str;
	node->m_typeName->getString(str);

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
		strPassing = "&";
	}
	TokenNode* tokenNode = node->m_typeName->m_scopeNameList ? node->m_typeName->m_scopeNameList->m_scopeName->m_name : node->m_typeName->m_keyword;
	sprintf_s(buf, "\'%s %s\' : can not be a property type", str.c_str(), strPassing);
	ErrorList_AddItem_CurrentFile(tokenNode->m_lineNo,
		tokenNode->m_columnNo, semantic_error_invalid_property, buf);
}

void RaiseError_InvalidClassTemplateName(IdentifyNode* node)
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
	IdentifyNode* identifyNode = node->m_scopeNameList->m_scopeName->m_name;
	sprintf_s(buf, "\'%s\' : is not a class template name", name.c_str());
	ErrorList_AddItem_CurrentFile(identifyNode->m_lineNo,
		identifyNode->m_columnNo, semantic_error_invalid_class_template_name, buf);
}

void RaiseError_TooManyTemplateArguments(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : too many template arguments", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_too_many_template_arguments, buf);
}

void RaiseError_TooFewTemplateArguments(IdentifyNode* node)
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

void RaiseError_TemplateParameterRedefinition(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "redefinition of template parameter \'%s\'", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_parameter_redefinition, buf);
}

void RaiseError_TemplateClassInstanceInvalidTypeName(const char* typeName, IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template class is not explicit instantition", typeName);
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_class_not_instantiton, buf);
}

void RaiseError_TemplateInterfaceNotSupported(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : template interface is not supported", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_template_interface_not_supported, buf);
}

void RaiseError_MissingRcObjectBaseType(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : rc object type must inherit from ::pafcore::Object", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_missing_rc_object_base_type, buf);
}

void RaiseError_InterfaceMethodIsNotVirtual(IdentifyNode* node)
{
	char buf[error_info_buffer_size];
	sprintf_s(buf, "\'%s\' : override method must be virtual or abstract", node->m_str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_lineNo,
		node->m_columnNo, semantic_error_override_method_must_be_virtual_or_abstract, buf);
}

void RaiseError_TooFewFormalParameters(OperatorNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getOperatorString(str);
	sprintf_s(buf, "\'operator %s\' : too few formal parameters", str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_sign->m_lineNo,
		node->m_sign->m_columnNo, semantic_error_too_few_formal_parameters, buf);
}

void RaiseError_TooManyFormalParameters(OperatorNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getOperatorString(str);
	sprintf_s(buf, "\'operator %s\' : too many formal parameters", str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_sign->m_lineNo,
		node->m_sign->m_columnNo, semantic_error_too_many_formal_parameters, buf);
}

void RaiseError_StaticOperator(OperatorNode* node)
{
	char buf[error_info_buffer_size];
	std::string str;
	node->getOperatorString(str);
	sprintf_s(buf, "\'operator %s\' : static operator overloading is not support", str.c_str());
	ErrorList_AddItem_CurrentFile(node->m_sign->m_lineNo,
		node->m_sign->m_columnNo, semantic_error_operator_can_not_be_static, buf);
}
