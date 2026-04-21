#include "source_file_generator.h"
#include "utility.h"
#include "source_file.h"
#include "program_node.h"
#include "namespace_node.h"
#include "token_node.h"
#include "identify_node.h"
#include "enumerator_list_node.h"
#include "scope_name_list_node.h"
#include "member_list_node.h"
#include "enum_node.h"
#include "class_node.h"
#include "delegate_node.h"
#include "template_parameters_node.h"
#include "template_parameter_list_node.h"
#include "type_name_list_node.h"
#include "type_name_node.h"
#include "field_node.h"
#include "property_node.h"
#include "method_node.h"
#include "parameter_node.h"
#include "parameter_list_node.h"
#include "typedef_node.h"
#include "options.h"
#include "platform.h"
#include "common_funcs.h"
#include "type_tree.h"
#include <assert.h>

void generateCode_Token(FILE* file, TokenNode* tokenNode, int indentation);
void generateCode_Identify(FILE* file, IdentifyNode* identifyNode, int indentation, bool addSpace = true);
void generateCode_Parameter(FILE* file, ParameterNode* parameterNode, ScopeNode* scopeNode);
void generateCode_TypeName(FILE* file, TypeNameNode* typeNameNode, ScopeNode* scopeNode, bool addKeyword, int indentation);
void generateCode_ParameterList(FILE* file, ParameterListNode* parameterListNode, ScopeNode* scopeNode);
void writeInterfaceMethodImpl_AssignInputParam(ParameterNode* parameterNode, size_t argIndex, FILE* file, int indentation);
void writeInterfaceMethodImpl_SetOutputParamType(ParameterNode* parameterNode, size_t argIndex, FILE* file, int indentation);
void writeInterfaceMethodImpl_CastOutputParam(ParameterNode* parameterNode, size_t argIndex, FILE* file, int indentation);

void generateCode_SourceSmartResultType(FILE* file, const char* wrapperName, TypeNameNode* typeNameNode, ScopeNode* scopeNode, bool constant, int indentation)
{
	writeStringToFile(wrapperName, file, indentation);
	writeStringToFile("<", file);
	if (constant)
	{
		writeStringToFile("const ", file);
	}
	generateCode_TypeName(file, typeNameNode, scopeNode, true, 0);
	writeStringToFile("> ", file);
}

void generateCode_SourceResultType(FILE* file, TypeNameNode* typeNameNode, ScopeNode* scopeNode, bool constant, TokenNode* typeCompound, TokenNode* byRef, bool returnsOwning, int indentation)
{
	if (0 != typeCompound && '^' == typeCompound->m_nodeType)
	{
		generateCode_SourceSmartResultType(file, "::pafcore::SharedPtr", typeNameNode, scopeNode, constant, indentation);
		return;
	}

	if (0 != typeCompound && '!' == typeCompound->m_nodeType)
	{
		generateCode_SourceSmartResultType(file, "::pafcore::UniquePtr", typeNameNode, scopeNode, constant, indentation);
		return;
	}

	if (0 != typeCompound && '*' == typeCompound->m_nodeType)
	{
		generateCode_SourceSmartResultType(file, "::pafcore::ObserverPtr", typeNameNode, scopeNode, constant, indentation);
		return;
	}

	if (constant)
	{
		writeStringToFile("const ", file, indentation);
		indentation = 0;
	}
	generateCode_TypeName(file, typeNameNode, scopeNode, true, indentation);
	if (0 != byRef)
	{
		writeStringToFile("&", file);
	}
	else if (returnsOwning)
	{
		writeStringToFile("*", file);
	}
	writeSpaceToFile(file);
}

//void writeDelegateImpl_InitRes(DelegateNode* delegateNode, FILE* file, int indentation)
//{
//	if (0 != delegateNode->m_resultConst)
//	{
//		generateCode_Token(file, delegateNode->m_resultConst, indentation);
//		generateCode_TypeName(file, delegateNode->m_resultTypeName, delegateNode->m_enclosing, true, 0);
//	}
//	else
//	{
//		generateCode_TypeName(file, delegateNode->m_resultTypeName, delegateNode->m_enclosing, true, indentation);
//	}
//	if (0 != delegateNode->m_passing)
//	{
//		generateCode_Token(file, delegateNode->m_passing, 0);
//	}
//	writeStringToFile(" __res__;\n", file, 0);
//}

