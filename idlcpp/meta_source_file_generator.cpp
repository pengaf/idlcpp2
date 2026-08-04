#include "meta_source_file_generator.h"
#include "utility.h"
#include "source_file.h"
#include "program_node.h"
#include "namespace_node.h"
#include "token_node.h"
#include "identifier_node.h"
#include "enumerator_list_node.h"
#include "enumerator_node.h"
#include "enum_node.h"
#include "class_node.h"
#include "template_parameters_node.h"
#include "template_class_instance_node.h"
#include "typedef_node.h"
#include "type_declaration_node.h"
#include "type_name_list_node.h"
#include "type_name_node.h"
#include "member_list_node.h"
#include "field_node.h"
#include "property_accessor_node.h"
#include "property_node.h"
#include "variable_node.h"
#include "variable_list_node.h"
#include "method_node.h"
#include "compound_type_node.h"
#include "attribute_node.h"
#include "attribute_list_node.h"
#include "type_tree.h"
#include "platform.h"
#include "common_funcs.h"
#include "options.h"
#include "compiler.h"

#include <assert.h>
#include <algorithm>
#include <vector>
#include <string>

const char* TypeCompoundString(CompoundTypeNode* compoundTypeNode)
{
	const char* typeCompound = "::pafcore::TypeCompound::none";
	switch (compoundTypeNode->m_typeCompound)
	{
	case tc_raw_ptr:
		typeCompound = "::pafcore::TypeCompound::raw_ptr";
		break;
	case tc_shared_ptr:
		typeCompound = "::pafcore::TypeCompound::shared_ptr";
		break;
	case tc_observer_ptr:
		typeCompound = "::pafcore::TypeCompound::observer_ptr";
		break;
	case tc_shared_array:
		typeCompound = "::pafcore::TypeCompound::shared_array";
		break;
	case tc_observer_array:
		typeCompound = "::pafcore::TypeCompound::observer_array";
		break;
	}
	return typeCompound;
}

void generateCode_Parameter(FILE* file, VariableNode* parameter, bool output, ScopeNode* scopeNode, bool outputEmbededCode);
void writeOverrideMethodParameter(MethodNode* methodNode, VariableNode* parameterNode, FILE* file);

void writeMetaConstructor(ClassNode* classNode,
	TemplateArguments* templateArguments,
	std::vector<MemberNode*>& nestedTypeNodes,
	std::vector<MemberNode*>& nestedTypeAliasNodes,
	std::vector<FieldNode*>& staticFieldNodes,
	std::vector<PropertyNode*>& staticPropertyNodes,
	std::vector<MethodNode*>& staticMethodNodes,
	std::vector<FieldNode*>& fieldNodes,
	std::vector<PropertyNode*>& propertyNodes,
	std::vector<MethodNode*>& methodNodes,
	FILE* file, int indentation);

void writeMetaPropertyImpls(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<PropertyNode*>& propertyNodes, FILE* file, int indentation);
void writeMetaMethodImpls(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<MethodNode*>& methodNodes, bool isStatic, FILE* file, int indentation);
void writeMetaGetSingletonImpls(MemberNode* typeNode, TemplateArguments* templateArguments, FILE* file, int indentation);
void writeEnumMetaConstructor(EnumNode* enumNode, TemplateArguments* templateArguments, std::vector<EnumeratorNode*>& enumerators, FILE* file, int indentation);


void MetaSourceFileGenerator::generateCode(FILE* dstFile, SourceFile* sourceFile, const char* fullPathName, const char* baseName)
{
	generateCode_Program(dstFile, sourceFile, fullPathName, baseName);
}

