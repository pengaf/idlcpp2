#include "meta_header_file_generator.h"
#include "utility.h"
#include "source_file.h"
#include "header_file_generator.h"
#include "program_node.h"
#include "namespace_node.h"
#include "token_node.h"
#include "identifier_node.h"
#include "enumerator_list_node.h"
#include "member_list_node.h"
#include "enum_node.h"
#include "class_node.h"
#include "template_class_instance_node.h"
#include "template_parameters_node.h"
#include "typedef_node.h"
#include "type_declaration_node.h"
#include "type_name_list_node.h"
#include "type_name_node.h"
#include "variable_list_node.h"
#include "variable_node.h"
#include "field_node.h"
#include "property_node.h"
#include "method_node.h"
#include "type_tree.h"
#include "platform.h"
#include "options.h"
#include "common_funcs.h"
#include "compiler.h"
#include "code_generator.h"
#include <assert.h>
#include <algorithm>


const char g_metaMethodPrefix[] = "static ::pafcore::ErrorCode ";
const char g_metaMethodPostfix[] = "(::pafcore::Variant** results, int numResults, ::pafcore::Variant** arguments, int numArguments);\n";

void writeMetaMethodDecl(const char* funcName, FILE* file, int indentation)
{
	writeStringToFile(g_metaMethodPrefix, sizeof(g_metaMethodPrefix) - 1, file, indentation);
	writeStringToFile(funcName, file);
	writeStringToFile(g_metaMethodPostfix, sizeof(g_metaMethodPostfix) - 1, file);
}

void writeMetaMethodDecls(ClassNode* classNode, std::vector<MethodNode*> methodNodes, FILE* file, int indentation)
{
	size_t methodCount = methodNodes.size();
	if(0 < methodCount)
	{
		writeStringToFile("public:\n", file, indentation);
		for(size_t i = 0 ; i < methodCount; ++i)
		{
			char funcName[256];
			if(i > 0 && methodNodes[i]->m_name->m_str == methodNodes[i-1]->m_name->m_str)
			{
				continue;
			}
			sprintf_s(funcName, "%s_%s", classNode->m_name->m_str.c_str(), methodNodes[i]->m_name->m_str.c_str());
			writeMetaMethodDecl(funcName, file, indentation + 1);
		}
	}
}

const char* g_metaPropertyDeclPrefix = "static ::pafcore::ErrorCode ";
const char* g_metaPropertyDeclGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclSetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* value);\n";

const char* g_metaPropertyDeclArrayGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t index, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclArraySetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t index, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclArraySizePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t& size);\n";
const char* g_metaPropertyDeclArrayResizePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t size);\n";

const char* g_metaPropertyDeclListIteratePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* iterator);\n";
const char* g_metaPropertyDeclListGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclListSetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclListInsertPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaPropertyDeclListErasePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator);\n";

const char* g_metaPropertyDeclEnumPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* candidates);\n";

const char* g_metaStaticPropertyDeclGetPostfix = "(::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclSetPostfix = "(::pafcore::Variant* value);\n";

const char* g_metaStaticPropertyDeclArrayGetPostfix = "(size_t index, ::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclArraySetPostfix = "(size_t index, ::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclArraySizePostfix = "(size_t& size);\n";
const char* g_metaStaticPropertyDeclArrayResizePostfix = "(size_t size);\n";

const char* g_metaStaticPropertyDeclListIteratePostfix = "(::pafcore::Variant* iterator);\n";
const char* g_metaStaticPropertyDeclListGetPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclListSetPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclListInsertPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value);\n";
const char* g_metaStaticPropertyDeclListErasePostfix = "(::pafcore::Iterator* iterator);\n";

const char* g_metaStaticPropertyDeclEnumPostfix = "(::pafcore::Variant* candidates);\n";


void writeMetaPropertyItemDecl(ClassNode* classNode, PropertyNode* propertyNode, const char* item, const char* prefix, const char* postfix, FILE* file, int indentation)
{
	char funcName[256];
	sprintf_s(funcName, "%s_%s_%s", classNode->m_name->m_str.c_str(), item, propertyNode->m_name->m_str.c_str());
	writeStringToFile(prefix, file, indentation);
	writeStringToFile(funcName, file);
	writeStringToFile(postfix, file);
}