void writeDelegateImpl_InitResult(DelegateNode* delegateNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	CalcTypeNativeName(typeName, delegateNode->m_resultTypeName, 0);
	if (delegateNode->byUniquePtr())
	{
		sprintf_s(buf, "::pafcore::UniquePtr<%s> __res__;\n", typeName.c_str());
	}
	else if (delegateNode->bySharedPtr())
	{
		sprintf_s(buf, "::pafcore::SharedPtr<%s> __res__;\n", typeName.c_str());
	}
	else
	{
		const char* sign;
		if (delegateNode->byValue())
		{
			sign = "";
		}
		else
		{
			sign = "*";
		}
		sprintf_s(buf, "%s%s __res__;\n", typeName.c_str(), sign);
	}
	writeStringToFile(buf, file, indentation);
}

void writeDelegateImpl_ReturnResult(DelegateNode* delegateNode, FILE* file, int indentation)
{
	if (delegateNode->byRef())
	{
		writeStringToFile("return *__res__;\n", file, indentation);
	}
	else
	{
		writeStringToFile("return __res__;\n", file, indentation);
	}
}

void writeDelegateImpl_CastResult(DelegateNode* delegateNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	TypeCategory typeCategory = CalcTypeNativeName(typeName, delegateNode->m_resultTypeName, 0);

	TypeNameNode* resultNode = static_cast<TypeNameNode*>(delegateNode->m_resultTypeName);
	if (delegateNode->byValue())
	{
		switch (typeCategory)
		{
			//case void_type: impossible
		case primitive_type:
			sprintf_s(buf, "__result__.castToPrimitive(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &__res__);\n", typeName.c_str());
			break;
		case enum_type:
			sprintf_s(buf, "__result__.castToEnum(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &__res__);\n", typeName.c_str());
			break;
		case value_type:
			sprintf_s(buf, "__result__.castToValue(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &__res__);\n", typeName.c_str());
			break;
		case rc_object_type:
			sprintf_s(buf, "__result__.castToRcObject(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), &__res__);\n", typeName.c_str());
			break;
		default:
			assert(false);
		}
		writeStringToFile(buf, file, indentation);
	}
	else
	{
		if (delegateNode->byUniquePtr())
		{
			sprintf_s(buf, "%s* __res_ptr__ = 0;\n", typeName.c_str());
			writeStringToFile(buf, file, indentation);
			switch (typeCategory)
			{
			case value_type:
				sprintf_s(buf, "__result__.castToValuePtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res_ptr__);\n", typeName.c_str());
				break;
			case rc_object_type:
				sprintf_s(buf, "__result__.castToRcPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res_ptr__);\n", typeName.c_str());
				break;
			default:
				assert(false);
			}
			writeStringToFile(buf, file, indentation);
			writeStringToFile("__result__.unhold();\n", file, indentation);
			sprintf_s(buf, "__res__ = ::pafcore::UniquePtr<%s>(__res_ptr__);\n", typeName.c_str());
			writeStringToFile(buf, file, indentation);
			return;
		}
		if (delegateNode->bySharedPtr())
		{
			sprintf_s(buf, "%s* __res_ptr__ = 0;\n", typeName.c_str());
			writeStringToFile(buf, file, indentation);
			sprintf_s(buf, "__result__.castToRcPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res_ptr__);\n", typeName.c_str());
			writeStringToFile(buf, file, indentation);
			writeStringToFile("__result__.unhold();\n", file, indentation);
			sprintf_s(buf, "__res__ = ::pafcore::SharedPtr<%s>(__res_ptr__);\n", typeName.c_str());
			writeStringToFile(buf, file, indentation);
			return;
		}

		switch (typeCategory)
		{
		case void_type:
			sprintf_s(buf, "__result__.castToVoidPtr(&__res__);\n");
			break;
		case primitive_type:
			sprintf_s(buf, "__result__.castToPrimitivePtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res__);\n", typeName.c_str());
			break;
		case enum_type:
			sprintf_s(buf, "__result__.castToEnumPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res__);\n", typeName.c_str());
			break;
		case value_type:
			sprintf_s(buf, "__result__.castToValuePtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res__);\n", typeName.c_str());
			break;
		case rc_object_type:
			sprintf_s(buf, "__result__.castToRcPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton(), (void**)&__res__);\n", typeName.c_str());
			break;
		default:
			assert(false);
		}
		writeStringToFile(buf, file, indentation);
		if (delegateNode->returnsOwning())
		{
			writeStringToFile("__result__.unhold();\n", file, indentation);
		}
	}
}