void MetaSourceFileGenerator::generateCode_Program(FILE* file, SourceFile* sourceFile, const char* fileName, const char* cppName)
{
	ProgramNode* programNode = sourceFile->m_syntaxTree;
	char buf[4096];
	std::string pafcorePath;
	GetRelativePath(pafcorePath, fileName, g_options.m_pafcorePath.c_str());
	FormatPathForInclude(pafcorePath);
	if (!pafcorePath.empty() && '/' != pafcorePath.back())
	{
		pafcorePath += '/';
	}

	writeStringToFile("#pragma once\n\n", file);
	g_compiler.outputUsedTypesForMetaSource(file, sourceFile);
	sprintf_s(buf, "#include \"%s%s\"\n", cppName, g_options.m_metaHeaderFilePostfix.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sauto_run.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sname_space.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sresult.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sparameter.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sinstance_field.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sstatic_field.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sinstance_property.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sstatic_property.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sinstance_method.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sstatic_method.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%senum_member.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%sprimitive_type.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%smemory.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	sprintf_s(buf, "#include \"%siterator.h\"\n", pafcorePath.c_str());
	writeStringToFile(buf, file);
	writeStringToFile("#include <new>\n\n", file);

	writeStringToFile("\nnamespace idlcpp\n{\n\n", file);
	generateCode_Namespace(file, programNode, 1);
	writeStringToFile("}\n\n", file);

	std::vector<TypeNode*> typeNodes;
	CollectTypeNodes(typeNodes, programNode);
	std::reverse(typeNodes.begin(), typeNodes.end());
	for (TypeNode* typeNode : typeNodes)
	{
		if (typeNode->getSyntaxNode()->canGenerateMetaCode())
		{
			std::string metaTypeName;
			GetMetaTypeFullName(metaTypeName, typeNode);
			sprintf_s(buf, "AUTO_REGISTER_TYPE(::idlcpp::%s)\n", metaTypeName.c_str());
			writeStringToFile(buf, file);
			if (typeNode->isTypeDeclaration())
			{
				TypeKind typeKind = typeNode->getTypeKind(0);
				const char* typeKindName = "";
				switch (typeKind)
				{
				case primitive_type:
					typeKindName = "primitive_instance";
					break;
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
				sprintf_s(buf, "static_assert(static_cast<int>(RuntimeTypeOf<%s>::type_kind) == static_cast<int>(::pafcore::MetadataKind::%s), \"type kind error\");\n",
					typeName.c_str(), typeKindName);
				writeStringToFile(buf, file);
			}
		}
	}
}

void MetaSourceFileGenerator::generateCode_Namespace(FILE* file, NamespaceNode* namespaceNode, int indentation)
{
	if (namespaceNode->isNoMeta())
	{
		return;
	}

	std::vector<MemberNode*> memberNodes;
	namespaceNode->m_memberList->collectMemberNodes(memberNodes);
	for (MemberNode* memberNode : memberNodes)
	{
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

void MetaSourceFileGenerator::generateCode_Enum(FILE* file, EnumNode* enumNode, TemplateArguments* templateArguments, int indentation)
{
	if (enumNode->isNoMeta())
	{
		return;
	}
	std::vector<EnumeratorNode*> enumeratorNodes;
	enumNode->m_enumeratorList->collectEnumeratorNodes(enumeratorNodes);
	std::sort(enumeratorNodes.begin(), enumeratorNodes.end(), CompareEnumeratorPtr());
	writeEnumMetaConstructor(enumNode, templateArguments, enumeratorNodes, file, indentation);
	writeMetaGetSingletonImpls(enumNode, templateArguments, file, indentation);
}

void MetaSourceFileGenerator::generateCode_TemplateClassInstance(FILE* file, TemplateClassInstanceNode* templateClassInstance, int indentation)
{
	if (templateClassInstance->isNoMeta())
	{
		return;
	}
	ClassNode* classNode = static_cast<ClassNode*>(templateClassInstance->m_classTypeNode->m_classNode);
	generateCode_Class(file, classNode, templateClassInstance, indentation);
}

void calcOverrideFunctionName(std::string& res, const char* funcName, const std::vector<MethodNode*>& typeMethodNodes, const std::string& className)
{
	assert(res.empty());
	IdentifierNode tmpIdentifierNode(funcName, 0, 0, 0);
	MethodNode tmpMethodNode(nullptr, &tmpIdentifierNode, nullptr, nullptr, nullptr);
	auto it = std::lower_bound(typeMethodNodes.begin(), typeMethodNodes.end(), &tmpMethodNode, CompareMemberNodeByName());
	if (typeMethodNodes.end() != it && (*it)->m_name->m_str == tmpIdentifierNode.m_str)
	{
		MethodNode* methodNode = *it;
		if (methodNode->m_nativeName)
		{
			if (methodNode->m_nativeName->m_str.find(':') != std::string::npos)
			{
				res = methodNode->m_nativeName->m_str;
			}
			else
			{
				res = className + "::" + methodNode->m_nativeName->m_str;
			}
		}
		else
		{
			res = className + "::" + methodNode->m_name->m_str;
		}
	}
}

void writeOverrideFunctions(ClassNode* classNode, TemplateArguments* templateArguments, const std::vector<MethodNode*>& typeMethodNodes, FILE* file, int indentation)
{
	char buf[4096];
	std::string metaClassName;
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);
	std::string className;
	classNode->getNativeName(className, templateArguments);

	std::string destructName, assignName;
	calcOverrideFunctionName(destructName, "__destruct__", typeMethodNodes, className);
	calcOverrideFunctionName(assignName, "__assign__", typeMethodNodes, className);


	sprintf_s(buf, "void %s::destruct(void* address, size_t count)\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	if (destructName.empty())
	{
		destructName = "::pafcore::Destruct";
	}
	sprintf_s(buf, "%s(reinterpret_cast<%s*>(address), count);\n", destructName.c_str(), className.c_str());

	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);

	sprintf_s(buf, "void %s::assign(void* dst, const void* src)\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	if (assignName.empty())
	{
		assignName = "::pafcore::Assign";
	}
	sprintf_s(buf, "%s(reinterpret_cast<%s*>(dst), reinterpret_cast<const %s*>(src));\n", assignName.c_str(), className.c_str(), className.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);

	

	if (classNode->needSubclassProxy(templateArguments))
	{
		std::string subclassProxyName;
		GetSubclassProxyFullName(subclassProxyName, classNode, templateArguments);
		sprintf_s(buf, "pafcore::SharedPtr<%s> %s::createSubclassProxy(::pafcore::SubclassInvoker* subclassInvoker)\n", className.c_str(), metaClassName.c_str());
		writeStringToFile(buf, file, indentation);
		writeStringToFile("{\n", file, indentation);
		sprintf_s(buf, "return MakeShared<%s>(subclassInvoker);\n", subclassProxyName.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("}\n\n", file, indentation);
	}
}

struct CompareMethodNode
{
	bool operator()(const MethodNode* m1, const MethodNode* m2) const
	{
		int cmp = m1->m_name->m_str.compare(m2->m_name->m_str);
		if (0 != cmp)
		{
			return cmp < 0;
		}
		return m1->getParameterCount() < m2->getParameterCount();
	}
};

void MetaSourceFileGenerator::generateCode_Class(FILE* file, ClassNode* classNode, TemplateClassInstanceNode* templateClassInstance, int indentation)
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
	bool hasReservedMember = (!reservedNames.empty());

	std::vector<MemberNode*> memberNodes;
	std::vector<MethodNode*> methodNodes;
	std::vector<MethodNode*> staticMethodNodes;
	std::vector<MethodNode*> typeMethodNodes;
	std::vector<PropertyNode*> propertyNodes;
	std::vector<PropertyNode*> staticPropertyNodes;
	std::vector<FieldNode*> fieldNodes;
	std::vector<FieldNode*> staticFieldNodes;
	std::vector<MemberNode*> nestedTypeNodes;
	std::vector<MemberNode*> nestedTypeAliasNodes;

	classNode->m_memberList->collectMemberNodes(memberNodes);
	for (MemberNode* memberNode : memberNodes)
	{
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
		if (!memberNode->isNoMeta())
		{
			if (snt_method == memberNode->m_nodeType)
			{
				MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
				if (memberNode->m_name->m_str != classNode->m_name->m_str)
				{
					if (methodNode->isStatic())
					{
						if (methodNode->m_name->m_str == "__destroyInstance__"
							|| methodNode->m_name->m_str == "__destroyArray__"
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
			else if (snt_property == memberNode->m_nodeType)
			{
				PropertyNode* propertyNode = static_cast<PropertyNode*>(memberNode);
				if (propertyNode->isStatic())
				{
					propertyNode->m_orderIndex = staticPropertyNodes.size();
					staticPropertyNodes.push_back(propertyNode);
				}
				else
				{
					propertyNode->m_orderIndex = propertyNodes.size();
					propertyNodes.push_back(propertyNode);
				}
			}
			else if (snt_field == memberNode->m_nodeType)
			{
				FieldNode* fieldNode = static_cast<FieldNode*>(memberNode);
				if (fieldNode->isStatic())
				{
					fieldNode->m_orderIndex = staticFieldNodes.size();
					staticFieldNodes.push_back(fieldNode);
				}
				else
				{
					fieldNode->m_orderIndex = fieldNodes.size();
					fieldNodes.push_back(fieldNode);
				}
			}
			else if (snt_enum == memberNode->m_nodeType ||
				snt_class == memberNode->m_nodeType)
			{
				nestedTypeNodes.push_back(memberNode);
			}
			else if (snt_typedef == memberNode->m_nodeType ||
				snt_type_declaration == memberNode->m_nodeType)
			{
				nestedTypeAliasNodes.push_back(memberNode);
			}
			else
			{
				assert(false);
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


	writeMetaConstructor(classNode, templateArguments, nestedTypeNodes, nestedTypeAliasNodes,
		staticFieldNodes, staticPropertyNodes, staticMethodNodes,
		fieldNodes, propertyNodes, methodNodes, file, indentation);

	writeOverrideFunctions(classNode, templateArguments, typeMethodNodes, file, indentation);
	writeMetaPropertyImpls(classNode, templateArguments, propertyNodes, file, indentation);
	writeMetaPropertyImpls(classNode, templateArguments, staticPropertyNodes, file, indentation);
	writeMetaMethodImpls(classNode, templateArguments, methodNodes, false, file, indentation);
	writeMetaMethodImpls(classNode, templateArguments, staticMethodNodes, true, file, indentation);
	writeMetaGetSingletonImpls(classNode, templateArguments, file, indentation);

	if (classNode->needSubclassProxy(templateArguments))
	{
		generateCode_SubclassProxy(file, classNode, templateArguments, indentation);
	}

	for (MemberNode* typeNode : nestedTypeNodes)
	{
		switch (typeNode->m_nodeType)
		{
		case snt_enum:
			generateCode_Enum(file, static_cast<EnumNode*>(typeNode), templateArguments, indentation);
			break;
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(typeNode), templateClassInstance, indentation);
			break;
		default:
			assert(false);
		}
	}

	for (MemberNode* typeAliasNode : nestedTypeAliasNodes)
	{
		switch (typeAliasNode->m_nodeType)
		{
		case snt_typedef:
			generateCode_Typedef(file, static_cast<TypedefNode*>(typeAliasNode), templateArguments, indentation);
			break;
		case snt_type_declaration:
			generateCode_TypeDeclaration(file, static_cast<TypeDeclarationNode*>(typeAliasNode), templateArguments, indentation);
			break;
		default:
			assert(false);
		}
	}
}

const char* g_metaPropertyImplPrefix = "::pafcore::ErrorCode ";

const char* g_metaPropertyImplGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplSetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* value)\n";

const char* g_metaPropertyImplArrayGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t index, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplArraySetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t index, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplArraySizePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t& size)\n";
const char* g_metaPropertyImplArrayResizePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, size_t size)\n";

const char* g_metaPropertyImplListIteratePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* iterator)\n";
const char* g_metaPropertyImplListGetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplListSetPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplListInsertPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaPropertyImplListErasePostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Iterator* iterator)\n";

const char* g_metaPropertyImplEnumPostfix = "(::pafcore::InstanceProperty* instanceProperty, ::pafcore::Variant* that, ::pafcore::Variant* candidates)\n";


const char* g_metaStaticPropertyImplGetPostfix = "(::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplSetPostfix = "(::pafcore::Variant* value)\n";

const char* g_metaStaticPropertyImplArrayGetPostfix = "(size_t index, ::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplArraySetPostfix = "(size_t index, ::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplArraySizePostfix = "(size_t& size)\n";
const char* g_metaStaticPropertyImplArrayResizePostfix = "(size_t size)\n";

const char* g_metaStaticPropertyImplListIteratePostfix = "(::pafcore::Variant* iterator)\n";
const char* g_metaStaticPropertyImplListGetPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplListSetPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplListInsertPostfix = "(::pafcore::Iterator* iterator, ::pafcore::Variant* value)\n";
const char* g_metaStaticPropertyImplListErasePostfix = "(::pafcore::Iterator* iterator)\n";

const char* g_metaStaticPropertyImplEnumPostfix = "(::pafcore::Variant* candidates)\n";



void writeMetaPropertyItemImplHead(const char* metaClassName, ClassNode* classNode, PropertyNode* propertyNode, const char* item, const char* prefix, const char* postfix, FILE* file, int indentation)
{
	char funcName[256];
	sprintf_s(funcName, "%s::%s_%s_%s", metaClassName, classNode->m_name->m_str.c_str(), item, propertyNode->m_name->m_str.c_str());
	writeStringToFile(prefix, file, indentation);
	writeStringToFile(funcName, file);
	writeStringToFile(postfix, file);
}

std::string CalcCompoundTypeNativeName(const std::string& typeNativeName, TypeCompound m_typeCompound)
{
	std::string res;
	switch (m_typeCompound)
	{
	case tc_raw_ptr:
		res = typeNativeName + "*";
		break;
	case tc_shared_ptr:
		res = "::pafcore::SharedPtr<" + typeNativeName + ">";
		break;
	case tc_observer_ptr:
		res = "::pafcore::ObserverPtr<" + typeNativeName + ">";
		break;
		break;
	case tc_shared_array:
		res = "::pafcore::SharedArray<" + typeNativeName + ">";
		break;
	case tc_observer_array:
		res = "::pafcore::ObserverArray<" + typeNativeName + ">";
	default:
		assert(tc_none == m_typeCompound);
		res = typeNativeName;
	}
	return res;
}

bool writeMetaPropertyItemImplValueToArg(TemplateArguments* templateArguments, PropertyNode* propertyNode,  FILE* file, int indentation)
{
	char buf[4096];

	std::string typeName;
	std::string compoundTypeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, propertyNode->m_compoundType->m_typeName, templateArguments);
	compoundTypeName = CalcCompoundTypeNativeName(typeName, propertyNode->m_compoundType->m_typeCompound);

	bool argUsePtr = (tc_none == propertyNode->m_compoundType->m_typeCompound && class_type == typeKind);
	sprintf_s(buf, "%s%s arg;\n", compoundTypeName.c_str(), argUsePtr ? "*" : "");
	writeStringToFile(buf, file, indentation + 1);

	switch (propertyNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		switch (typeKind)
		{
		case primitive_type:
			sprintf_s(buf, "if(!value->castToPrimitive(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &arg))\n", typeName.c_str());
			break;
		case enum_type:
			sprintf_s(buf, "if(!value->castToEnum(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &arg))\n", typeName.c_str());
			break;
		case class_type:
			sprintf_s(buf, "if(!value->castToClassPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&arg))\n", typeName.c_str());
			break;
		}
		break;
	case tc_raw_ptr:
		sprintf_s(buf, "if(!value->castToRawPtr(arg))\n", typeName.c_str());
		break;
	case tc_shared_ptr:
		sprintf_s(buf, "if(!value->castToSharedPtr(arg))\n", typeName.c_str());
		break;
	case tc_observer_ptr:
		sprintf_s(buf, "if(!value->castToObserverPtr(arg))\n", typeName.c_str());
		break;
	case tc_shared_array:
		sprintf_s(buf, "if(!value->castToSharedArray(arg))\n", typeName.c_str());
		break;
	case tc_observer_array:
		sprintf_s(buf, "if(!value->castToObserverArray(arg))\n", typeName.c_str());
		break;
	}
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("{\n", file, indentation + 1);
	sprintf_s(buf, "return ::pafcore::e_invalid_arg_type_%d;\n", propertyNode->isSimple() ? 1 : 2);
	writeStringToFile(buf, file, indentation + 2);
	writeStringToFile("}\n", file, indentation + 1);

	return argUsePtr;
}

void writeMetaPropertyGetImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	char strCall[1024];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = "";
	if (propertyNode->isStatic())
	{
		switch (propertyNode->getKind())
		{
		case simple_property:
			postfix = g_metaStaticPropertyImplGetPostfix;
			break;
		case fixed_array_property:
		case dynamic_array_property:
			postfix = g_metaStaticPropertyImplArrayGetPostfix;
		case list_property:
			postfix = g_metaStaticPropertyImplListGetPostfix;
			break;
		}
	}
	else
	{
		switch (propertyNode->getKind())
		{
		case simple_property:
			postfix = g_metaPropertyImplGetPostfix;
			break;
		case fixed_array_property:
		case dynamic_array_property:
			postfix = g_metaPropertyImplArrayGetPostfix;
		case list_property:
			postfix = g_metaPropertyImplListGetPostfix;
			break;
		}
	}

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "get", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::string typeName;
	std::string compoundTypeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, propertyNode->m_compoundType->m_typeName, templateArguments);
	compoundTypeName = CalcCompoundTypeNativeName(typeName, propertyNode->m_compoundType->m_typeCompound);

	const char* extParam = "index";
	if (propertyNode->isSimple())
	{
		extParam = "";
	}
	else if (propertyNode->isList())
	{
		extParam = "iterator";
	}

	if (propertyNode->isStatic())
	{
		if (propertyNode->m_get->m_nativeName)
		{
			sprintf_s(strCall, "%s(", propertyNode->m_get->m_nativeName->m_str.c_str());
		}
		else
		{
			sprintf_s(strCall, "%s::%s(", className.c_str(), propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s res = %s%s);\n", compoundTypeName.c_str(), strCall, extParam);
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);

		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);

		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);

		if (propertyNode->m_get->m_nativeName)
		{
			if (propertyNode->m_get->m_nativeName->m_str.find(':') != std::string::npos)
			{
				sprintf_s(strCall, "%s(self%s", propertyNode->m_get->m_nativeName->m_str.c_str(),
					propertyNode->isSimple() ? "" : ", ");
			}
			else
			{
				sprintf_s(strCall, "self->%s(", propertyNode->m_get->m_nativeName->m_str.c_str());
			}
		}
		else
		{
			sprintf_s(strCall, "self->%s(", propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s res = %s%s);\n", compoundTypeName.c_str(), strCall, extParam);
		writeStringToFile(buf, file, indentation + 1);
	}

	const char* func = "";
	bool move = false;
	switch (propertyNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		switch (typeKind)
		{
		case primitive_type:
			func = "assignPrimitive";
			break;
		case enum_type:
			func = "assignEnum";
			break;
		case class_type:
			func = "assignClass";
			break;
		}
		break;
	case tc_raw_ptr:
		func = "assignRawPtr";
		break;
	case tc_shared_ptr:
		func = "assignSharedPtr";
		move = true;
		break;
	case tc_observer_ptr:
		func = "assignObserverPtr";
		break;
	case tc_shared_array:
		func = "assignSharedArray";
		move = true;
		break;
	case tc_observer_array:
		func = "assignObserverArray";
		break;
	}
	if (move)
	{
		sprintf_s(buf, "value->%s(std::move(res));\n", func, typeName.c_str());
	}
	else
	{
		sprintf_s(buf, "value->%s(res);\n", func, typeName.c_str());
	}
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertySetImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	char strCall[1024];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = "";
	if (propertyNode->isStatic())
	{
		switch (propertyNode->getKind())
		{
		case simple_property:
			postfix = g_metaStaticPropertyImplSetPostfix;
			break;
		case fixed_array_property:
		case dynamic_array_property:
			postfix = g_metaStaticPropertyImplArraySetPostfix;
		case list_property:
			postfix = g_metaStaticPropertyImplListSetPostfix;
			break;
		}
	}
	else
	{
		switch (propertyNode->getKind())
		{
		case simple_property:
			postfix = g_metaPropertyImplSetPostfix;
			break;
		case fixed_array_property:
		case dynamic_array_property:
			postfix = g_metaPropertyImplArraySetPostfix;
		case list_property:
			postfix = g_metaPropertyImplListSetPostfix;
			break;
		}
	}

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "set", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	bool argUsePtr = writeMetaPropertyItemImplValueToArg(templateArguments, propertyNode, file, indentation);

	const char* extParam = "index, ";
	if (propertyNode->isSimple())
	{
		extParam = "";
	}
	else if (propertyNode->isList())
	{
		extParam = "iterator, ";
	}

	if (propertyNode->isStatic())
	{
		if (propertyNode->m_set->m_nativeName)
		{
			sprintf_s(strCall, "%s(", propertyNode->m_set->m_nativeName->m_str.c_str());
		}
		else
		{
			sprintf_s(strCall, "%s::%s(", className.c_str(), propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s%sarg);\n", strCall, extParam);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);

		if (propertyNode->m_set->m_nativeName)
		{
			if (propertyNode->m_set->m_nativeName->m_str.find(':') != std::string::npos)
			{
				sprintf_s(strCall, "%s(self%s", propertyNode->m_set->m_nativeName->m_str.c_str(),
					propertyNode->isSimple() ? "" : ", ");
			}
			else
			{
				sprintf_s(strCall, "self->%s(", propertyNode->m_set->m_nativeName->m_str.c_str());
			}
		}
		else
		{
			sprintf_s(strCall, "self->%s(", propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s& res = %s%s%sarg);\n", className.c_str(), strCall, extParam, argUsePtr ? "*" : "");
	}
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertyEnumImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	char strCall[1024];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplEnumPostfix : g_metaPropertyImplEnumPostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "enum", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::string typeName;
	std::string compoundTypeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, propertyNode->m_compoundType->m_typeName, templateArguments);
	compoundTypeName = CalcCompoundTypeNativeName(typeName, tc_shared_array);

	if (propertyNode->isStatic())
	{
		if (propertyNode->m_enumerate->m_nativeName)
		{
			sprintf_s(strCall, "%s(", propertyNode->m_enumerate->m_nativeName->m_str.c_str());
		}
		else
		{
			sprintf_s(strCall, "%s::enum_%s(", className.c_str(), propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s res = %s);\n", compoundTypeName.c_str(), strCall);
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);

		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);

		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);

		if (propertyNode->m_enumerate->m_nativeName)
		{
			if (propertyNode->m_enumerate->m_nativeName->m_str.find(':') != std::string::npos)
			{
				sprintf_s(strCall, "%s(self%s", propertyNode->m_enumerate->m_nativeName->m_str.c_str(),
					propertyNode->isSimple() ? "" : ", ");
			}
			else
			{
				sprintf_s(strCall, "self->%s(", propertyNode->m_enumerate->m_nativeName->m_str.c_str());
			}
		}
		else
		{
			sprintf_s(strCall, "self->enum_%s(", propertyNode->m_name->m_str.c_str());
		}
		sprintf_s(buf, "%s res = %s);\n", compoundTypeName.c_str(), strCall);
		writeStringToFile(buf, file, indentation + 1);
	}

	sprintf_s(buf, "candidates->moveSharedArray(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &res);\n", typeName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertySizeImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplArraySizePostfix : g_metaPropertyImplArraySizePostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "size", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	if (propertyNode->isStatic())
	{
		sprintf_s(buf, "size = size_%s();\n", propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
		sprintf_s(buf, "size = self->size_%s();\n", propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	//writeStringToFile("size->assignPrimitive(RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), &res);\n", file, indentation + 1);
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertyResizeImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplArrayResizePostfix : g_metaPropertyImplArrayResizePostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "resize", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	if (propertyNode->isStatic())
	{
		sprintf_s(buf, "%s::resize_%s(size);\n", className.c_str(), propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
		sprintf_s(buf, "self->resize_%s(size);\n", propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertyIterateImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplListIteratePostfix : g_metaPropertyImplListIteratePostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "iterate", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);


	if (propertyNode->isStatic())
	{
		sprintf_s(buf, "::pafcore::SharedPtr<::pafcore::Iterator> res = %s::insert_%s();\n", className.c_str(), propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
		sprintf_s(buf, "::pafcore::SharedPtr<::pafcore::Iterator> res = self->iterate_%s();\n", propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("iterator->assignSharedPtr(std::move(res));\n", file, indentation + 1);
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaPropertyInsertImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplListInsertPostfix : g_metaPropertyImplListInsertPostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "insert", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	bool argUsePtr = writeMetaPropertyItemImplValueToArg(templateArguments, propertyNode, file, indentation);

	if (propertyNode->isStatic())
	{
		sprintf_s(buf, "%s::insert_%s(iterator, arg);\n", className.c_str(), propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
		sprintf_s(buf, "self->insert_%s(iterator, %sarg);\n", propertyNode->m_name->m_str.c_str(), argUsePtr ? "*" : "");
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}


void writeMetaPropertyEraseImpl(ClassNode* classNode, TemplateArguments* templateArguments, PropertyNode* propertyNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string className;
	std::string metaClassName;

	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	const char* prefix = g_metaPropertyImplPrefix;
	const char* postfix = propertyNode->isStatic() ? g_metaStaticPropertyImplListInsertPostfix : g_metaPropertyImplListInsertPostfix;

	writeMetaPropertyItemImplHead(metaClassName.c_str(), classNode, propertyNode, "earse", prefix, postfix, file, indentation);
	writeStringToFile("{\n", file, indentation);

	if (propertyNode->isStatic())
	{
		sprintf_s(buf, "%s::earse_%s(iterator);\n", className.c_str(), propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "%s* self;\n", className.c_str());
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("if(!that->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
		sprintf_s(buf, "self->earse_%s(iterator);\n", propertyNode->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}


void writeMetaPropertyImpls(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<PropertyNode*>& propertyNodes, FILE* file, int indentation)
{
	for (PropertyNode* propertyNode : propertyNodes)
	{
		if (0 != propertyNode->m_get)
		{
			writeMetaPropertyGetImpl(classNode, templateArguments, propertyNode, file, indentation);
		}
		if (0 != propertyNode->m_set)
		{
			writeMetaPropertySetImpl(classNode, templateArguments, propertyNode, file, indentation);
		}
		if (propertyNode->m_enumerate)
		{
			writeMetaPropertyEnumImpl(classNode, templateArguments, propertyNode, file, indentation);
		}
		else if (propertyNode->isFixedArray() || propertyNode->isDynamicArray())
		{
			writeMetaPropertySizeImpl(classNode, templateArguments, propertyNode, file, indentation);
			if (propertyNode->isDynamicArray())
			{
				writeMetaPropertyResizeImpl(classNode, templateArguments, propertyNode, file, indentation);
			}
		}
		else if (propertyNode->isList())
		{
			writeMetaPropertyIterateImpl(classNode, templateArguments, propertyNode, file, indentation);
			writeMetaPropertyInsertImpl(classNode, templateArguments, propertyNode, file, indentation);
			writeMetaPropertyEraseImpl(classNode, templateArguments, propertyNode, file, indentation);
		}
	}
}

const char g_metaMethodImplPrefix[] = "::pafcore::ErrorCode ";
const char g_metaMethodImplPostfix[] = "(::pafcore::Variant** results, int_t numResults, ::pafcore::Variant** arguments, int_t numArguments)\n";

void writeMetaMethodImpl_CastSelf(ClassNode* classNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	std::string className;
	classNode->getNativeName(className, templateArguments);
	writeStringToFile(className.c_str(), file, indentation);
	writeStringToFile("* self;\n", file, 0);
	writeStringToFile("if(!arguments[0]->castToClassPtr(GetSingleton(), (void**)&self))\n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeStringToFile("return ::pafcore::e_invalid_this_type;\n", file, indentation + 1);
	writeStringToFile("}\n", file, indentation);
}

void writeMetaMethodImpl_InitInputParam(ClassNode* classNode, TemplateArguments* templateArguments, VariableNode* parameterNode, uint32_t argIndex, uint32_t paramIndex, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	std::string compoundTypeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, parameterNode->m_compoundType->m_typeName, templateArguments);
	compoundTypeName = CalcCompoundTypeNativeName(typeName, parameterNode->m_compoundType->m_typeCompound);

	bool usePtr = parameterNode->m_compoundType->isNotPtr() && class_type == typeKind;
	sprintf_s(buf, "%s%s a%u;\n", compoundTypeName.c_str(), usePtr ? "*" : "", paramIndex);
	writeStringToFile(buf, file, indentation);
	switch (parameterNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		switch (typeKind)
		{
		case primitive_type:
			sprintf_s(buf, "arguments[%u]->castToPrimitive<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
			break;
		case enum_type:
			sprintf_s(buf, "arguments[%u]->castToEnum<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
			break;
		case class_type:
			sprintf_s(buf, "arguments[%u]->castToClassPtr<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
			break;
		default:
			sprintf_s(buf, "invalid type kind");
			assert(false);
		}
		break;
	case tc_raw_ptr:
		sprintf_s(buf, "arguments[%u]->castToRawPtr<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
		break;
	case tc_shared_ptr:
		sprintf_s(buf, "arguments[%u]->castToSharedPtr<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
		break;
	case tc_observer_ptr:
		sprintf_s(buf, "arguments[%u]->castToObserverPtr<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
		break;
	case tc_shared_array:
		sprintf_s(buf, "arguments[%u]->castToSharedArray<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
		break;
	case tc_observer_array:
		sprintf_s(buf, "arguments[%u]->castToObserverArray<%s>(a%u)", argIndex, typeName.c_str(), paramIndex);
		break;
	default:
		sprintf_s(buf, "invalid type compound");
		assert(false);
	}

	writeStringToFile("if(!", file, indentation);
	writeStringToFile(buf, file, 0);
	writeStringToFile(")\n", file, 0);
	writeStringToFile("{\n", file, indentation);
	sprintf_s(buf, "return ::pafcore::e_invalid_arg_type_%u;\n", paramIndex + 1);
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("}\n", file, indentation);
}

void writeMetaMethodImpl_InitOutputParam(ClassNode* classNode, TemplateArguments* templateArguments, VariableNode* parameterNode, uint32_t resultIndex, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	std::string compoundTypeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, parameterNode->m_compoundType->m_typeName, templateArguments);
	compoundTypeName = CalcCompoundTypeNativeName(typeName, parameterNode->m_compoundType->m_typeCompound);
	sprintf_s(buf, "%s r%u;\n", compoundTypeName.c_str(), resultIndex);
	writeStringToFile(buf, file, indentation);
}

void writeMetaMethodImpl_UseInputParam(ClassNode* classNode, TemplateArguments* templateArguments, VariableNode* parameterNode, uint32_t paramIndex, bool firstParam, FILE* file)
{
	char buf[4096];
	std::string typeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, parameterNode->m_compoundType->m_typeName, templateArguments);
	bool usePtr = parameterNode->m_compoundType->isNotPtr() && class_type == typeKind;

	sprintf_s(buf, "%sa%u", usePtr ? "*" : "", paramIndex);
	if (!firstParam)
	{
		writeStringToFile(", ", file, 0);
	}
	writeStringToFile(buf, file, 0);
}

void writeMetaMethodImpl_UseOutputParam(ClassNode* classNode, TemplateArguments* templateArguments, VariableNode* resultNode, uint32_t resultIndex, bool firstParam, FILE* file)
{
	char buf[4096];
	sprintf_s(buf, "r%u", resultIndex);
	if (!firstParam)
	{
		writeStringToFile(", ", file, 0);
	}
	writeStringToFile(buf, file, 0);
}

void writeMetaMethodImpl_AssignResult(VariableNode* resultNode, uint32_t resultIndex, FILE* file, int indentation)
{
	char buf[4096];
	
	switch (resultNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		sprintf_s(buf, "results[%u]->assignValue(r%u);\n", resultIndex, resultIndex);
		break;
	case tc_raw_ptr:
		sprintf_s(buf, "results[%u]->assignRawPtr(r%u);\n", resultIndex, resultIndex);
		break;
	case tc_shared_ptr:
		sprintf_s(buf, "results[%u]->assignSharedPtr(std::move(r%u));\n", resultIndex, resultIndex);
		break;
	case tc_observer_ptr:
		sprintf_s(buf, "results[%u]->assignObserverPtr(r%u);\n", resultIndex, resultIndex);
		break;
	case tc_shared_array:
		sprintf_s(buf, "results[%u]->assignSharedArray(std::move(r%u));\n", resultIndex, resultIndex);
		break;
	case tc_observer_array:
		sprintf_s(buf, "results[%u]->assignObserverArray(r%u);\n", resultIndex, resultIndex);
		break;
	default:
		sprintf_s(buf, "invalid type compound");
		assert(false);
	}
	writeStringToFile(buf, file, indentation);
}

void writeMetaMethodImpl_Call(ClassNode* classNode, TemplateArguments* templateArguments, MethodNode* methodNode, std::vector<VariableNode*>& resultNodes, std::vector<VariableNode*>& parameterNodes, FILE* file, int indentation)
{
	char buf[4096];

	IdentifierNode* methodNameNode = methodNode->m_nativeName ? methodNode->m_nativeName : methodNode->m_name;

	uint32_t resultCount = (uint32_t)resultNodes.size();
	uint32_t outputParamCount = resultCount > 1 ? resultCount - 1 : 0;
	uint32_t inputParamCount = (uint32_t)parameterNodes.size();
	uint32_t paramCount = outputParamCount + inputParamCount;

	if (0 == resultCount)
	{
		writeStringToFile("", 0, file, indentation);
	}
	else
	{
		VariableNode* resultNode = resultNodes.front();
		std::string typeName;
		std::string compoundTypeName;
		TypeKind typeKind = CalcTypeNativeName(typeName, resultNode->m_compoundType->m_typeName, templateArguments);
		compoundTypeName = CalcCompoundTypeNativeName(typeName, resultNode->m_compoundType->m_typeCompound);
		sprintf_s(buf, "%s%s r0 = ", compoundTypeName.c_str(), resultNode->isByRef() ? "&" : "");
		writeStringToFile(buf, file, indentation);
	}
	if (methodNode->isStatic())
	{
		if (methodNode->m_nativeName)
		{
			sprintf_s(buf, "%s(", methodNameNode->m_str.c_str());
		}
		else
		{
			std::string className;
			classNode->getNativeName(className, templateArguments);
			sprintf_s(buf, "%s::%s(", className.c_str(), methodNameNode->m_str.c_str());
		}
	}
	else
	{
		if (methodNameNode->m_str.find(':') != std::string::npos)
		{
			sprintf_s(buf, "%s(self%s", methodNameNode->m_str.c_str(), 0 == paramCount ? "" : ", ");
		}
		else
		{
			sprintf_s(buf, "self->%s(", methodNameNode->m_str.c_str());
		}
	}
	writeStringToFile(buf, file, 0);

	bool firstParam = true;
	for (uint32_t i = 1; i < resultCount; ++i)
	{
		VariableNode* resultNode = resultNodes[i];
		writeMetaMethodImpl_UseOutputParam(classNode, templateArguments, resultNode, i, firstParam, file);
		firstParam = false;
	}
	for (uint32_t i = 0; i < inputParamCount; ++i)
	{
		VariableNode* parameterNode = parameterNodes[i];
		writeMetaMethodImpl_UseInputParam(classNode, templateArguments, parameterNode, i, firstParam, file);
		firstParam = false;
	}

	writeStringToFile(");\n", file, 0);
}

void writeMetaMethodImpl_OneOverload(ClassNode* classNode, TemplateArguments* templateArguments,
	MethodNode* methodNode, bool isStatic, bool noOverloads, FILE* file, int indentation)
{
	assert(methodNode->isStatic() == isStatic);

	std::vector<VariableNode*> parameterNodes;
	methodNode->m_parameterList->collectVariableNodes(parameterNodes);
	uint32_t paramCount = (uint32_t)parameterNodes.size();

	for (uint32_t i = 0; i < paramCount; ++i)
	{
		VariableNode* parameterNode = parameterNodes[i];
		uint32_t argIndex = isStatic ? i : i + 1;
		writeMetaMethodImpl_InitInputParam(classNode, templateArguments, parameterNode, argIndex, i, file, indentation);
	}
	std::vector<VariableNode*> resultNodes;
	methodNode->m_resultList->collectVariableNodes(resultNodes);
	uint32_t resultCount = (uint32_t)resultNodes.size();
	for (uint32_t i = 1; i < resultCount; ++i)
	{
		VariableNode* outputNode = resultNodes[i];
		writeMetaMethodImpl_InitOutputParam(classNode, templateArguments, outputNode, i, file, indentation);
	}

	writeMetaMethodImpl_Call(classNode, templateArguments, methodNode, resultNodes, parameterNodes, file, indentation);
	for (uint32_t i = 0; i < resultCount; ++i)
	{
		VariableNode* resultNode = resultNodes[i];
		writeMetaMethodImpl_AssignResult(resultNode, i, file, indentation);
	}
	writeStringToFile("return ::pafcore::s_ok;\n", file, indentation);
}

void writeMetaMethodImpl_SameParamCount(
	ClassNode* classNode, 
	TemplateArguments* templateArguments,
	std::vector<MethodNode*>::iterator begin, 
	std::vector<MethodNode*>::iterator end,
	bool isStatic, 
	uint32_t methodIndex, 
	uint32_t overloadIndex, 
	uint32_t maxParamCount, 
	FILE* file, 
	int indentation)
{
	char buf[4096];

	uint32_t overloadCount = end - begin;
	uint32_t paramCount = (*begin)->getParameterCount();
	uint32_t argCount = isStatic ? paramCount : paramCount + 1;

	sprintf_s(buf, "if(%u %s numArguments)\n", argCount, (paramCount == maxParamCount && !g_options.m_strictArgumentsCount) ? "<=" : "==");
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	if (1 == overloadCount)
	{
		writeMetaMethodImpl_OneOverload(classNode, templateArguments, *begin, isStatic, true, file, indentation + 1);
	}
	else
	{
		sprintf_s(buf, "uint32_t matched = ::pafcore::Overload::Resolve(&GetSingleton()->%s[%u].m_overloads[%u], %u, arguments, %u);\n",
			isStatic ? "m_staticMethods" : "m_instanceMethods", methodIndex, overloadIndex, overloadCount, argCount);
		writeStringToFile(buf, file, indentation + 1);
		sprintf_s(buf, "switch(matched)\n");
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("{\n", file, indentation + 1);
		for (uint32_t i = 0; i < overloadCount; ++i)
		{
			sprintf_s(buf, "case %u:\n", i);
			writeStringToFile(buf, file, indentation + 1);
			writeStringToFile("{\n", file, indentation + 1);
			writeMetaMethodImpl_OneOverload(classNode, templateArguments, *(begin + i), isStatic, true, file, indentation + 2);
			writeStringToFile("}\n", file, indentation + 1);
		}
		sprintf_s(buf, "case %u:\n", overloadCount);
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("return ::pafcore::e_no_match_overload;\n", file, indentation + 2);
		writeStringToFile("default:", file, indentation + 1);
		writeStringToFile("return ::pafcore::e_ambiguous_overload;\n", file, indentation + 2);
		writeStringToFile("}\n", file, indentation + 1);
	}

	writeStringToFile("}\n", file, indentation);
}

void writeMetaMethodImpl(
	ClassNode* classNode, 
	TemplateArguments* templateArguments,
	const char* funcName, 
	std::vector<MethodNode*>::iterator begin, 
	std::vector<MethodNode*>::iterator end,
	bool isStatic, 
	uint32_t methodIndex, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	std::string typeName;
	std::string metaClassName;
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);

	writeStringToFile(g_metaMethodImplPrefix, sizeof(g_metaMethodImplPrefix) - 1, file, indentation);
	sprintf_s(buf, "%s::%s_%s", metaClassName.c_str(),
		classNode->m_name->m_str.c_str(), funcName);
	writeStringToFile(buf, file);
	writeStringToFile(g_metaMethodImplPostfix, sizeof(g_metaMethodImplPostfix) - 1, file);
	writeStringToFile("{\n", file, indentation);

	if (!isStatic)
	{
		writeMetaMethodImpl_CastSelf(classNode, templateArguments, file, indentation + 1);
	}
	std::vector<MethodNode*>::iterator first = begin;
	std::vector<MethodNode*>::iterator last = begin;
	uint32_t maxParamCount = (*(end - 1))->getParameterCount();
	uint32_t overloadIndex = 0;
	for (; first != end;)
	{
		++last;
		if (last == end || (*last)->getParameterCount() != (*first)->getParameterCount())
		{
			writeMetaMethodImpl_SameParamCount(classNode, templateArguments, first, last, isStatic, methodIndex, overloadIndex, maxParamCount, file, indentation + 1);
			overloadIndex += last - first;
			first = last;
		}
	}
	writeStringToFile("return ::pafcore::e_invalid_arg_num;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaMethodImpls(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<MethodNode*>& methodNodes, bool isStatic, FILE* file, int indentation)
{
	std::vector<MethodNode*>::iterator first = methodNodes.begin();
	std::vector<MethodNode*>::iterator last = first;
	std::vector<MethodNode*>::iterator end = methodNodes.end();
	uint32_t methodIndex = 0;
	for (; first != end;)
	{
		++last;
		if (last == end || (*last)->m_name->m_str != (*first)->m_name->m_str)
		{
			writeMetaMethodImpl(classNode, templateArguments, (*first)->m_name->m_str.c_str(), first, last, isStatic, methodIndex, file, indentation);
			++methodIndex;
			first = last;
		}
	}
}

void writeMetaConstructor_attributesForType(
	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets, 
	SyntaxNodeImpl* memberNode, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	auto it = attributesOffsets.find(memberNode);
	if (it != attributesOffsets.end())
	{
		sprintf(buf, "m_attributes = &s_attributeses[%u];\n", it->second);
		writeStringToFile(buf, file, indentation);
	}
}

struct AttributeOffsetAndCount
{
	SyntaxNodeImpl* node;
	uint32_t offset;
	uint32_t count;
};

template<typename T>
void writeMetaConstructor_Attributes_Member(
	std::vector<AttributeOffsetAndCount>& attributeOffsetAndCounts, 
	uint32_t& offset,
	T* memberNode, FILE* file, int indentation)
{
	AttributeListNode* attributeList = memberNode->m_attributeList;
	if (attributeList)
	{
		std::vector<AttributeNode*> attributeNodes;
		attributeList->collectAttributeNodes(attributeNodes);
		std::sort(attributeNodes.begin(), attributeNodes.end(), CompareAttributePtr());
		uint32_t count = static_cast<uint32_t>(attributeNodes.size());
		for (uint32_t i = 0; i < count; ++i)
		{
			AttributeNode* attributeNode = attributeNodes[i];
			writeStringToFile("{ \"", file, indentation);
			writeStringToFile(attributeNode->m_name->m_str.c_str(), file);
			if (attributeNode->m_u8content)
			{
				writeStringToFile("\", u8\"", file);
			}
			else
			{
				writeStringToFile("\", \"", file);
			}
			if (attributeNode->m_content)
			{
				writeStringToFile(attributeNode->m_content->m_str.c_str(), file);
			}
			writeStringToFile("\" },\n", file);
		}
		AttributeOffsetAndCount aoac;
		aoac.node = memberNode;
		aoac.offset = offset;
		aoac.count = count;
		attributeOffsetAndCounts.push_back(aoac);
		offset += count;
	}
}

void writeMetaConstructor_Attributes_Method(
	std::vector<AttributeOffsetAndCount>& attributeOffsetAndCounts, 
	uint32_t& offset,
	std::vector<MethodNode*>::iterator first, 
	std::vector<MethodNode*>::iterator last, 
	FILE* file, 
	int indentation)
{
	std::vector<AttributeNode*> attributeNodes;
	for (auto it = first; it != last; ++it)
	{
		MethodNode* methodNode = *it;
		AttributeListNode* attributeList = methodNode->m_attributeList;
		if (attributeList)
		{
			attributeList->collectAttributeNodes(attributeNodes);
		}
	}
	if (!attributeNodes.empty())
	{
		std::sort(attributeNodes.begin(), attributeNodes.end(), CompareAttributePtr());
		uint32_t count = static_cast<uint32_t>(attributeNodes.size());
		for (uint32_t i = 0; i < count; ++i)
		{
			AttributeNode* attributeNode = attributeNodes[i];
			writeStringToFile("{ \"", file, indentation);
			writeStringToFile(attributeNode->m_name->m_str.c_str(), file);
			writeStringToFile("\", u8\"", file);
			if (attributeNode->m_content)
			{
				writeStringToFile(attributeNode->m_content->m_str.c_str(), file);
			}
			writeStringToFile("\" },\n", file);
		}
		AttributeOffsetAndCount aoac;
		aoac.node = *first;
		aoac.offset = offset;
		aoac.count = count;
		attributeOffsetAndCounts.push_back(aoac);
		offset += count;
	}
}

template<typename T>
void writeMetaConstructor_Attributes_Members(std::vector<AttributeOffsetAndCount>& attributeOffsetAndCounts, uint32_t& offset,
	std::vector<T*>& memberNodes, FILE* file, int indentation)
{
	for (T* node : memberNodes)
	{
		writeMetaConstructor_Attributes_Member(attributeOffsetAndCounts, offset, node, file, indentation);
	}
}

void writeMetaConstructor_Attributes_Methods(std::vector<AttributeOffsetAndCount>& attributeOffsetAndCounts, uint32_t& offset,
	std::vector<MethodNode*>& methodNodes, FILE* file, int indentation)
{
	std::vector<MethodNode*>::iterator begin = methodNodes.begin();
	std::vector<MethodNode*>::iterator end = methodNodes.end();
	std::vector<MethodNode*>::iterator first = begin;
	std::vector<MethodNode*>::iterator last = begin;
	for (; first != end;)
	{
		++last;
		if (last == end || (*last)->m_name->m_str != (*first)->m_name->m_str)
		{
			writeMetaConstructor_Attributes_Method(attributeOffsetAndCounts, offset, first, last, file, indentation);
			first = last;
		}
	}
}

template<typename T>
bool noAttributes(std::vector<T*>& memberNodes)
{
	for (T* node : memberNodes)
	{
		AttributeListNode* attributeList = node->m_attributeList;
		if (attributeList)
		{
			return false;
		}
	}
	return true;
}


void writeMetaConstructor_Attributeses(
	std::map<SyntaxNodeImpl*, uint32_t>& attributessOffsets,
	ClassNode* classNode,
	std::vector<FieldNode*>& staticFieldNodes,
	std::vector<PropertyNode*>& staticPropertyNodes,
	std::vector<MethodNode*>& staticMethodNodes,
	std::vector<FieldNode*>& fieldNodes,
	std::vector<PropertyNode*>& propertyNodes,
	std::vector<MethodNode*>& methodNodes,
	FILE* file, int indentation)
{
	char buf[4096];
	if (0 == classNode->m_attributeList
		&& noAttributes(staticFieldNodes)
		&& noAttributes(staticPropertyNodes)
		&& noAttributes(staticMethodNodes)
		&& noAttributes(fieldNodes)
		&& noAttributes(propertyNodes)
		&& noAttributes(methodNodes))
	{
		return;
	}

	uint32_t offset = 0;
	std::vector<AttributeOffsetAndCount> attributeOffsetAndCounts;

	writeStringToFile("static ::pafcore::Attribute s_attributes[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);

	writeMetaConstructor_Attributes_Member(attributeOffsetAndCounts, offset, classNode, file, indentation + 1);
	writeMetaConstructor_Attributes_Members(attributeOffsetAndCounts, offset, staticFieldNodes, file, indentation + 1);
	writeMetaConstructor_Attributes_Members(attributeOffsetAndCounts, offset, staticPropertyNodes, file, indentation + 1);
	writeMetaConstructor_Attributes_Methods(attributeOffsetAndCounts, offset, staticMethodNodes, file, indentation + 1);
	writeMetaConstructor_Attributes_Members(attributeOffsetAndCounts, offset, fieldNodes, file, indentation + 1);
	writeMetaConstructor_Attributes_Members(attributeOffsetAndCounts, offset, propertyNodes, file, indentation + 1);
	writeMetaConstructor_Attributes_Methods(attributeOffsetAndCounts, offset, methodNodes, file, indentation + 1);
	writeStringToFile("};\n", file, indentation);

	uint32_t size = (uint32_t)attributeOffsetAndCounts.size();

	writeStringToFile("static ::pafcore::Attributes s_attributeses[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	for (uint32_t i = 0; i < size; ++i)
	{
		AttributeOffsetAndCount& aoac = attributeOffsetAndCounts[i];
		sprintf(buf, "{ %u, &s_attributes[%u] },\n", aoac.count, aoac.offset);
		writeStringToFile(buf, file, indentation + 1);
		attributessOffsets.insert(std::make_pair(aoac.node, i));
	}
	writeStringToFile("};\n", file, indentation);
}

void writeEnumMetaConstructor_Attributeses(
	std::map<SyntaxNodeImpl*, uint32_t>& attributessOffsets,
	EnumNode* enumNode, std::vector<EnumeratorNode*>& enumeratorNodes,
	FILE* file, int indentation)
{
	char buf[4096];
	if (0 == enumNode->m_attributeList
		&& noAttributes(enumeratorNodes))
	{
		return;
	}
	uint32_t offset = 0;
	std::vector<AttributeOffsetAndCount> attributeOffsetAndCounts;
	writeStringToFile("static ::pafcore::Attribute s_attributes[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeMetaConstructor_Attributes_Member(attributeOffsetAndCounts, offset, enumNode, file, indentation + 1);
	writeMetaConstructor_Attributes_Members(attributeOffsetAndCounts, offset, enumeratorNodes, file, indentation + 1);
	writeStringToFile("};\n", file, indentation);

	uint32_t size = static_cast<uint32_t>(attributeOffsetAndCounts.size());

	writeStringToFile("static ::pafcore::Attributes s_attributeses[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	for (uint32_t i = 0; i < size; ++i)
	{
		AttributeOffsetAndCount& aoac = attributeOffsetAndCounts[i];
		sprintf(buf, "{ %u, &s_attributes[%u] },\n", aoac.count, aoac.offset);
		writeStringToFile(buf, file, indentation + 1);
		attributessOffsets.insert(std::make_pair(aoac.node, i));
	}
	writeStringToFile("};\n", file, indentation);
}

void writeTypeDefMetaConstructor_Attributeses(
	std::map<SyntaxNodeImpl*, uint32_t>& attributessOffsets,
	MemberNode* memberNode, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	if (0 == memberNode->m_attributeList)
	{
		return;
	}
	uint32_t offset = 0;
	std::vector<AttributeOffsetAndCount> attributeOffsetAndCounts;
	writeStringToFile("static ::pafcore::Attribute s_attributes[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeMetaConstructor_Attributes_Member(attributeOffsetAndCounts, offset, memberNode, file, indentation + 1);
	writeStringToFile("};\n", file, indentation);

	uint32_t size = static_cast<uint32_t>(attributeOffsetAndCounts.size());

	writeStringToFile("static ::pafcore::Attributes s_attributeses[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	for (uint32_t i = 0; i < size; ++i)
	{
		AttributeOffsetAndCount& aoac = attributeOffsetAndCounts[i];
		sprintf(buf, "{ %u, &s_attributes[%u] },\n", aoac.count, aoac.offset);
		writeStringToFile(buf, file, indentation + 1);
		attributessOffsets.insert(std::make_pair(aoac.node, i));
	}
	writeStringToFile("};\n", file, indentation);
}

void writeMetaConstructor_Fields(
	ClassNode* classNode, 
	TemplateArguments* templateArguments, 
	std::map<SyntaxNodeImpl*, uint32_t>& attributesOffsets,
	std::vector<FieldNode*>& fieldNodes, 
	bool isStatic, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	char strAttributes[256];
	if (fieldNodes.empty())
	{
		return;
	}
	std::string className;
	classNode->getNativeName(className, templateArguments);
	uint32_t count = static_cast<uint32_t>(fieldNodes.size());

	if (isStatic)
	{
		writeStringToFile("static ::pafcore::StaticField s_staticFields[] = \n", file, indentation);
	}
	else
	{
		writeStringToFile("static ::pafcore::InstanceField s_instanceFields[] = \n", file, indentation);
	}
	writeStringToFile("{\n", file, indentation);

	for (FieldNode* fieldNode : fieldNodes)
	{
		if (fieldNode->m_attributeList)
		{
			auto it = attributesOffsets.find(fieldNode);
			assert(it != attributesOffsets.end());
			sprintf_s(strAttributes, "&s_attributeses[%u]", it->second);
		}
		else
		{
			strcpy_s(strAttributes, "0");
		}

		IdentifierNode* fieldNameNode = fieldNode->m_nativeName ? fieldNode->m_nativeName : fieldNode->m_name;
		char arraySize[512];
		if (fieldNode->isArray())
		{
			sprintf_s(arraySize, "paf_field_array_size_of(%s, %s)", className.c_str(), fieldNameNode->m_str.c_str());
		}
		else
		{
			strcpy_s(arraySize, "0");
		}
		std::string typeName;
		TypeKind typeKind = CalcTypeNativeName(typeName, fieldNode->m_compoundType->m_typeName, templateArguments);
		const char* typeCompound = TypeCompoundString(fieldNode->m_compoundType);
		if (isStatic)
		{
			sprintf_s(buf, "::pafcore::StaticField(\"%s\", %s, RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (size_t)&%s::%s, %s, %s),\n",
				fieldNode->m_name->m_str.c_str(), strAttributes, typeName.c_str(), className.c_str(), fieldNameNode->m_str.c_str(),
				arraySize, typeCompound);
		}
		else
		{
			sprintf_s(buf, "::pafcore::InstanceField(\"%s\", %s, GetSingleton(), RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), paf_field_offset_of(%s, %s), %s, %s),\n",
				fieldNode->m_name->m_str.c_str(), strAttributes, typeName.c_str(), className.c_str(), fieldNameNode->m_str.c_str(),
				arraySize, typeCompound);
		}

		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);

	if (isStatic)
	{
		writeStringToFile("m_staticFields = s_staticFields;\n", file, indentation);
		writeStringToFile("m_staticFieldCount = paf_array_size_of(s_staticFields);\n", file, indentation);
	}
	else
	{
		writeStringToFile("m_instanceFields = s_instanceFields;\n", file, indentation);
		writeStringToFile("m_instanceFieldCount = paf_array_size_of(s_instanceFields);\n", file, indentation);
	}
}

void writeMetaConstructor_Properties(
	ClassNode* classNode, 
	TemplateArguments* templateArguments, 
	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets,
	std::vector<PropertyNode*>& propertyNodes, 
	bool isStatic, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	char strAttributes[256];
	char enumerateFunc[1024];
	char getFunc[1024];
	char setFunc[1024];
	char metaType[1024];

	if (propertyNodes.empty())
	{
		return;
	}
	uint32_t count = static_cast<uint32_t>(propertyNodes.size());

	if (isStatic)
	{
		writeStringToFile("static ::pafcore::StaticProperty s_staticProperties[] = \n", file, indentation);
	}
	else
	{
		writeStringToFile("static ::pafcore::InstanceProperty s_instanceProperties[] = \n", file, indentation);
	}

	writeStringToFile("{\n", file, indentation);

	for (PropertyNode* propertyNode : propertyNodes)
	{
		if (propertyNode->m_attributeList)
		{
			auto it = attributesOffsets.find(propertyNode);
			assert(it != attributesOffsets.end());
			sprintf_s(strAttributes, "&s_attributeses[%u]", it->second);
		}
		else
		{
			strcpy_s(strAttributes, "0");
		}
	
		std::string typeName;
		std::string compoundTypeName;
		TypeKind typeKind = CalcTypeNativeName(typeName, propertyNode->m_compoundType->m_typeName, templateArguments);
		//compoundTypeName = CalcCompoundTypeNativeName(typeName, propertyNode->m_compoundType->m_typeCompound);
		const char* typeCompound = TypeCompoundString(propertyNode->m_compoundType);

		sprintf_s(metaType, "RuntimeTypeOf<%s>::RuntimeType::GetSingleton()", typeName.c_str());


		if (propertyNode->m_enumerate)
		{
			sprintf_s(enumerateFunc, "%s_enum_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
		}
		else
		{
			strcpy_s(enumerateFunc, "nullptr");
		}

		if (propertyNode->m_get)
		{
			sprintf_s(getFunc, "%s_get_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
		}
		else
		{
			strcpy_s(getFunc, "nullptr");
		}

		if (propertyNode->m_set)
		{
			sprintf_s(setFunc, "%s_set_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
		}
		else
		{
			strcpy_s(setFunc, "nullptr");
		}

		if (propertyNode->isSimple())
		{
			sprintf_s(buf, "::pafcore::%s(\"%s\", %s, %s%s, %s, %s, %s, %s),\n",
				isStatic ? "StaticProperty" : "InstanceProperty",
				propertyNode->m_name->m_str.c_str(), strAttributes, isStatic ? "" : "GetSingleton(), ",
				metaType, typeCompound, enumerateFunc, getFunc, setFunc);
		}
		else if(propertyNode->isFixedArray() || propertyNode->isDynamicArray())
		{
			char sizeFunc[1024];
			char resizeFunc[1024];
			sprintf_s(sizeFunc, "%s_size_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());

			if (propertyNode->isDynamicArray())
			{
				sprintf_s(resizeFunc, "%s_resize_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
			}
			else
			{
				strcpy_s(resizeFunc, "nullptr");
			}

			sprintf_s(buf, "::pafcore::%s(\"%s\", %s, %s%s, %s, %s, %s, %s, %s, %s),\n",
				isStatic ? "StaticProperty" : "InstanceProperty",
				propertyNode->m_name->m_str.c_str(), strAttributes, isStatic ? "" : "GetSingleton(), ",
				metaType, typeCompound, enumerateFunc, getFunc, setFunc, sizeFunc, resizeFunc);
		}
		else if (propertyNode->isList())
		{
			char iterateFunc[1024];
			char insertFunc[1024];
			char eraseFunc[1024];

			sprintf_s(iterateFunc, "%s_iterate_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
			sprintf_s(insertFunc, "%s_insert_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());
			sprintf_s(eraseFunc, "%s_erase_%s", classNode->m_name->m_str.c_str(), propertyNode->m_name->m_str.c_str());

			sprintf_s(buf, "::pafcore::%s(\"%s\", %s, %s%s, %s, %s, %s, %s, %s, %s, %s),\n",
				isStatic ? "StaticProperty" : "InstanceProperty",
				propertyNode->m_name->m_str.c_str(), strAttributes, isStatic ? "" : "GetSingleton(), ",
				metaType, typeCompound, enumerateFunc, getFunc, setFunc, iterateFunc, insertFunc, eraseFunc);
		}
		else
		{
			assert(false);
		}
		writeStringToFile(buf, file, indentation + 1);
	}

	writeStringToFile("};\n", file, indentation);

	if (isStatic)
	{
		writeStringToFile("m_staticProperties = s_staticProperties;\n", file, indentation);
		writeStringToFile("m_staticPropertyCount = paf_array_size_of(s_staticProperties);\n", file, indentation);
	}
	else
	{
		writeStringToFile("m_instanceProperties = s_instanceProperties;\n", file, indentation);
		writeStringToFile("m_instancePropertyCount = paf_array_size_of(s_instanceProperties);\n", file, indentation);
	}
}

void writeMetaConstructor_Method_Results(ClassNode* classNode, TemplateArguments* templateArguments, MethodNode* methodNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	uint32_t resultCount = methodNode->getResultCount();

	if (0 < resultCount)
	{
		std::vector<VariableNode*> resultNodes;
		methodNode->m_resultList->collectVariableNodes(resultNodes);
		assert(resultNodes.size() == resultCount);

		for (VariableNode* resultNode : resultNodes)
		{
			const char* typeCompound = TypeCompoundString(resultNode->m_compoundType);
			const char* byRef = resultNode->isByRef() ? "true" : "false";
			//const char* allowNull = resultNode->isAllowNull() ? "true" : "false";
			TypeKind typeKind = CalcTypeNativeName(typeName, resultNode->m_compoundType->m_typeName, templateArguments);
			sprintf_s(buf, "::pafcore::Result(\"%s\", RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), %s, %s),\n",
				resultNode->m_name->m_str.c_str(), typeName.c_str(), typeCompound, byRef);
			writeStringToFile(buf, file, indentation);
		}
	}
}

void writeMetaConstructor_Method_Parameters(ClassNode* classNode, TemplateArguments* templateArguments, MethodNode* methodNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	uint32_t paramCount = methodNode->getParameterCount();

	if (0 < paramCount)
	{
		std::vector<VariableNode*> parameterNodes;
		methodNode->m_parameterList->collectVariableNodes(parameterNodes);
		assert(parameterNodes.size() == paramCount);

		for (VariableNode* parameterNode : parameterNodes)
		{
			const char* typeCompound = TypeCompoundString(parameterNode->m_compoundType);
			const char* byRef = parameterNode->isByRef() ? "true" : "false";
			const char* allowNull = parameterNode->isAllowNull() ? "true" : "false";
			TypeKind typeKind = CalcTypeNativeName(typeName, parameterNode->m_compoundType->m_typeName, templateArguments);
			sprintf_s(buf, "::pafcore::Parameter(\"%s\", RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), %s, %s, %s),\n",
				parameterNode->m_name->m_str.c_str(), typeName.c_str(), typeCompound, byRef, allowNull);
			writeStringToFile(buf, file, indentation);
		}
	}
}

void writeMetaConstructor_Methods(
	ClassNode* classNode, 
	TemplateArguments* templateArguments, 
	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets,
	std::vector<MethodNode*>& methodNodes, 
	bool isStatic, 
	FILE* file, 
	int indentation)
{
	char buf[4096];

	if (methodNodes.empty())
	{
		return;
	}

	//Result & Arguments
	uint32_t count = static_cast<uint32_t>(methodNodes.size());

	if (isStatic)
	{
		writeStringToFile("static ::pafcore::Result s_staticResults[] = \n", file, indentation);
	}
	else
	{
		writeStringToFile("static ::pafcore::Result s_instanceResults[] = \n", file, indentation);
	}
	writeStringToFile("{\n", file, indentation);
	for (uint32_t i = 0; i < count; ++i)
	{
		MethodNode* methodNode = methodNodes[i];
		writeMetaConstructor_Method_Results(classNode, templateArguments, methodNode, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);


	bool hasArguments = false;
	for (uint32_t i = 0; i < count; ++i)
	{
		MethodNode* methodNode = methodNodes[i];
		uint32_t paramCount = methodNode->getParameterCount();
		if (paramCount > 0)
		{
			hasArguments = true;
			break;
		}
	}

	if (hasArguments)
	{
		if (isStatic)
		{
			writeStringToFile("static ::pafcore::Parameter s_staticArguments[] = \n", file, indentation);
		}
		else
		{
			writeStringToFile("static ::pafcore::Parameter s_instanceArguments[] = \n", file, indentation);
		}
		writeStringToFile("{\n", file, indentation);
		for (MethodNode* methodNode : methodNodes)
		{
			writeMetaConstructor_Method_Parameters(classNode, templateArguments, methodNode, file, indentation + 1);
		}
		writeStringToFile("};\n", file, indentation);
	}

	//Overloads
	if (isStatic)
	{
		writeStringToFile("static ::pafcore::Overload s_staticOverloads[] = \n", file, indentation);
	}
	else
	{
		writeStringToFile("static ::pafcore::Overload s_instanceOverloads[] = \n", file, indentation);
	}
	writeStringToFile("{\n", file, indentation);

	std::vector<MethodNode*>::iterator begin = methodNodes.begin();
	std::vector<MethodNode*>::iterator end = methodNodes.end();
	std::vector<MethodNode*>::iterator first = begin;
	std::vector<MethodNode*>::iterator last = begin;

	uint32_t resultOffset = 0;
	uint32_t parameterOffset = 0;
	for (; first != end;)
	{
		++last;
		if (last == end || (*last)->m_name->m_str != (*first)->m_name->m_str)
		{
			for (auto it = first; it != last; ++it)
			{
				char strResults[256];
				char strParameters[256];
				MethodNode* methodNode = *it;
				uint32_t parameterCount = methodNode->getParameterCount();
				uint32_t resultCount = methodNode->getResultCount();
				if (resultCount > 0)
				{
					sprintf_s(strResults, "&%s[%u]", isStatic ? "s_staticResults" : "s_instanceResults", resultOffset);
				}
				else
				{
					strcpy_s(strParameters, "nullptr");
				}
				if (parameterCount > 0)
				{
					sprintf_s(strParameters, "&%s[%u]", isStatic ? "s_staticArguments" : "s_instanceArguments", parameterOffset);
				}
				else
				{
					strcpy_s(strParameters, "nullptr");
				}

				sprintf_s(buf, "::pafcore::Overload(%s, %u, %s, %u),\n",
					strResults, resultCount, strParameters, parameterCount);
				writeStringToFile(buf, file, indentation + 1);
				resultOffset += resultCount;
				parameterOffset += parameterCount;
			}
			first = last;
		}
	}
	writeStringToFile("};\n", file, indentation);

	//Method
	if (isStatic)
	{
		writeStringToFile("static ::pafcore::StaticMethod s_staticMethods[] = \n", file, indentation);
	}
	else
	{
		writeStringToFile("static ::pafcore::InstanceMethod s_instanceMethods[] = \n", file, indentation);
	}
	writeStringToFile("{\n", file, indentation);

	first = begin;
	last = first;
	uint32_t overloadOffset = 0;

	for (; first != end;)
	{
		++last;
		if (last == end || (*last)->m_name->m_str != (*first)->m_name->m_str)
		{
			char strAttributes[256];
			const char* methodName = (*first)->m_name->m_str.c_str();
			auto it = attributesOffsets.find(*first);
			if (it != attributesOffsets.end())
			{
				sprintf_s(strAttributes, "&s_attributeses[%u]", it->second);
			}
			else
			{
				strcpy_s(strAttributes, "0");
			}
			int overloadCount = last - first;

			sprintf_s(buf, "::pafcore::%s(\"%s\", %s, %s_%s, &%s[%u], %d),\n",
				isStatic ? "StaticMethod" : "InstanceMethod",
				methodName, strAttributes, classNode->m_name->m_str.c_str(), methodName,
				isStatic ? "s_staticOverloads" : "s_instanceOverloads", overloadOffset, overloadCount);
			writeStringToFile(buf, file, indentation + 1);

			overloadOffset += overloadCount;
			first = last;
		}
	}
	writeStringToFile("};\n", file, indentation);

	if (isStatic)
	{
		writeStringToFile("m_staticMethods = s_staticMethods;\n", file, indentation);
		writeStringToFile("m_staticMethodCount = paf_array_size_of(s_staticMethods);\n", file, indentation);
	}
	else
	{
		writeStringToFile("m_instanceMethods = s_instanceMethods;\n", file, indentation);
		writeStringToFile("m_instanceMethodCount = paf_array_size_of(s_instanceMethods);\n", file, indentation);
	}
}

bool MethodNodeNameEqual(MethodNode* arg1, MethodNode* arg2)
{
	return arg1->m_name->m_str == arg2->m_name->m_str;
}

void writeMetaConstructor_Member(
	std::vector<MemberNode*>& nestedTypeNodes,
	std::vector<MemberNode*>& nestedTypeAliasNodes,
	std::vector<FieldNode*>& staticFieldNodes,
	std::vector<PropertyNode*>& staticPropertyNodes,
	const std::vector<MethodNode*>& staticMethodNodes_,
	std::vector<FieldNode*>& fieldNodes,
	std::vector<PropertyNode*>& propertyNodes,
	const std::vector<MethodNode*>& methodNodes_,
	FILE* file, int indentation)
{
	char buf[4096];

	std::vector<MethodNode*> staticMethodNodes = staticMethodNodes_;
	std::vector<MethodNode*> methodNodes = methodNodes_;

	auto it = std::unique(staticMethodNodes.begin(), staticMethodNodes.end(), MethodNodeNameEqual);
	staticMethodNodes.erase(it, staticMethodNodes.end());

	it = std::unique(methodNodes.begin(), methodNodes.end(), MethodNodeNameEqual);
	methodNodes.erase(it, methodNodes.end());

	uint32_t nestedTypeCount = static_cast<uint32_t>(nestedTypeNodes.size());
	uint32_t nestedTypeAliasCount = static_cast<uint32_t>(nestedTypeAliasNodes.size());
	uint32_t staticFieldCount = static_cast<uint32_t>(staticFieldNodes.size());
	uint32_t staticPropertyCount = static_cast<uint32_t>(staticPropertyNodes.size());
	uint32_t staticMethodCount = static_cast<uint32_t>(staticMethodNodes.size());
	uint32_t fieldCount = static_cast<uint32_t>(fieldNodes.size());
	uint32_t propertyCount = static_cast<uint32_t>(propertyNodes.size());
	uint32_t methodCount = static_cast<uint32_t>(methodNodes.size());

	enum MemberKind
	{
		unknown_member,
		nested_type,
		nested_type_alias,
		static_field,
		static_property,
		static_method,
		instance_field,
		instance_property,
		instance_method,
	};

	if (!(staticFieldNodes.empty() && staticPropertyNodes.empty() && staticMethodNodes.empty()
		&& fieldNodes.empty() && propertyNodes.empty() && methodNodes.empty()))
	{

		writeStringToFile("static Metadata* s_members[] = \n", file, indentation);
		writeStringToFile("{\n", file, indentation);


		//uint32_t currentNestedType = 0;
		//uint32_t currentNestedTypeAlias = 0;
		uint32_t currentStaticField = 0;
		uint32_t currentStaticProperty = 0;
		uint32_t currentStaticMethod = 0;
		uint32_t currentField = 0;
		uint32_t currentProperty = 0;
		uint32_t currentMethod = 0;
		while (true)
		{
			MemberNode* current = 0;
			MemberKind kind = unknown_member;
			//if (currentNestedType < nestedTypeCount)
			//{
			//	current = nestedTypeNodes[currentNestedType];
			//	kind = nested_type;
			//}
			//if (currentNestedTypeAlias < nestedTypeAliasCount)
			//{
			//	MemberNode* memberNode = nestedTypeAliasNodes[currentNestedTypeAlias];
			//	if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
			//	{
			//		current = memberNode;
			//		kind = nested_type_alias;
			//	}
			//}
			if (currentStaticField < staticFieldCount)
			{
				MemberNode* memberNode = staticFieldNodes[currentStaticField];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_field;
				}
			}
			if (currentStaticProperty < staticPropertyCount)
			{
				MemberNode* memberNode = staticPropertyNodes[currentStaticProperty];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_property;
				}
			}
			if (currentStaticMethod < staticMethodCount)
			{
				MemberNode* memberNode = staticMethodNodes[currentStaticMethod];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_method;
				}
			}
			if (currentField < fieldCount)
			{
				MemberNode* memberNode = fieldNodes[currentField];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = instance_field;
				}
			}
			if (currentProperty < propertyCount)
			{
				MemberNode* memberNode = propertyNodes[currentProperty];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = instance_property;
				}
			}
			if (currentMethod < methodCount)
			{
				MemberNode* memberNode = methodNodes[currentMethod];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = instance_method;
				}
			}
			if (unknown_member == kind)
			{
				break;
			}
			switch (kind)
			{
			//case nested_type:
			//	sprintf_s(buf, "s_nestedTypes[%u],\n", currentNestedType);
			//	++currentNestedType;
			//	break;
			//case nested_type_alias:
			//	sprintf_s(buf, "s_nestedTypeAliases[%u],\n", currentNestedTypeAlias);
			//	++currentNestedTypeAlias;
			//	break;
			case static_field:
				sprintf_s(buf, "&s_staticFields[%d],\n", staticFieldNodes[currentStaticField]->m_orderIndex);
				++currentStaticField;
				break;
			case static_property:
				sprintf_s(buf, "&s_staticProperties[%d],\n", staticPropertyNodes[currentStaticProperty]->m_orderIndex);
				++currentStaticProperty;
				break;
			case static_method:
				sprintf_s(buf, "&s_staticMethods[%u],\n", currentStaticMethod);
				++currentStaticMethod;
				break;
			case instance_field:
				sprintf_s(buf, "&s_instanceFields[%d],\n", fieldNodes[currentField]->m_orderIndex);
				++currentField;
				break;
			case instance_property:
				sprintf_s(buf, "&s_instanceProperties[%d],\n", propertyNodes[currentProperty]->m_orderIndex);
				++currentProperty;
				break;
			case instance_method:
				sprintf_s(buf, "&s_instanceMethods[%u],\n", currentMethod);
				++currentMethod;
				break;
			default:
				assert(false);
			}
			writeStringToFile(buf, file, indentation + 1);
		}

		writeStringToFile("};\n", file, indentation);
		writeStringToFile("m_members = s_members;\n", file, indentation);
		writeStringToFile("m_memberCount = paf_array_size_of(s_members);\n", file, indentation);
	}

	if (!(nestedTypeNodes.empty() && nestedTypeAliasNodes.empty() &&
		staticFieldNodes.empty() && staticPropertyNodes.empty() &&
		staticMethodNodes.empty()))
	{
		writeStringToFile("static Metadata* s_classMembers[] = \n", file, indentation);
		writeStringToFile("{\n", file, indentation);

		uint32_t classCurrentNestedType = 0;
		uint32_t classCurrentNestedTypeAlias = 0;
		uint32_t classCurrentStaticField = 0;
		uint32_t classCurrentStaticProperty = 0;
		uint32_t classCurrentStaticMethod = 0;

		while (true)
		{
			MemberNode* current = 0;
			MemberKind kind = unknown_member;
			if (classCurrentNestedType < nestedTypeCount)
			{
				current = nestedTypeNodes[classCurrentNestedType];
				kind = nested_type;
			}
			if (classCurrentNestedTypeAlias < nestedTypeAliasCount)
			{
				MemberNode* memberNode = nestedTypeAliasNodes[classCurrentNestedTypeAlias];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = nested_type_alias;
				}
			}
			if (classCurrentStaticField < staticFieldCount)
			{
				MemberNode* memberNode = staticFieldNodes[classCurrentStaticField];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_field;
				}
			}
			if (classCurrentStaticProperty < staticPropertyCount)
			{
				MemberNode* memberNode = staticPropertyNodes[classCurrentStaticProperty];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_property;
				}
			}
			if (classCurrentStaticMethod < staticMethodCount)
			{
				MemberNode* memberNode = staticMethodNodes[classCurrentStaticMethod];
				if (0 == current || memberNode->m_name->m_str < current->m_name->m_str)
				{
					current = memberNode;
					kind = static_method;
				}
			}
			if (unknown_member == kind)
			{
				break;
			}
			switch (kind)
			{
			case nested_type:
				sprintf_s(buf, "s_nestedTypes[%u],\n", classCurrentNestedType);
				++classCurrentNestedType;
				break;
			case nested_type_alias:
				sprintf_s(buf, "s_nestedTypeAliases[%u],\n", classCurrentNestedTypeAlias);
				++classCurrentNestedTypeAlias;
				break;
			case static_field:
				sprintf_s(buf, "&s_staticFields[%d],\n", staticFieldNodes[classCurrentStaticField]->m_orderIndex);
				++classCurrentStaticField;
				break;
			case static_property:
				sprintf_s(buf, "&s_staticProperties[%d],\n", staticPropertyNodes[classCurrentStaticProperty]->m_orderIndex);
				++classCurrentStaticProperty;
				break;
			case static_method:
				sprintf_s(buf, "&s_staticMethods[%u],\n", classCurrentStaticMethod);
				++classCurrentStaticMethod;
				break;
			default:
				assert(false);
			}
			writeStringToFile(buf, file, indentation + 1);
		}

		writeStringToFile("};\n", file, indentation);
		writeStringToFile("m_classMembers = s_classMembers;\n", file, indentation);
		writeStringToFile("m_classMemberCount = paf_array_size_of(s_classMembers);\n", file, indentation);
	}

	bool findDynamicInstanceField = false;
	bool findDynamicInstanceFieldCount = false;
	bool findToString = false;
	bool findFromString = false;
	for (MethodNode* methodNode : methodNodes)
	{
		if (methodNode->m_name->m_str == "dynamicInstanceField__")
		{
			findDynamicInstanceField = true;
		}
		else if (methodNode->m_name->m_str == "dynamicInstanceFieldCount__")
		{
			findDynamicInstanceFieldCount = true;
		}
		else if (methodNode->m_name->m_str == "toString__")
		{
			findToString = true;
		}
		else if (methodNode->m_name->m_str == "fromString__")
		{
			findFromString = true;
		}
	}
	if (findDynamicInstanceField && findDynamicInstanceFieldCount)
	{
		writeStringToFile("m_hasDynamicInstanceField = true;\n", file, indentation);
	}
	if (findToString && findFromString)
	{
		writeStringToFile("m_specialClass = string_class;\n", file, indentation);
	}
}

void writeMetaConstructor_BaseClasses(ClassNode* classNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	std::vector<std::pair<TokenNode*, TypeNameNode*>> tempNodes;
	classNode->m_baseList->collectTypeNameNodesNotNoMeta(tempNodes);
	std::vector<TypeNameNode*> typeNameNodes;
	uint32_t count = static_cast<uint32_t>(tempNodes.size());
	for (uint32_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = tempNodes[i].second;
		TypeNode* typeNode = typeNameNode->getActualTypeNode(templateArguments);
		std::string fullName;
		typeNode->getFullName(fullName);
		if (fullName != "::pafcore::Interface" && !typeNode->getSyntaxNode()->isNoMeta())
		{
			typeNameNodes.push_back(typeNameNode);
		}
	}

	if (typeNameNodes.empty())
	{
		return;
	}
	std::string className;
	classNode->getNativeName(className, templateArguments);

	writeStringToFile("static BaseClass s_baseClasses[] =\n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	
	for (TypeNameNode* typeNameNode : typeNameNodes)
	{
		TypeKind typeKind = CalcTypeNativeName(typeName, typeNameNode, templateArguments);
			sprintf_s(buf, "{RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), paf_base_offset_of(%s, %s)},\n",
				typeName.c_str(), className.c_str(), typeName.c_str());
			writeStringToFile(buf, file, indentation + 1);	
	}
	writeStringToFile("};\n", file, indentation);
	writeStringToFile("m_baseClasses = s_baseClasses;\n", file, indentation);
	writeStringToFile("m_baseClassCount = paf_array_size_of(s_baseClasses);\n", file, indentation);
}

void writeMetaConstructor_ClassTypeIterators(ClassNode* classNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	std::vector<std::pair<TokenNode*, TypeNameNode*>> tempNodes;
	classNode->m_baseList->collectTypeNameNodesNotNoMeta(tempNodes);
	std::vector<TypeNameNode*> typeNameNodes;
	uint32_t count = static_cast<uint32_t>(tempNodes.size());
	for (uint32_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = tempNodes[i].second;
		TypeNode* typeNode = typeNameNode->getActualTypeNode(templateArguments);
		std::string fullName;
		typeNode->getFullName(fullName);
		if (fullName != "::pafcore::Interface" && !typeNode->getSyntaxNode()->isNoMeta())
		{
			typeNameNodes.push_back(typeNameNode);
		}
	}

	if (typeNameNodes.empty())
	{
		return;
	}
	//std::string className;
	//classNode->getNativeName(className, templateArguments);

	writeStringToFile("static ::pafcore::ClassTypeIterator s_classTypeIterators[] =\n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	count = typeNameNodes.size();
	for (uint32_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = typeNameNodes[i];
		TypeKind typeKind = CalcTypeNativeName(typeName, typeNameNode, templateArguments);
		sprintf_s(buf, "::pafcore::ClassTypeIterator(RuntimeTypeOf<%s>::RuntimeType::GetSingleton()->m_firstDerivedClass, this),\n",
			typeName.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);
	for (uint32_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = typeNameNodes[i];
		TypeKind typeKind = CalcTypeNativeName(typeName, typeNameNode, templateArguments);
		sprintf_s(buf, "RuntimeTypeOf<%s>::RuntimeType::GetSingleton()->m_firstDerivedClass = &s_classTypeIterators[%u];\n",
			typeName.c_str(), i);
		writeStringToFile(buf, file, indentation);
	}
	writeStringToFile("m_classTypeIterators = s_classTypeIterators;\n", file, indentation);
}

void writeMetaConstructor_NestedTypes(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<MemberNode*>& nestedTypeNodes, FILE* file, int indentation)
{
	char buf[4096];
	if (nestedTypeNodes.empty())
	{
		return;
	}
	writeStringToFile("static ::pafcore::Type* s_nestedTypes[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);

	for (MemberNode* typeNode : nestedTypeNodes)
	{
		std::string metaTypeName;
		GetMetaTypeFullName(metaTypeName, typeNode, templateArguments);

		sprintf_s(buf, "%s::GetSingleton(),\n", metaTypeName.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);

	writeStringToFile("m_nestedTypes = s_nestedTypes;\n", file, indentation);
	writeStringToFile("m_nestedTypeCount = paf_array_size_of(s_nestedTypes);\n", file, indentation);
}

void writeMetaConstructor_NestedTypeAliases(ClassNode* classNode, TemplateArguments* templateArguments, std::vector<MemberNode*>& nestedTypeAliasNodes, FILE* file, int indentation)
{
	char buf[4096];
	if (nestedTypeAliasNodes.empty())
	{
		return;
	}
	uint32_t count = static_cast<uint32_t>(nestedTypeAliasNodes.size());
	writeStringToFile("static ::pafcore::TypeAlias* s_nestedTypeAliases[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);

	for (MemberNode* typeAliasNode : nestedTypeAliasNodes)
	{
		std::string metaTypeName;
		GetMetaTypeFullName(metaTypeName, typeAliasNode, templateArguments);
		sprintf_s(buf, "%s::GetSingleton(),\n", metaTypeName.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);

	writeStringToFile("m_nestedTypeAliases = s_nestedTypeAliases;\n", file, indentation);
	writeStringToFile("m_nestedTypeAliasCount = paf_array_size_of(s_nestedTypeAliases);\n", file, indentation);
}

void writeMetaConstructor_Scope(MemberNode* memberNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	char buf[4096];
	assert(0 != memberNode->m_enclosing);
	if (snt_namespace == memberNode->m_enclosing->m_nodeType)
	{
		writeStringToFile("::pafcore::NameSpace::GetGlobalNameSpace()", file, indentation);

		std::vector<ScopeNode*> enclosings;
		memberNode->getEnclosings(enclosings);
		assert(!enclosings.empty());
		uint32_t count = static_cast<uint32_t>(enclosings.size());
		for (uint32_t i = 1; i < count; ++i)//enclosings[0] is global namespace 
		{
			ScopeNode* enclosing = enclosings[i];
			if (snt_namespace == enclosing->m_nodeType)
			{
				sprintf_s(buf, "->getNameSpace(\"%s\")", enclosing->m_name->m_str.c_str());
				writeStringToFile(buf, file);
			}
			else
			{
				break;
			}
		}
		writeStringToFile("->registerMember(this);\n", file);
	}
	else if (snt_class == memberNode->m_enclosing->m_nodeType)
	{
		std::string metaTypeName;
		GetMetaTypeFullName(metaTypeName, memberNode->m_enclosing, templateArguments);
		sprintf_s(buf, "m_enclosing = %s::GetSingleton();\n", metaTypeName.c_str());
		writeStringToFile(buf, file, indentation);
	}
	else
	{
		assert(false);
	}
}

void writeMetaConstructor(ClassNode* classNode,
	TemplateArguments* templateArguments,
	std::vector<MemberNode*>& nestedTypeNodes,
	std::vector<MemberNode*>& nestedTypeAliasNodes,
	std::vector<FieldNode*>& staticFieldNodes,
	std::vector<PropertyNode*>& staticPropertyNodes,
	std::vector<MethodNode*>& staticMethodNodes,
	std::vector<FieldNode*>& fieldNodes,
	std::vector<PropertyNode*>& propertyNodes,
	std::vector<MethodNode*>& methodNodes,
	FILE* file, int indentation)
{
	char buf[4096];
	std::string localClassName;
	std::string className;
	std::string metaClassName;
	classNode->getLocalName(localClassName, templateArguments);
	classNode->getNativeName(className, templateArguments);
	GetMetaTypeFullName(metaClassName, classNode, templateArguments);


	std::sort(nestedTypeNodes.begin(), nestedTypeNodes.end(), CompareMemberNodeByName());
	std::sort(nestedTypeAliasNodes.begin(), nestedTypeAliasNodes.end(), CompareMemberNodeByName());
	//std::sort(fieldNodes.begin(), fieldNodes.end(), CompareMemberNodeByName());
	//std::sort(propertyNodes.begin(), propertyNodes.end(), CompareMemberNodeByName());
	std::sort(methodNodes.begin(), methodNodes.end(), CompareMethodNode());
	//std::sort(staticFieldNodes.begin(), staticFieldNodes.end(), CompareMemberNodeByName());
	//std::sort(staticPropertyNodes.begin(), staticPropertyNodes.end(), CompareMemberNodeByName());
	std::sort(staticMethodNodes.begin(), staticMethodNodes.end(), CompareMethodNode());

	sprintf_s(buf, "%s::%s() : ::pafcore::ClassType(\"%s\", ::pafcore::MetadataKind::%s, \"%s\")\n",
		metaClassName.c_str(), metaClassName.c_str(), localClassName.c_str(),
		classNode->m_metadataKind ? classNode->m_metadataKind->m_str.c_str() : "class_instance",
		classNode->getSourceFilePath().c_str());

	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets;
	writeMetaConstructor_Attributeses(attributesOffsets, classNode, staticFieldNodes, staticPropertyNodes, staticMethodNodes,
		fieldNodes, propertyNodes, methodNodes, file, indentation + 1);

	writeMetaConstructor_attributesForType(attributesOffsets, classNode, file, indentation + 1);

	sprintf_s(buf, "m_size = sizeof(%s);\n", className.c_str());
	writeStringToFile(buf, file, indentation + 1);

	writeMetaConstructor_BaseClasses(classNode, templateArguments, file, indentation + 1);
	writeMetaConstructor_ClassTypeIterators(classNode, templateArguments, file, indentation + 1);
	writeMetaConstructor_NestedTypes(classNode, templateArguments, nestedTypeNodes, file, indentation + 1);
	writeMetaConstructor_NestedTypeAliases(classNode, templateArguments, nestedTypeAliasNodes, file, indentation + 1);
	writeMetaConstructor_Fields(classNode, templateArguments, attributesOffsets, staticFieldNodes, true, file, indentation + 1);
	writeMetaConstructor_Properties(classNode, templateArguments, attributesOffsets, staticPropertyNodes, true, file, indentation + 1);
	writeMetaConstructor_Methods(classNode, templateArguments, attributesOffsets, staticMethodNodes, true, file, indentation + 1);

	writeMetaConstructor_Fields(classNode, templateArguments, attributesOffsets, fieldNodes, false, file, indentation + 1);
	writeMetaConstructor_Properties(classNode, templateArguments, attributesOffsets, propertyNodes, false, file, indentation + 1);
	writeMetaConstructor_Methods(classNode, templateArguments, attributesOffsets, methodNodes, false, file, indentation + 1);

	std::sort(fieldNodes.begin(), fieldNodes.end(), CompareMemberNodeByName());
	std::sort(propertyNodes.begin(), propertyNodes.end(), CompareMemberNodeByName());
	std::sort(staticFieldNodes.begin(), staticFieldNodes.end(), CompareMemberNodeByName());
	std::sort(staticPropertyNodes.begin(), staticPropertyNodes.end(), CompareMemberNodeByName());

	writeMetaConstructor_Member(nestedTypeNodes, nestedTypeAliasNodes, staticFieldNodes, staticPropertyNodes,
		staticMethodNodes, fieldNodes, propertyNodes, methodNodes, file, indentation + 1);

	writeMetaConstructor_Scope(classNode, templateArguments, file, indentation + 1);

	writeStringToFile("}\n\n", file, indentation);
}

void writeEnumMetaConstructor_Enumerators(
	EnumNode* enumNode, 
	TemplateArguments* templateArguments, 
	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets,
	std::vector<EnumeratorNode*>& enumerators, 
	FILE* file, 
	int indentation)
{
	char buf[4096];
	char strAttributes[256];
	if (enumerators.empty())
	{
		return;
	}
	std::string metaClassName;
	GetMetaTypeFullName(metaClassName, enumNode, templateArguments);
	uint32_t count = static_cast<uint32_t>(enumerators.size());

	writeStringToFile("static ::pafcore::EnumMember s_enumerators[] = \n", file, indentation);
	writeStringToFile("{\n", file, indentation);
	for (EnumeratorNode* enum_member : enumerators)
	{
		if (enum_member->m_attributeList)
		{
			auto it = attributesOffsets.find(enum_member);
			assert(it != attributesOffsets.end());
			sprintf_s(strAttributes, "&s_attributeses[%u]", it->second);
		}
		else
		{
			strcpy_s(strAttributes, "0");
		}

		std::string enumScopeName;
		enumNode->m_enclosing->getNativeName(enumScopeName, 0);
		if (enumNode->isStronglyTypedEnum())
		{
			sprintf_s(buf, "::pafcore::EnumMember(\"%s\", %s, %s::GetSingleton(), int(%s::%s::%s)),\n",
				enum_member->m_name->m_str.c_str(), strAttributes,
				metaClassName.c_str(), enumScopeName.c_str(), enumNode->m_name->m_str.c_str(), enum_member->m_name->m_str.c_str());
		}
		else
		{
			sprintf_s(buf, "::pafcore::EnumMember(\"%s\", %s, %s::GetSingleton(), %s::%s),\n",
				enum_member->m_name->m_str.c_str(), strAttributes,
				metaClassName.c_str(), enumScopeName.c_str(), enum_member->m_name->m_str.c_str());
		}
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("};\n", file, indentation);

	writeStringToFile("m_enumerators = s_enumerators;\n", file, indentation);
	writeStringToFile("m_enumeratorCount = paf_array_size_of(s_enumerators);\n", file, indentation);
}

void writeEnumMetaConstructor(EnumNode* enumNode, TemplateArguments* templateArguments, std::vector<EnumeratorNode*>& enumerators, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	std::string metaTypeName;
	enumNode->getNativeName(typeName, 0);
	GetMetaTypeFullName(metaTypeName, enumNode, templateArguments);

	sprintf_s(buf, "%s::%s() : ::pafcore::EnumType(\"%s\", \"%s\")\n",
		metaTypeName.c_str(),
		metaTypeName.c_str(),
		enumNode->m_name->m_str.c_str(),
		enumNode->getSourceFilePath().c_str());

	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets;
	writeEnumMetaConstructor_Attributeses(attributesOffsets, enumNode, enumerators, file, indentation + 1);
	writeMetaConstructor_attributesForType(attributesOffsets, enumNode, file, indentation + 1);

	sprintf_s(buf, "m_size = sizeof(%s);\n", typeName.c_str());
	writeStringToFile(buf, file, indentation + 1);

	writeEnumMetaConstructor_Enumerators(enumNode, templateArguments, attributesOffsets, enumerators, file, indentation + 1);
	writeMetaConstructor_Scope(enumNode, templateArguments, file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void writeMetaGetSingletonImpls(MemberNode* memberNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	char buf[4096];
	std::string metaClassName;
	GetMetaTypeFullName(metaClassName, memberNode, templateArguments);

	sprintf_s(buf, "%s* %s::GetSingleton()\n", metaClassName.c_str(), metaClassName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	sprintf_s(buf, "static %s* s_instance = 0;\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	sprintf_s(buf, "static char s_buffer[sizeof(%s)];\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation + 1);
	writeStringToFile("if(0 == s_instance)\n", file, indentation + 1);
	writeStringToFile("{\n", file, indentation + 1);
	sprintf_s(buf, "s_instance = (%s*)s_buffer;\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation + 2);
	sprintf_s(buf, "new (s_buffer)%s;\n", metaClassName.c_str());
	writeStringToFile(buf, file, indentation + 2);
	writeStringToFile("}\n", file, indentation + 1);
	writeStringToFile("return s_instance;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
}

void MetaSourceFileGenerator::generateCode_Typedef(FILE* file, TypedefNode* typedefNode, TemplateArguments* templateArguments, int indentation)
{
	if (typedefNode->isNoMeta())
	{
		return;
	}

	char buf[4096];
	std::string typeName;
	std::string metaTypeName;
	typedefNode->getNativeName(typeName, templateArguments);
	GetMetaTypeFullName(metaTypeName, typedefNode, templateArguments);

	sprintf_s(buf, "%s::%s() : TypeAlias(\"%s\", RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), \"%s\")\n",
		metaTypeName.c_str(), metaTypeName.c_str(), typedefNode->m_name->m_str.c_str(), typeName.c_str(), typedefNode->getSourceFilePath().c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);

	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets;
	writeTypeDefMetaConstructor_Attributeses(attributesOffsets, typedefNode, file, indentation + 1);
	writeMetaConstructor_attributesForType(attributesOffsets, typedefNode, file, indentation + 1);

	writeMetaConstructor_Scope(typedefNode, templateArguments, file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
	writeMetaGetSingletonImpls(typedefNode, templateArguments, file, indentation);
}

void MetaSourceFileGenerator::generateCode_TypeDeclaration(FILE* file, TypeDeclarationNode* typeDeclarationNode, TemplateArguments* templateArguments, int indentation)
{
	if (typeDeclarationNode->isNoMeta())
	{
		return;
	}

	char buf[4096];
	std::string typeName;
	std::string metaTypeName;
	typeDeclarationNode->getNativeName(typeName, templateArguments);
	GetMetaTypeFullName(metaTypeName, typeDeclarationNode, templateArguments);

	sprintf_s(buf, "%s::%s() : TypeAlias(\"%s\", RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), \"%s\")\n",
		metaTypeName.c_str(), metaTypeName.c_str(), typeDeclarationNode->m_name->m_str.c_str(), typeName.c_str(), typeDeclarationNode->getSourceFilePath().c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	std::map<SyntaxNodeImpl*, uint32_t> attributesOffsets;
	writeTypeDefMetaConstructor_Attributeses(attributesOffsets, typeDeclarationNode, file, indentation + 1);
	writeMetaConstructor_attributesForType(attributesOffsets, typeDeclarationNode, file, indentation + 1);
	writeMetaConstructor_Scope(typeDeclarationNode, templateArguments, file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);
	writeMetaGetSingletonImpls(typeDeclarationNode, templateArguments, file, indentation);
}

void writeInterfaceMethodImpl_AssignThis(ClassNode* classNode, MethodNode* methodNode, FILE* file, int indentation)
{
	std::string className;
	classNode->getFullName(className, 0);
	char buf[4096];
	sprintf_s(buf, "__self__.assignObserverPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), this);\n",
		className.c_str());
	writeStringToFile(buf, file, indentation);
}

void writeInterfaceMethodImpl_CastOutputParam(VariableNode* resultNode, uint32_t resultIndex, const char* resultName, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, resultNode->m_compoundType->m_typeName, nullptr);

	const char* func = "";
	switch (resultNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		switch (typeKind)
		{
		case primitive_type:
			func = "castToPrimitive";
			break;
		case enum_type:
			func = "castToEnum";
			break;
		case class_type:
			func = "castToClass";
			break;
		}
		break;
	case tc_raw_ptr:
		func = "castToRawPtr";
		break;
	case tc_shared_ptr:
		func = "castToSharedPtr";
		break;
	case tc_observer_ptr:
		func = "castToObserverPtr";
		break;
	case tc_shared_array:
		func = "castToSharedArray";
		break;
	case tc_observer_array:
		func = "castToObserverArray";
		break;
	}
	sprintf_s(buf, "__results__[%u].%s(%s);\n", resultIndex, func, resultName);
	writeStringToFile(buf, file, indentation);
}

void writeInterfaceMethodImpl_AssignInputParam(VariableNode* parameterNode, uint32_t argIndex, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	TypeKind typeKind = CalcTypeNativeName(typeName, parameterNode->m_compoundType->m_typeName, nullptr);

	const char* func = "";
	switch (parameterNode->m_compoundType->m_typeCompound)
	{
	case tc_none:
		switch (typeKind)
		{
		case primitive_type:
			func = "assignPrimitive";
			break;
		case enum_type:
			func = "assignEnum";
			break;
		case class_type:
			func = "assignClass";
			break;
		}
		break;
	case tc_raw_ptr:
		func = "assignRawPtr";
		break;
	case tc_shared_ptr:
		func = "assignSharedPtr";
		break;
	case tc_observer_ptr:
		func = "assignObserverPtr";
		break;
	case tc_shared_array:
		func = "assignSharedArray";
		break;
	case tc_observer_array:
		func = "assignObserverArray";
		break;
	}
	sprintf_s(buf, "__arguments__[%u].%s(%s);\n", argIndex, func, parameterNode->m_name->m_str.c_str());
	writeStringToFile(buf, file, indentation);
}

void writeInterfaceMethodImpl(ClassNode* classNode, TemplateArguments* templateArguments, MethodNode* methodNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string subclassProxyName;
	GetSubclassProxyFullName(subclassProxyName, classNode, templateArguments);
	IdentifierNode* methodNameNode = methodNode->m_nativeName ? methodNode->m_nativeName : methodNode->m_name;
	std::string resultName;

	std::vector<VariableNode*> resultNodes;
	methodNode->m_resultList->collectVariableNodes(resultNodes);
	uint32_t resultCount = (uint32_t)resultNodes.size();

	std::vector<VariableNode*> parameterNodes;
	methodNode->m_parameterList->collectVariableNodes(parameterNodes);
	uint32_t paramCount = (uint32_t)parameterNodes.size();


	if (resultCount > 0)
	{
		VariableNode* resultNode = resultNodes[0];
		std::string typeName;
		std::string compoundTypeName;
		TypeKind typeKind = CalcTypeNativeName(typeName, resultNode->m_compoundType->m_typeName, nullptr);
		compoundTypeName = CalcCompoundTypeNativeName(typeName, resultNode->m_compoundType->m_typeCompound);	
		resultName = compoundTypeName;
		if (resultNode->isByRef())
		{
			resultName += "&";
		}
	}
	else
	{
		resultName = "void";
	}

	sprintf_s(buf, "%s %s::%s( ", resultName.c_str(), subclassProxyName.c_str(), methodNameNode->m_str.c_str());
	writeStringToFile(buf, file, indentation);


	bool firstParam = true;
	for (uint32_t i = 1; i < resultCount; ++i)
	{
		if (!firstParam)
		{
			writeStringToFile(", ", file);
		}
		generateCode_Parameter(file, resultNodes[i], true, nullptr, false);
		firstParam = false;
	}
	for (uint32_t i = 0; i < paramCount; ++i)
	{
		if (!firstParam)
		{
			writeStringToFile(", ", file);
		}
		generateCode_Parameter(file, parameterNodes[i], false, nullptr, false);
		firstParam = false;
	}
	writeStringToFile(")\n", file);
	writeStringToFile("{\n", file, indentation);


	writeStringToFile("pafcore::Variant __self__;\n", file, indentation + 1);
	if (0 < resultCount)
	{
		writeStringToFile("", file, indentation + 1);
		generateCode_Parameter(file, resultNodes.front(), true, nullptr, false);
		writeStringToFile(";\n", file);
		sprintf_s(buf, "pafcore::Variant __results__[%u];\n", resultCount);
		writeStringToFile(buf, file, indentation + 1);
	}
	if (0 < paramCount)
	{
		sprintf_s(buf, "pafcore::Variant __arguments__[%u];\n", paramCount);
		writeStringToFile(buf, file, indentation + 1);
	}

	writeInterfaceMethodImpl_AssignThis(classNode, methodNode, file, indentation + 1);
	for (uint32_t i = 0; i < paramCount; ++i)
	{
		VariableNode* parameterNode = parameterNodes[i];
		writeInterfaceMethodImpl_AssignInputParam(parameterNode, i, file, indentation + 1);
	}

	writeStringToFile("if(m_subclassInvoker)\n", file, indentation + 1);
	writeStringToFile("{\n", file, indentation + 1);
	sprintf_s(buf, "::pafcore::ErrorCode __error__ = m_subclassInvoker->invoke(\"%s\", &__self__, %s, %u, %s, %u);\n",
		methodNode->m_name->m_str.c_str(), 
		resultCount > 0 ? "__results__" : "nullptr", resultCount,
		paramCount > 0 ? "__arguments__" : "nullptr", paramCount);

	writeStringToFile(buf, file, indentation + 2);
	writeStringToFile("}\n", file, indentation + 1);

	for (uint32_t i = 0; i < resultCount; ++i)
	{
		VariableNode* resultNode = resultNodes[i];
		writeInterfaceMethodImpl_CastOutputParam(resultNode, i, resultNode->m_name->m_str.c_str(), file, indentation + 1);
	}
	if (0 < resultCount)
	{
		sprintf_s(buf, "return %s;\n", resultNodes.front()->m_name->m_str.c_str());
		writeStringToFile(buf, file, indentation + 1);
	}
	writeStringToFile("}\n\n", file, indentation);
}

void writeInterfaceMethodsImpl(ClassNode* classNode, TemplateArguments* templateArguments, FILE* file, int indentation)
{
	std::vector<MethodNode*> methodNodes;
	classNode->collectOverrideMethods(methodNodes, templateArguments);
	for (MethodNode* methodNode : methodNodes)
	{
		assert(snt_method == methodNode->m_nodeType && methodNode->isVirtual());
		writeInterfaceMethodImpl(classNode, templateArguments, methodNode, file, indentation);
	}
}

void MetaSourceFileGenerator::generateCode_SubclassProxy(FILE* file, ClassNode* classNode, TemplateArguments* templateArguments, int indentation)
{
	char buf[4096];
	std::string subclassProxyName;
	GetSubclassProxyFullName(subclassProxyName, classNode, templateArguments);

	sprintf_s(buf, "%s::%s(::pafcore::SubclassInvoker* subclassInvoker)\n", subclassProxyName.c_str(), subclassProxyName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeStringToFile("m_subclassInvoker = subclassInvoker;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);

	sprintf_s(buf, "%s::~%s()\n", subclassProxyName.c_str(), subclassProxyName.c_str());
	writeStringToFile(buf, file, indentation);
	writeStringToFile("{\n", file, indentation);
	writeStringToFile("delete m_subclassInvoker;\n", file, indentation + 1);
	writeStringToFile("}\n\n", file, indentation);

	writeInterfaceMethodsImpl(classNode, templateArguments, file, indentation);
}