void writeMetaPropertyDecl(ClassNode* classNode, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char funcName[256];
	int itemIndentation = indentation + 1;
	const char* prefix = g_metaPropertyDeclPrefix;
	const char* postfix = "";
	if(propertyNode->m_get)
	{
		if (propertyNode->isStatic())
		{
			switch (propertyNode->getKind())
			{
			case simple_property:
				postfix = g_metaStaticPropertyDeclGetPostfix;
				break;
			case fixed_array_property:
			case dynamic_array_property:
				postfix = g_metaStaticPropertyDeclArrayGetPostfix;
				break;
			case list_property:
				postfix = g_metaStaticPropertyDeclListGetPostfix;
				break;
			}
		}
		else
		{
			switch (propertyNode->getKind())
			{
			case simple_property:
				postfix = g_metaPropertyDeclGetPostfix;
				break;
			case fixed_array_property:
			case dynamic_array_property:
				postfix = g_metaPropertyDeclArrayGetPostfix;
				break;
			case list_property:
				postfix = g_metaPropertyDeclListGetPostfix;
				break;
			}
		}
		writeMetaPropertyItemDecl(classNode, propertyNode, "get", prefix, postfix, file, itemIndentation);
	}		
	if(propertyNode->m_set)
	{
		if (propertyNode->isStatic())
		{
			switch (propertyNode->getKind())
			{
			case simple_property:
				postfix = g_metaStaticPropertyDeclSetPostfix;
				break;
			case fixed_array_property:
			case dynamic_array_property:
				postfix = g_metaStaticPropertyDeclArraySetPostfix;
				break;
			case list_property:
				postfix = g_metaStaticPropertyDeclListSetPostfix;
				break;
			}
		}
		else
		{
			switch (propertyNode->getKind())
			{
			case simple_property:
				postfix = g_metaPropertyDeclSetPostfix;
				break;
			case fixed_array_property:
			case dynamic_array_property:
				postfix = g_metaPropertyDeclArraySetPostfix;
				break;
			case list_property:
				postfix = g_metaPropertyDeclListSetPostfix;
				break;
			}
		}
		writeMetaPropertyItemDecl(classNode, propertyNode, "set", prefix, postfix, file, itemIndentation);
	}
	if (propertyNode->m_enumerate)
	{
		postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclEnumPostfix : g_metaPropertyDeclEnumPostfix;
		writeMetaPropertyItemDecl(classNode, propertyNode, "enum", prefix, postfix, file, itemIndentation);
	}
	if(propertyNode->isFixedArray() || propertyNode->isDynamicArray())
	{
		postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclArraySizePostfix : g_metaPropertyDeclArraySizePostfix;
		writeMetaPropertyItemDecl(classNode, propertyNode, "size", prefix, postfix, file, itemIndentation);
		if (propertyNode->isDynamicArray())
		{
			postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclArrayResizePostfix : g_metaPropertyDeclArrayResizePostfix;
			writeMetaPropertyItemDecl(classNode, propertyNode, "resize", prefix, postfix, file, itemIndentation);
		}
	}
	else if (propertyNode->isList())
	{
		postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclListIteratePostfix : g_metaPropertyDeclListIteratePostfix;
		writeMetaPropertyItemDecl(classNode, propertyNode, "iterate", prefix, postfix, file, itemIndentation);

		postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclListInsertPostfix : g_metaPropertyDeclListInsertPostfix;
		writeMetaPropertyItemDecl(classNode, propertyNode, "insert", prefix, postfix, file, itemIndentation);

		postfix = propertyNode->isStatic() ? g_metaStaticPropertyDeclListErasePostfix : g_metaPropertyDeclListErasePostfix;
		writeMetaPropertyItemDecl(classNode, propertyNode, "erase", prefix, postfix, file, itemIndentation);
	}
}

void writeMetaPropertyDecls(ClassNode* classNode, std::vector<PropertyNode*> propertyNodes, FILE* file, int indentation)
{
	size_t propertyCount = propertyNodes.size();
	if(0 < propertyCount)
	{
		writeStringToFile("public:\n", file, indentation);
		for(size_t i = 0 ; i < propertyCount; ++i)
		{
			writeMetaPropertyDecl(classNode, propertyNodes[i], file, indentation);
		}
	}
}