void writeDelegateImpl_SetResultType(DelegateNode* delegateNode, FILE* file, int indentation)
{
	char buf[4096];
	std::string typeName;
	TypeCategory typeCategory = CalcTypeNativeName(typeName, delegateNode->m_resultTypeName, 0);

	if (delegateNode->byValue() && (primitive_type == typeCategory || enum_type == typeCategory))
	{
		if (primitive_type == typeCategory)
		{
			sprintf_s(buf, "__result__.assignNullPrimitive(RuntimeTypeOf<%s>::RuntimeType::GetSingleton());\n", typeName.c_str());
		}
		else
		{
			sprintf_s(buf, "__result__.assignNullEnum(RuntimeTypeOf<%s>::RuntimeType::GetSingleton());\n", typeName.c_str());
		}
	}
	else
	{
		sprintf_s(buf, "__result__.assignNullPtr(RuntimeTypeOf<%s>::RuntimeType::GetSingleton());\n", typeName.c_str());
	}
	writeStringToFile(buf, file, indentation);
}

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

	std::string headerName = cppName;
	ConvertFileBaseNameToSnakeCase(headerName);
	sprintf_s(buf, "#include \"%s.h\"\n", headerName.c_str());
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
		case snt_delegate:
			generateCode_Delegate(file, static_cast<DelegateNode*>(memberNode), namespaceNode, "", indentation + 1);
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
		std::vector<IdentifyNode*> templateParameterNodes;
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

		if (!classNode->isValueType())
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
			if (classNode->derivesFromObject(0) && typeName != "Object")
			{
				writeStringToFile("PAF_ASSERT(0 == paf_base_offset_of(", file, indentation + 1);
				writeStringToFile(typeName.c_str(), file);
				writeStringToFile(", ::pafcore::Object));\n", file);
			}
			writeStringToFile("return ::RuntimeTypeOf<", file, indentation + 1);
			writeStringToFile(typeName.c_str(), file);
			writeStringToFile(">::RuntimeType::GetSingleton();\n", file);
			writeStringToFile("}\n\n", file, indentation);

			if (classNode->needGetAddress(0))
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
				writeStringToFile("return reinterpret_cast<size_t>(static_cast<::pafcore::Object*>(this));\n", file, indentation + 1);
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
		case snt_delegate:
			generateCode_Delegate(file, static_cast<DelegateNode*>(memberNode), classNode, typeName, indentation);
			break;
		}
	}
}

