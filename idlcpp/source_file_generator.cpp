#include "source_file_generator.h"
#include "utility.h"
#include "source_file.h"
#include "program_node.h"
#include "namespace_node.h"
#include "token_node.h"
#include "identifier_node.h"
#include "enumerator_list_node.h"
#include "scope_name_list_node.h"
#include "member_list_node.h"
#include "enum_node.h"
#include "class_node.h"
#include "template_parameters_node.h"
#include "type_name_list_node.h"
#include "type_name_node.h"
#include "field_node.h"
#include "property_node.h"
#include "method_node.h"
#include "variable_list_node.h"
#include "variable_node.h"
#include "compound_type_node.h"
#include "typedef_node.h"
#include "options.h"
#include "platform.h"
#include "common_funcs.h"
#include "type_tree.h"
#include "code_generator.h"
#include <assert.h>

void SourceFileGenerator::generateCode(FILE* dstFile, SourceFile* sourceFile, const char* fullPathName, const char* baseName)
{
	generateCode_Program(dstFile, sourceFile->m_syntaxTree, fullPathName, baseName);
}

void SourceFileGenerator::generateCode_Program(FILE* file, ProgramNode* programNode, const char* fileName, const char* cppName)
{
	char buf[4096];
	std::string pafcorePath;
	GetRelativePath(pafcorePath, fileName, g_options.m_pafcorePath.c_str());
	FormatPathForInclude(pafcorePath);
	if (!pafcorePath.empty() && '/' != pafcorePath.back())
	{
		pafcorePath += '/';
	}

	writeStringToFile("#pragma once\n\n", file);

	sprintf_s(buf, "#include \"%s.h\"\n", cppName);
	writeStringToFile(buf, file);

	sprintf_s(buf, "#include \"%s%s\"\n", cppName, g_options.m_metaHeaderFilePostfix.c_str());
	writeStringToFile(buf, file);

	sprintf_s(buf, "#include \"%smemory.h\"\n\n", pafcorePath.c_str());
	writeStringToFile(buf, file);

	generateCode_Namespace(file, programNode, -1);
}

void SourceFileGenerator::generateCode_Namespace(FILE* file, NamespaceNode* namespaceNode, int indentation)
{
	//if (namespaceNode->isNoCode())
	//{
	//	file = 0;
	//}

	char buf[4096];
	if(!namespaceNode->isGlobalNamespace())
	{
		sprintf_s(buf, "namespace %s\n", namespaceNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation);
		writeStringToFile("{\n\n", file, indentation);
	}
	std::vector<MemberNode*> memberNodes;
	namespaceNode->m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		switch (memberNode->m_nodeType)
		{
		case snt_namespace:
			generateCode_Namespace(file, static_cast<NamespaceNode*>(memberNode), indentation + 1);
			break;
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(memberNode), "", indentation + 1);
			break;
		}
	}

	if(!namespaceNode->isGlobalNamespace())
	{
		writeStringToFile("}\n\n", file, indentation);
	}
}

void GetClassName(std::string& className, ClassNode* classNode)
{
	className = classNode->m_name->m_str;
	if(classNode->m_templateParametersNode)
	{
		std::vector<IdentifierNode*> templateParameterNodes;
		classNode->m_templateParametersNode->collectParameterNodes(templateParameterNodes);
		className += "<";
		size_t count = templateParameterNodes.size();
		for(size_t i = 0; i < count; ++i)
		{
			if(0 != i)
			{
				className += ", ";
			}
			className += templateParameterNodes[i]->m_str;
		}
		className += ">";
	}
}