void MetaHeaderFileGenerator::generateCode(FILE* dstFile, SourceFile* sourceFile, const char* fullPathName, const char* baseName)
{
	generateCode_Program(dstFile, sourceFile, sourceFile->m_syntaxTree, fullPathName, baseName);
}

void MetaHeaderFileGenerator::generateCode_Program(FILE* file, SourceFile* sourceFile, ProgramNode* programNode, const char* fileName, const char* cppName)
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

	if(0 == programNode->m_memberList)
	{
		return;
	}

	g_compiler.outputUsedTypesForMetaHeader(file, sourceFile);
	std::string headerName = cppName;

	sprintf_s(buf, "#include \"%s.h\"\n", headerName.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sclass_type.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%senum_type.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%stype_alias.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%svariant.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%ssubclass_invoker.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);

	writeStringToFile("\nnamespace idlcpp\n{\n\n", file);
	generateCode_Namespace(file, programNode, 1);
	writeStringToFile("}\n\n", file);


	std::vector<TypeNode*> typeNodes;
	CollectTypeNodes(typeNodes, programNode);
	std::reverse(typeNodes.begin(), typeNodes.end());
	size_t typeCount = typeNodes.size();

	for(size_t i = 0; i < typeCount; ++i)
	{
		TypeNode* typeNode = typeNodes[i];
		if(!typeNode->isTypedef() 
			&& !typeNode->isTypeDeclaration()
			&& typeNode->getSyntaxNode()->canGenerateMetaCode())
		{
			TypeKind typeKind = typeNode->getTypeKind(0);
			const char* typeKindName = "";
			switch(typeKind)
			{
			case enum_type:
				typeKindName = "enum_instance";
				break;
			case class_type:
				typeKindName = "class_instance";
				break;
			default:
				assert(false);
			}

			std::string typeName;
			typeNode->getNativeName(typeName);
			std::string metaTypeName;
			GetMetaTypeFullName(metaTypeName, typeNode);
			sprintf_s(buf, "template<>\n"
				"struct RuntimeTypeOf<%s>\n"
				"{\n"
				"\ttypedef ::idlcpp::%s RuntimeType;\n"
				"\tenum {type_kind = ::pafcore::MetadataKind::%s};\n"
				"};\n\n",
				typeName.c_str(), metaTypeName.c_str(), typeKindName);
			writeStringToFile(buf, file);
		}
	}
}

void MetaHeaderFileGenerator::generateCode_Namespace(FILE* file, NamespaceNode* namespaceNode, int indentation)
{
	if (namespaceNode->isNoMeta())
	{
		return;
	}
	std::vector<MemberNode*> memberNodes;
	namespaceNode->m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		switch (memberNode->m_nodeType)
		{
		case snt_enum:
			generateCode_Enum(file, static_cast<EnumNode*>(memberNode), 0, indentation);
			break;
		case snt_class:
			if (!memberNode->isTemplateClass())
			{
				generateCode_Class(file, static_cast<ClassNode*>(memberNode), 0, indentation);
			}
			break;
		case snt_namespace:
			generateCode_Namespace(file, static_cast<NamespaceNode*>(memberNode), indentation);
			break;
		case snt_template_class_instance:
			generateCode_TemplateClassInstance(file, static_cast<TemplateClassInstanceNode*>(memberNode), indentation);
			break;
		case snt_typedef:
			generateCode_Typedef(file, static_cast<TypedefNode*>(memberNode), 0, indentation);
			break;
		case snt_type_declaration:
			generateCode_TypeDeclaration(file, static_cast<TypeDeclarationNode*>(memberNode), 0, indentation);
			break;
		default:
			assert(false);
		}
	}
}