void SourceFileGenerator::generateCode_Delegate(FILE* file, DelegateNode* delegateNode, ScopeNode* scopeNode, const std::string& scopeClassName, int indentation)
{
	char buf[4096];
	if (delegateNode->isNoCode())
	{
		file = 0;
	}
	TypeNode* resultTypeNode = delegateNode->m_resultTypeName->getTypeNode(0);
	bool isVoid = (void_type == resultTypeNode->getTypeCategory(0) && 0 == delegateNode->m_typeCompound && 0 == delegateNode->m_byRef);

	std::string typeName = scopeClassName + delegateNode->m_name->m_str;

	if (0 != delegateNode->m_resultConst)
	{
	}
	generateCode_SourceResultType(file, delegateNode->m_resultTypeName, delegateNode->m_enclosing,
		0 != delegateNode->m_resultConst, delegateNode->m_typeCompound, delegateNode->m_byRef, delegateNode->returnsOwning(), indentation);
	writeStringToFile(typeName.c_str(), file);
	writeStringToFile("::invoke", file);
	generateCode_Token(file, delegateNode->m_leftParenthesis, 0);
	generateCode_ParameterList(file, delegateNode->m_parameterList, delegateNode->m_enclosing);
	generateCode_Token(file, delegateNode->m_rightParenthesis, 0);
	writeStringToFile("\n", file);
	writeStringToFile("{\n", file, indentation);

	std::vector<ParameterNode*> parameterNodes;
	delegateNode->m_parameterList->collectParameterNodes(parameterNodes);
	size_t paramCount = parameterNodes.size();

	writeStringToFile("bool __arguments_init__ = false;\n", file, indentation + 1);
	if (!isVoid)
	{
		writeStringToFile("bool __res_init__ = false;\n", file, indentation + 1);
		writeDelegateImpl_InitResult(delegateNode, file, indentation + 1);
	}
	
	writeStringToFile("pafcore::Variant __result__;\n", file, indentation + 1);
	if (0 < paramCount)
	{
		sprintf_s(buf, "::pafcore::Variant __arguments__[%zu];\n", paramCount);
		writeStringToFile(buf, file, indentation + 1);
	}
	sprintf_s(buf, "::pafcore::Variant* __args__[%zu] = {0", paramCount + 1);
	writeStringToFile(buf, file, indentation + 1);
	for (size_t i = 0; i < paramCount; ++i)
	{
		sprintf_s(buf, ", &__arguments__[%zu]", i);
		writeStringToFile(buf, file, 0);
	}
	writeStringToFile("};\n", file, 0);

	writeStringToFile("::pafcore::CallBack* __callBack__ = getCallBack();\n", file, indentation + 1);
	writeStringToFile("while (__callBack__)\n", file, indentation + 1);
	writeStringToFile("{\n", file, indentation + 1);
	writeStringToFile("::pafcore::CallBack* __next__ = __callBack__->getNext();\n", file, indentation + 2);
	writeStringToFile("::pafcore::ClassType* __classType__ = __callBack__->getType();\n", file, indentation + 2);
	writeStringToFile("if (__classType__ == ::pafcore::FunctionCallBack::GetType())\n", file, indentation + 2);
	writeStringToFile("{\n", file, indentation + 2);
	writeStringToFile("::pafcore::FunctionCallBack* __functionCallBack__ = static_cast<::pafcore::FunctionCallBack*>(__callBack__);\n", file, indentation + 3);
	writeStringToFile(isVoid ? "" : "__res__ = ", file, indentation + 3);
	writeStringToFile("(*reinterpret_cast<CallBackFunction>(reinterpret_cast<size_t>(__functionCallBack__->m_function)))(__functionCallBack__->m_userData", file, 0);
	for (size_t i = 0; i < paramCount; ++i)
	{
		sprintf_s(buf, ", %s", parameterNodes[i]->m_name->m_str.c_str());
		writeStringToFile(buf, file, 0);
	}
	writeStringToFile(");\n", file, 0);
	writeStringToFile("}\n", file, indentation + 2);

	writeStringToFile("else\n", file, indentation + 2);
	writeStringToFile("{\n", file, indentation + 2);

	writeStringToFile("if(!__arguments_init__)\n", file, indentation + 3);
	writeStringToFile("{\n", file, indentation + 3);
	writeStringToFile("__arguments_init__ = true;\n", file, indentation + 4);
	//if (!isVoid)
	//{
	//	writeDelegateImpl_SetResultType(delegateNode, file, indentation + 4);
	//}
	for (size_t i = 0; i < paramCount; ++i)
	{
		ParameterNode* parameterNode = parameterNodes[i];
		if (parameterNode->isInput())
		{
			writeInterfaceMethodImpl_AssignInputParam(parameterNode, i, file, indentation + 4);
		}
		else
		{
			writeInterfaceMethodImpl_SetOutputParamType(parameterNode, i, file, indentation + 4);
		}
	}
	writeStringToFile("}\n", file, indentation + 3);

	sprintf_s(buf, "__callBack__->invoke(&__result__, __args__, %zu);\n", paramCount + 1);
	writeStringToFile(buf, file, indentation + 3);

	if (!isVoid)
	{
		writeStringToFile("if(!__res_init__)\n", file, indentation + 3);
		writeStringToFile("{\n", file, indentation + 3);
		writeStringToFile("__res_init__ = false;\n", file, indentation + 4);
		for (size_t i = 0; i < paramCount; ++i)
		{
			ParameterNode* parameterNode = parameterNodes[i];
			if (parameterNode->isOutput())
			{
				writeInterfaceMethodImpl_CastOutputParam(parameterNode, i, file, indentation + 4);
			}
		}
		writeDelegateImpl_CastResult(delegateNode, file, indentation + 4);
		writeStringToFile("}\n", file, indentation + 3);
	}
	writeStringToFile("}\n", file, indentation + 2);
	writeStringToFile("__callBack__ = __next__;\n", file, indentation + 2);
	writeStringToFile("}\n", file, indentation + 1);//while
	if (!isVoid)
	{
		writeStringToFile("return __res__;\n", file, indentation + 1);
	}
	writeStringToFile("}\n", file, indentation);

}