void SourceFileGenerator::generateCode_Class(FILE* file, ClassNode* classNode, const std::string& scopeClassName, int indentation)
{
	//if (classNode->isNoCode())
	//{
	//	file = 0;
	//}
	std::string typeName;
	GetClassName(typeName, classNode);
	typeName = scopeClassName + typeName;

	bool isInline = 0 != classNode->m_templateParametersNode;

	if (!(classNode->isNoCode() || classNode->isNoMeta()))
	{
		generateCode_TemplateHeader(file, classNode, indentation);
		if (isInline)
		{
			writeStringToFile("inline ::pafcore::ClassType* ", file, indentation);
		}
		else
		{
			writeStringToFile("::pafcore::ClassType* ", file, indentation);
		}
		writeStringToFile(typeName.c_str(), file);
		writeStringToFile("::GetType()\n", file);
		writeStringToFile("{\n", file, indentation);
		writeStringToFile("return ::RuntimeTypeOf<", file, indentation + 1);
		writeStringToFile(typeName.c_str(), file);
		writeStringToFile(">::RuntimeType::GetSingleton();\n", file);
		writeStringToFile("}\n\n", file, indentation);

		if (classNode->isDerivedFromObject())
		{
			generateCode_TemplateHeader(file, classNode, indentation);
			if (isInline)
			{
				writeStringToFile("inline ::pafcore::ClassType* ", file, indentation);
			}
			else
			{
				writeStringToFile("::pafcore::ClassType* ", file, indentation);
			}
			writeStringToFile(typeName.c_str(), file);
			writeStringToFile("::getType()\n", file);
			writeStringToFile("{\n", file, indentation);
			writeStringToFile("return ::RuntimeTypeOf<", file, indentation + 1);
			writeStringToFile(typeName.c_str(), file);
			writeStringToFile(">::RuntimeType::GetSingleton();\n", file);
			writeStringToFile("}\n\n", file, indentation);

			if (classNode->isDerivedFromInterface())
			{
				generateCode_TemplateHeader(file, classNode, indentation);
				if (isInline)
				{
					writeStringToFile("inline size_t ", file, indentation);
				}
				else
				{
					writeStringToFile("size_t ", file, indentation);
				}
				writeStringToFile(typeName.c_str(), file);
				writeStringToFile("::getAddress()\n", file);
				writeStringToFile("{\n", file, indentation);
				//writeStringToFile("return reinterpret_cast<size_t>(static_cast<::pafcore::Object*>(this));\n", file, indentation + 1);
				writeStringToFile("return (size_t)this;\n", file, indentation + 1);
				writeStringToFile("}\n\n", file, indentation);
			}
		}
	}

	if (!classNode->isNoCode())
	{
		if (!classNode->m_additionalMethods.empty())
		{
			size_t count = classNode->m_additionalMethods.size();
			for (size_t i = 0; i < count; ++i)
			{
				generateCode_AdditionalMethod(file, classNode->m_additionalMethods[i], typeName, indentation);
			}
		}
	}


	typeName += "::";
	std::vector<MemberNode*> memberNodes;
	classNode->m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for (size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		switch (memberNode->m_nodeType)
		{
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(memberNode), typeName, indentation);
			break;
		}
	}
}

void SourceFileGenerator::generateCode_TemplateHeader(FILE* file, ClassNode* classNode, int indentation)
{
	if(classNode->m_templateParametersNode)
	{
		std::vector<IdentifierNode*> templateParameterNodes;
		classNode->m_templateParametersNode->collectParameterNodes(templateParameterNodes);
		writeStringToFile("template<", file, indentation);
		size_t count = templateParameterNodes.size();
		for(size_t i = 0; i < count; ++i)
		{
			if(0 != i)
			{
				writeStringToFile(",", file);
			}
			writeStringToFile("typename ", file);
			writeStringToFile(templateParameterNodes[i]->m_str.c_str(), file);
		}
		writeStringToFile(">\n", file);
	}
}


void SourceFileGenerator::generateCode_AdditionalMethod(FILE* file, MethodNode* methodNode, const std::string& typeName, int indentation)
{
	char buf[4096];

	if (methodNode->isNoCode())
	{
		file = 0;
	}

	ClassNode* classNode = static_cast<ClassNode*>(methodNode->m_enclosing);
	//std::string typeName;
	//GetClassName(typeName, classNode);
	bool isInline = 0 != classNode->m_templateParametersNode;

	generateCode_TemplateHeader(file, classNode, indentation);

	if (isInline)
	{
		writeStringToFile("inline ", file, indentation);
	}
	assert(methodNode->m_resultList && methodNode->m_resultList->m_variable
		&& nullptr == methodNode->m_resultList->m_variableList && nullptr == methodNode->m_resultList->m_variable->m_byRef);

	VariableNode* resultVariable = methodNode->m_resultList->m_variable;
	generateCode_ResultType(file, resultVariable->m_compoundType, nullptr, classNode->m_enclosing, false, isInline ? 0 : indentation, false);

	writeStringToFile(typeName.c_str(), file);
	writeStringToFile("::", file);

	generateCode_Identifier(file, methodNode->m_name, 0, false);

	generateCode_Token(file, methodNode->m_leftParenthesis, 0, false);

	std::vector<std::pair<TokenNode*, VariableNode*>> parameterNodes;
	if (methodNode->m_parameterList)
	{
		methodNode->m_parameterList->collectVariableNodes(parameterNodes);
	}
	for (auto& parameterNode : parameterNodes)
	{
		if (parameterNode.first)
		{
			generateCode_Token(file, parameterNode.first, 0, false);
		}
		generateCode_Parameter(file, parameterNode.second, false, methodNode->m_enclosing, false);
	}
	
	generateCode_Token(file, methodNode->m_rightParenthesis, 0, false);
	writeStringToFile("\n", file);
	writeStringToFile("{\n", file, indentation);

	if ("New" == methodNode->m_name->m_str)
	{
		sprintf_s(buf, "return ::pafcore::MakeShared<%s>(", typeName.c_str());
	}
	else
	{
		assert("NewArray" == methodNode->m_name->m_str);
		sprintf_s(buf, "return ::pafcore::MakeSharedArray<%s>(", typeName.c_str());
	}

	writeStringToFile(buf, file, indentation + 1);

	for (auto& parameterNode : parameterNodes)
	{
		if (parameterNode.first)
		{
			writeStringToFile(", ", file);
		}
		writeStringToFile(parameterNode.second->m_name->m_str.c_str(), file);
	}
	writeStringToFile(");\n", file);
	
	writeStringToFile("}\n\n", file, indentation);
}