void MetaHeaderFileGenerator::generateCode_Enum(FILE* file, EnumNode* enumNode, TemplateArguments* templateArguments, int indentation)
{
	if (enumNode->isNoMeta())
	{
		return;
	}

	char buf[4096];
	std::string metaTypeName;
	GetMetaTypeFullName(metaTypeName, enumNode, templateArguments);

	sprintf_s(buf, "class %s : public ::pafcore::EnumType\n",
		metaTypeName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	writeStringToFile("public:\n", file, indentation);

	writeStringToFile(metaTypeName.c_str(), metaTypeName.length(), file, indentation + 1);
	writeStringToFile("();\n", file);

	writeStringToFile("public:\n", file, indentation);
	sprintf_s(buf, "%s static %s* GetSingleton();\n", 
		g_options.m_exportMacro.c_str(), metaTypeName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("};\n\n", file, indentation);
}

void MetaHeaderFileGenerator::generateCode_Class(FILE* file, ClassNode* classNode, TemplateClassInstanceNode* templateClassInstance, int indentation)
{
	if (classNode->isNoMeta())
	{
		return;
	}
	TemplateArguments* templateArguments = templateClassInstance ? &templateClassInstance->m_templateArguments : 0;

	std::vector<IdentifierNode*> reservedNames;
	if (templateClassInstance && templateClassInstance->m_reservedMemberList
		&& templateClassInstance->m_classTypeNode->m_classNode == classNode)
	{
		assert(classNode->m_typeNode == templateClassInstance->m_classTypeNode);
		templateClassInstance->getReservedMembers(reservedNames);
	}
	bool hasReservedMember = !reservedNames.empty();

	char buf[4096];
	std::string metaClassName;
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	sprintf_s(buf, "class %s : public ::pafcore::ClassType\n",
		metaClassName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::vector<MemberNode*> memberNodes;
	std::vector<MethodNode*> methodNodes;
	std::vector<MethodNode*> staticMethodNodes;
	std::vector<MethodNode*> typeMethodNodes;
	std::vector<PropertyNode*> propertyNodes;
	std::vector<PropertyNode*> staticPropertyNodes;
	std::vector<MemberNode*> subTypeNodes;

	classNode->m_memberList->collectMemberNodes(memberNodes);
	size_t memberCount = memberNodes.size();
	for(size_t i = 0; i < memberCount; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		if (hasReservedMember)
		{
			if (snt_method == memberNode->m_nodeType || snt_property == memberNode->m_nodeType)
			{
				if (!std::binary_search(reservedNames.begin(), reservedNames.end(), memberNode->m_name, CompareIdentifierPtr()))
				{
					continue;
				}
			}	
		}

		if(!memberNode->isNoMeta())
		{
			if(snt_method == memberNode->m_nodeType)
			{
				MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
				if(memberNode->m_name->m_str != classNode->m_name->m_str)
				{
					if(methodNode->isStatic())
					{
						if (methodNode->m_name->m_str == "__destruct__"
							|| methodNode->m_name->m_str == "__assign__")
						{
							typeMethodNodes.push_back(methodNode);
						}
						else
						{
							staticMethodNodes.push_back(methodNode);
						}
					}
					else
					{
						methodNodes.push_back(methodNode);
					}
				}
			}
			else if(snt_property == memberNode->m_nodeType)
			{
				PropertyNode* propertyNode = static_cast<PropertyNode*>(memberNode);
				if(propertyNode->isStatic())
				{
					staticPropertyNodes.push_back(propertyNode);
				}
				else
				{
					propertyNodes.push_back(propertyNode);
				}
			}
			else if(snt_enum == memberNode->m_nodeType
				|| snt_class == memberNode->m_nodeType
				|| snt_typedef == memberNode->m_nodeType
				|| snt_type_declaration == memberNode->m_nodeType)
			{
				subTypeNodes.push_back(memberNode);
			}
			else
			{
				assert(snt_field == memberNode->m_nodeType);
			}
		}
	}
	
	//if(!classNode->isAbstractClass())
	{
		auto it = classNode->m_additionalMethods.begin();
		auto end = classNode->m_additionalMethods.end();
		for (; it != end; ++it)
		{
			MethodNode* methodNode = *it;
			if (!reservedNames.empty())
			{
				if (!std::binary_search(reservedNames.begin(), reservedNames.end(), methodNode->m_name, CompareIdentifierPtr()))
				{
					continue;
				}
			}
			if (!methodNode->isNoMeta())
			{
				staticMethodNodes.push_back(methodNode);
			}
		}
	}

	std::sort(propertyNodes.begin(), propertyNodes.end(), CompareMemberNodeByName());
	std::sort(methodNodes.begin(), methodNodes.end(), CompareMemberNodeByName());
	std::sort(staticPropertyNodes.begin(), staticPropertyNodes.end(), CompareMemberNodeByName());
	std::sort(staticMethodNodes.begin(), staticMethodNodes.end(), CompareMemberNodeByName());


	writeStringToFile("public:\n", file, indentation);
	writeStringToFile(metaClassName.c_str(), metaClassName.length(), file, indentation + 1);
	writeStringToFile("();\n", file);

	writeStringToFile("public:\n", file, indentation);
	writeStringToFile("virtual void destruct(void* address, size_t count);\n", file, indentation + 1);
	writeStringToFile("virtual void copyConstruct(void* dst, const void* src, size_t count);\n", file, indentation + 1);
	writeStringToFile("virtual void moveConstruct(void* dst, void* src, size_t count);\n", file, indentation + 1);
	writeStringToFile("virtual void copyAssign(void* dst, const void* src, size_t count);\n", file, indentation + 1);
	writeStringToFile("virtual void moveAssign(void* dst, void* src, size_t count);\n", file, indentation + 1);

	if(classNode->needSubclassProxy(templateArguments))
	{
		std::string className;
		classNode->getNativeName(className, templateArguments);

		writeStringToFile("public:\n", file, indentation);
		sprintf_s(buf, "virtual pafcore::SharedPtr<%s> createSubclassProxy(::pafcore::SubclassInvoker* subclassInvoker);\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeMetaPropertyDecls(classNode, propertyNodes, file, indentation);
	writeMetaMethodDecls(classNode, methodNodes, file, indentation);
	writeMetaPropertyDecls(classNode, staticPropertyNodes, file, indentation);
	writeMetaMethodDecls(classNode, staticMethodNodes, file, indentation);


	writeStringToFile("public:\n", file, indentation);
	sprintf_s(buf, "%s static %s* GetSingleton();\n", 
		g_options.m_exportMacro.c_str(), metaClassName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("};\n\n", file, indentation);

	if(classNode->needSubclassProxy(templateArguments))
	{
		generateCode_SubclassProxy(file, classNode, templateArguments, indentation);
	}

	size_t subTypeCount = subTypeNodes.size();
	for(size_t i = 0; i < subTypeCount; ++i)
	{
		MemberNode* typeNode = subTypeNodes[i];
		switch (typeNode->m_nodeType)
		{
		case snt_enum:
			generateCode_Enum(file, static_cast<EnumNode*>(typeNode), templateArguments, indentation);
			break;
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(typeNode), templateClassInstance, indentation);
			break;
		case snt_typedef:
			generateCode_Typedef(file, static_cast<TypedefNode*>(typeNode), templateArguments, indentation);
			break;
		case snt_type_declaration:
			generateCode_TypeDeclaration(file, static_cast<TypeDeclarationNode*>(typeNode), templateArguments, indentation);
			break;
		default:
			assert(false);
		}
	}
}

void writeInterfaceMethodDecl(MethodNode* methodNode, FILE* file, int indentation)
{
	ScopeNode* scopeNode = methodNode->getProgramNode();

	std::vector<VariableNode*> resultNodes;
	if (methodNode->m_resultList)
	{
		methodNode->m_resultList->collectVariableNodes(resultNodes);
		generateCode_ResultType(file, resultNodes.front()->m_compoundType,
			resultNodes.front()->m_byRef, scopeNode, true, indentation, false);
		indentation = 0;
	}
	else
	{
		writeStringToFile("void ", file, indentation);
		indentation = 0;
	}
	generateCode_Identifier(file, methodNode->m_name, indentation, false);
	generateCode_Token(file, methodNode->m_leftParenthesis, 0, false);
	if (resultNodes.size() > 1)
	{
		for (size_t i = 1; i < resultNodes.size(); ++i)
		{
			generateCode_Parameter(file, resultNodes[i], true, scopeNode, false);
			if (i + 1 < resultNodes.size() || methodNode->m_parameterList)
			{
				writeStringToFile(",", file);
			}
		}
	}
	if (methodNode->m_parameterList)
	{
		std::vector<std::pair<TokenNode*, VariableNode*>> parameterNodes;
		methodNode->m_parameterList->collectVariableNodes(parameterNodes);
		size_t parameterCount = parameterNodes.size();
		for (size_t i = 0; i < parameterCount; ++i)
		{
			if (parameterNodes[i].first)
			{
				generateCode_Token(file, parameterNodes[i].first, 0, false);
			}
			generateCode_Parameter(file, parameterNodes[i].second, false, scopeNode, false);
		}
	}
	generateCode_Token(file, methodNode->m_rightParenthesis, 0, false);
	writeStringToFile(" override", file);
	generateCode_Token(file, methodNode->m_semicolon, 0, false);
	writeStringToFile("\n", file);
}


void writeInterfaceMethodsDecl(FILE* file, ClassNode* classNode, TemplateArguments* templateArguments, int indentation)
{
	std::vector<MethodNode*> methodNodes;
	classNode->collectOverrideMethods(methodNodes, templateArguments);
	size_t count = methodNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		MethodNode* methodNode = methodNodes[i];
		assert(snt_method == methodNode->m_nodeType && methodNode->isVirtual());
		writeInterfaceMethodDecl(methodNode, file, indentation);
	}
}

void MetaHeaderFileGenerator::generateCode_SubclassProxy(FILE* file, ClassNode* classNode, TemplateArguments* templateArguments, int indentation)
{
	char buf[4096];
	std::string className;
	classNode->getFullName(className, templateArguments);
	std::string subclassProxyName;
	GetSubclassProxyFullName(subclassProxyName, classNode, templateArguments);
	
	sprintf_s(buf, "class %s : public %s\n", subclassProxyName.c_str(), className.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeStringToFile("public:\n", file, indentation);
	writeStringToFile("::pafcore::SubclassInvoker* m_subclassInvoker;\n", file, indentation + 1);
	writeStringToFile("public:\n", file, indentation);
	sprintf_s(buf, "%s(::pafcore::SubclassInvoker* subclassInvoker);\n", subclassProxyName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	sprintf_s(buf, "~%s();\n", subclassProxyName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeInterfaceMethodsDecl(file, classNode, templateArguments, indentation + 1);
	writeStringToFile("};\n\n", file, indentation);

}

void MetaHeaderFileGenerator::generateCode_TemplateClassInstance(FILE* file, TemplateClassInstanceNode* templateClassInstance, int indentation)
{
	if (templateClassInstance->isNoMeta())
	{
		return;
	}

	ClassNode* classNode = static_cast<ClassNode*>(templateClassInstance->m_classTypeNode->m_classNode);
	generateCode_Class(file, classNode, templateClassInstance, indentation);

}

void MetaHeaderFileGenerator::generateCode_Typedef(FILE* file, TypedefNode* typedefNode, TemplateArguments* templateArguments, int indentation)
{
	if (typedefNode->isNoMeta())
	{
		return;
	}

	char buf[4096];
	std::string metaTypeName;
	GetMetaTypeFullName(metaTypeName, typedefNode, templateArguments);

	sprintf_s(buf, "class %s : public ::pafcore::TypeAlias\n",
		metaTypeName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	writeStringToFile("public:\n", file, indentation);

	writeStringToFile(metaTypeName.c_str(), metaTypeName.length(), file, indentation + 1);
	writeStringToFile("();\n", file);

	writeStringToFile("public:\n", file, indentation);
	sprintf_s(buf, "%s static %s* GetSingleton();\n", 
		g_options.m_exportMacro.c_str(), metaTypeName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("};\n\n", file, indentation);
}

void MetaHeaderFileGenerator::generateCode_TypeDeclaration(FILE* file, TypeDeclarationNode* typeDeclarationNode, TemplateArguments* templateArguments, int indentation)
{
	if (typeDeclarationNode->isNoMeta())
	{
		return;
	}

	char buf[4096];
	std::string metaTypeName;
	GetMetaTypeFullName(metaTypeName, typeDeclarationNode, templateArguments);

	sprintf_s(buf, "class %s : public ::pafcore::TypeAlias\n",
		metaTypeName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	writeStringToFile("public:\n", file, indentation);

	writeStringToFile(metaTypeName.c_str(), metaTypeName.length(), file, indentation + 1);
	writeStringToFile("();\n", file);

	writeStringToFile("public:\n", file, indentation);
	sprintf_s(buf, "%s static %s* GetSingleton();\n",
		g_options.m_exportMacro.c_str(), metaTypeName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("};\n\n", file, indentation);
}