void SourceFileGenerator::generateCode_TemplateHeader(FILE* file, ClassNode* classNode, int indentation)
{
	if(classNode->m_templateParametersNode)
	{
		std::vector<IdentifyNode*> templateParameterNodes;
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
		generateCode_SourceResultType(file, methodNode->m_resultTypeName, classNode->m_enclosing,
			0 != methodNode->m_resultConst, methodNode->m_typeCompound, methodNode->m_byRef, methodNode->returnsOwning(), 0);
	}
	else
	{
		generateCode_SourceResultType(file, methodNode->m_resultTypeName, classNode->m_enclosing,
			0 != methodNode->m_resultConst, methodNode->m_typeCompound, methodNode->m_byRef, methodNode->returnsOwning(), indentation);
	}
	writeStringToFile(typeName.c_str(), file);
	writeStringToFile("::", file);

	generateCode_Identify(file, methodNode->m_name, 0);

	generateCode_Token(file, methodNode->m_leftParenthesis, 0);
	std::vector<ParameterNode*> parameterNodes;

	methodNode->m_parameterList->collectParameterNodes(parameterNodes);
	size_t parameterCount = parameterNodes.size();
	for (size_t i = 0; i < parameterCount; ++i)
	{
		if (0 != i)
		{
			writeStringToFile(", ", file);
		}
		if ((methodNode->m_name->m_str == "Delete" || methodNode->m_name->m_str == "DeleteArray")
			&& 0 == i)
		{
			generateCode_TypeName(file, parameterNodes[i]->m_typeName, classNode->m_enclosing, true, 0);
			writeStringToFile("* ", file);
			writeStringToFile(parameterNodes[i]->m_name->m_str.c_str(), file);
		}
		else
		{
			generateCode_Parameter(file, parameterNodes[i], classNode->m_enclosing);
		}
	}
	generateCode_Token(file, methodNode->m_rightParenthesis, 0);
	writeStringToFile("\n", file);
	writeStringToFile("{\n", file, indentation);

	if ("New" == methodNode->m_name->m_str)
	{
		if (classNode->isValueType())
		{
			sprintf_s(buf, "return ::pafcore::Create<%s>(", typeName.c_str());
		}
		else
		{
			sprintf_s(buf, "return ::pafcore::CreateObject<%s>(", typeName.c_str());
		}
	}
	else if ("Delete" == methodNode->m_name->m_str)
	{
		sprintf_s(buf, "::pafcore::DeleteObject(value);\n");
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("}\n\n", file, indentation);
		return;
	}
	else if ("DeleteArray" == methodNode->m_name->m_str)
	{
		sprintf_s(buf, "::pafcore::DeleteObjectArray(value, count);\n");
		writeStringToFile(buf, file, indentation + 1);
		writeStringToFile("}\n\n", file, indentation);
		return;
	}
	else
	{
		assert("NewArray" == methodNode->m_name->m_str);
		assert(classNode->isValueType());
		sprintf_s(buf, "return ::pafcore::CreateArray<%s>(", typeName.c_str());
	}

	writeStringToFile(buf, file, indentation + 1);
	for (size_t i = 0; i < parameterCount; ++i)
	{
		if (i != 0)
		{
			writeStringToFile(", ", file);
		}
		writeStringToFile(parameterNodes[i]->m_name->m_str.c_str(), file);
	}
	writeStringToFile(");\n", file);
	
	writeStringToFile("}\n\n", file, indentation);
}

