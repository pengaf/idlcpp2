#include "header_file_generator.h"
#include "utility.h"
#include "source_file.h"
#include "program_node.h"
#include "namespace_node.h"
#include "token_node.h"
#include "compound_type_node.h"
#include "identifier_node.h"
#include "enumerator_list_node.h"
#include "scope_name_list_node.h"
#include "scope_name_node.h"
#include "member_list_node.h"
#include "variable_list_node.h"
#include "variable_node.h"
#include "compound_type_node.h"

#include "enumerator_node.h"
#include "enum_node.h"
#include "class_node.h"
#include "template_parameters_node.h"
#include "type_name_list_node.h"
#include "type_name_node.h"
#include "field_node.h"
#include "property_accessor_node.h"
#include "property_accessor_list_node.h"
#include "property_node.h"
#include "method_node.h"
#include "typedef_node.h"
#include "compiler.h"
#include "options.h"
#include "template_class_instance_node.h"
#include "type_tree.h"
#include <assert.h>
#include "code_generator.h"

void HeaderFileGenerator::generateCode(FILE* dstFile, SourceFile* sourceFile)
{
	generateCode_Program(dstFile, sourceFile);
}

void HeaderFileGenerator::generateCode_Program(FILE* file, SourceFile* sourceFile)
{
	writeStringToFile("#pragma once\n\n", file);
	g_compiler.outputUsedTypes(file, sourceFile);
	writeStringToFile("namespace pafcore{ class ClassType; }\n", file);
	if (sourceFile->m_hasListProperty)
	{
		writeStringToFile("namespace pafcore{ class Iterator; }\n", file);
	}

	generateCode_Namespace(file, sourceFile->m_syntaxTree, -1);

	g_compiler.outputEmbededCodes(file, 0);
}

void HeaderFileGenerator::generateCode_Namespace(FILE* file, NamespaceNode* namespaceNode, int indentation)
{
	if (namespaceNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, namespaceNode->m_filter);
		file = 0;
	}

	if(!namespaceNode->isGlobalNamespace())
	{
		generateCode_Token(file, namespaceNode->m_keyword, indentation, true);
		generateCode_Identifier(file, namespaceNode->m_name, 0, true);
		generateCode_Token(file, namespaceNode->m_leftBrace, indentation, true);
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
			generateCode_Enum(file, static_cast<EnumNode*>(memberNode), indentation + 1);
			break;
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(memberNode), indentation + 1);
			break;
		case snt_template_class_instance:
			break;
		case snt_typedef:
			generateCode_Typedef(file, static_cast<TypedefNode*>(memberNode), indentation + 1);
			break;
		case snt_namespace:
			generateCode_Namespace(file, static_cast<NamespaceNode*>(memberNode), indentation + 1);
			break;
		case snt_type_declaration:
			break;
		default:
			assert(false);
		}
	}

	if(!namespaceNode->isGlobalNamespace())
	{
		generateCode_Token(file, namespaceNode->m_rightBrace, indentation, true);
	}
}


void HeaderFileGenerator::generateCode_Typedef(FILE* file, TypedefNode* typedefNode, int indentation)
{
	if (typedefNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, typedefNode->m_filter);
		file = 0;
	}
	if(0 != typedefNode->m_typeName)
	{
		generateCode_Token(file, typedefNode->m_keyword, indentation, true);
		generateCode_TypeName(file, typedefNode->m_typeName, typedefNode->m_enclosing, true, 0, true);
		writeSpaceToFile(file);
		generateCode_Identifier(file, typedefNode->m_name, 0, true);
		generateCode_Token(file, typedefNode->m_semicolon, 0, true);
	}
}

void HeaderFileGenerator::generateCode_Enum(FILE* file, EnumNode* enumNode, int indentation)
{
	if (enumNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, enumNode->m_filter);
		file = 0;
	}
	else if (enumNode->m_nativeName)
	{
		g_compiler.outputEmbededCodes(file, enumNode->m_keyword);
		file = 0;
	}

	generateCode_Token(file, enumNode->m_keyword, indentation, true);
	if (enumNode->m_keyword2)
	{
		generateCode_Token(file, enumNode->m_keyword2, 0, true);
	}
	generateCode_Identifier(file, enumNode->m_name, 0, true);
	generateCode_Token(file, enumNode->m_leftBrace, indentation, true);

	std::vector<std::pair<TokenNode*, EnumeratorNode*>> enumeratorNodes;
	enumNode->m_enumeratorList->collectEnumeratorNodes(enumeratorNodes);
	//size_t itemCount = enumeratorNodes.size();
	for(auto item: enumeratorNodes)
	{
		if(item.first)
		{
			generateCode_Token(file, item.first, 0, true);
		}
		generateCode_Identifier(file, item.second->m_name, indentation + 1, true);
		if (item.second->m_equalSign)
		{
			generateCode_Token(file, item.second->m_equalSign, 0, true);
		}
	}
	generateCode_Token(file, enumNode->m_rightBrace, indentation, true);
	generateCode_Token(file, enumNode->m_semicolon, 0, true);
}

void HeaderFileGenerator::generateCode_Class(FILE* file, ClassNode* classNode, int indentation)
{
	if (classNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, classNode->m_filter);
		file = 0;
	}
	else if (classNode->m_nativeName)
	{
		g_compiler.outputEmbededCodes(file, classNode->m_keyword);
		file = 0;
	}

	if(classNode->m_templateParametersNode)
	{
		generateCode_Token(file, classNode->m_templateParametersNode->m_keyword, indentation, true);
		generateCode_Token(file, classNode->m_templateParametersNode->m_leftBracket, 0, true);
		std::vector<std::pair<TokenNode*, IdentifierNode*>> parameterNodes;
		classNode->m_templateParametersNode->collectParameterNodes(parameterNodes);

		size_t count = parameterNodes.size();
		for(size_t i = 0; i < count; ++i)
		{
			if(parameterNodes[i].first)
			{
				generateCode_Token(file, parameterNodes[i].first, 0, true);
			}
			writeStringToFile("typename ", file);
			generateCode_Identifier(file, parameterNodes[i].second, 0, true);
		}
		generateCode_Token(file, classNode->m_templateParametersNode->m_rightBracket, 0, true);
	}

	generateCode_Token(file, classNode->m_keyword, indentation, true);
	generateCode_Identifier(file, classNode->m_name, 0, true);


	if(classNode->m_baseList)
	{
		std::vector<std::pair<TokenNode*, TypeNameNode*>> typeNameNodes;
		classNode->m_baseList->collectTypeNameNodesNotNoCode(typeNameNodes);
		size_t baseCount = typeNameNodes.size();
		if (baseCount)
		{
			assert(0 != classNode->m_colon);
			writeSpaceToFile(file);
			generateCode_Token(file, classNode->m_colon, 0, true);
			writeSpaceToFile(file);
			for(size_t i = 0; i < baseCount; ++i)
			{
				if(typeNameNodes[i].first && 0 != i)
				{
					generateCode_Token(file, typeNameNodes[i].first, 0, true);
				}
				writeStringToFile("public ", file);
				generateCode_TypeName(file, typeNameNodes[i].second, classNode->m_enclosing, false, 0, true);
			}
		}
	}

	generateCode_Token(file, classNode->m_leftBrace, indentation, true);
	writeStringToFile("\n", file);
	writeStringToFile("public:\n", file, indentation);

	std::vector<MemberNode*> memberNodes;
	classNode->m_memberList->collectMemberNodes(memberNodes);

	auto it = classNode->m_additionalMethods.begin();
	auto end = classNode->m_additionalMethods.end();
	for (; it != end; ++it)
	{
		MethodNode* methodNode = *it;
		if (!methodNode->isNoCode())
		{
			memberNodes.push_back(methodNode);
		}
	}

	size_t memberCount = memberNodes.size();
	for (size_t i = 0; i < memberCount; ++i)
	{
		char buf[4096];
		MemberNode* memberNode = memberNodes[i];
		switch (memberNode->m_nodeType)
		{
		case snt_class:
		{
			ClassNode* nestedClassNode = static_cast<ClassNode*>(memberNode);
			if (0 == nestedClassNode->m_nativeName)
			{
				sprintf_s(buf, "%s %s;\n", KeywardTokenToString(nestedClassNode->m_keyword),
					nestedClassNode->m_name->m_str.c_str());
				writeStringToFile(buf, file, indentation + 1);
			}
		}
		break;
		case snt_enum:
		{
			EnumNode* nestedEnumNode = static_cast<EnumNode*>(memberNode);
			if (0 == nestedEnumNode->m_nativeName)
			{
				sprintf_s(buf, "%s %s%s;\n", KeywardTokenToString(nestedEnumNode->m_keyword),
					nestedEnumNode->m_keyword2 ? "class " : "",
					nestedEnumNode->m_name->m_str.c_str());
				writeStringToFile(buf, file, indentation + 1);
			}
		}
		break;
		}
	}
	if (!classNode->isNoMeta())
	{
		writeStringToFile("static ::pafcore::ClassType* GetType();\n", file, indentation + 1);
		if (classNode->isDerivedFromObject() || classNode->isDerivedFromInterface())
		{
			writeStringToFile("virtual ::pafcore::ClassType* getType();\n", file, indentation + 1);
		}
		if (classNode->isDerivedFromInterface())
		{
			writeStringToFile("virtual size_t getAddress();\n", file, indentation + 1);
		}
	}
	for(size_t i = 0; i < memberCount; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		switch(memberNode->m_nodeType)
		{
		case snt_field:
			generateCode_Field(file, static_cast<FieldNode*>(memberNode), indentation + 1);
			break;
		case snt_property:
			generateCode_Property(file, static_cast<PropertyNode*>(memberNode), indentation + 1);
			break;
		case snt_method:
			generateCode_Method(file, static_cast<MethodNode*>(memberNode), indentation + 1);
			break;
		case snt_class:
			generateCode_Class(file, static_cast<ClassNode*>(memberNode), indentation + 1);
			break;
		case snt_enum:
			generateCode_Enum(file, static_cast<EnumNode*>(memberNode), indentation + 1);
			break;
		case snt_typedef:
			generateCode_Typedef(file, static_cast<TypedefNode*>(memberNode), indentation + 1);
			break;
		case snt_type_declaration:
			break;
		default:
			assert(false);
		}
	}

	generateCode_Token(file, classNode->m_rightBrace, indentation, true);
	generateCode_Token(file, classNode->m_semicolon, 0, true);
	writeStringToFile("\n", file);
	//if (!classNode->isValueType() && classNode->derivesFromObject(0))
	//{
	//	std::string typeName;
	//	GetClassName(typeName, classNode);
	//	if (typeName != "Object")
	//	{
	//		writeStringToFile("static_assert(std::is_base_of_v<::pafcore::Object, ", file, indentation);
	//		writeStringToFile(typeName.c_str(), file);
	//		writeStringToFile(">, \"Type must derive from pafcore::Object.\");\n", file);
	//	}
	//}
}

void HeaderFileGenerator::generateCode_Field(FILE* file, FieldNode* fieldNode, int indentation)
{
	if (fieldNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, fieldNode->m_filter);
		file = 0;
	}
	else if (fieldNode->m_nativeName)
	{
		TokenNode* firstToken;
		if (0 != fieldNode->m_modifier)
		{
			firstToken = fieldNode->m_modifier;
		}
		else
		{
			if (fieldNode->m_compoundType->m_typeName->m_keyword)
			{
				firstToken = fieldNode->m_compoundType->m_typeName->m_keyword;
			}
			else
			{
				firstToken = fieldNode->m_compoundType->m_typeName->m_scopeNameList->m_scopeName->m_name;
			}
		}
		g_compiler.outputEmbededCodes(file, firstToken);
		file = 0;
	}
	ClassNode* classNode = static_cast<ClassNode*>(fieldNode->m_enclosing);
	if(fieldNode->m_modifier)
	{
		generateCode_Token(file, fieldNode->m_modifier, indentation, true);
		indentation = 0;
	}
	generateCode_CompoundType(file, fieldNode->m_compoundType, fieldNode->m_enclosing, indentation, true);

	writeSpaceToFile(file);
	generateCode_Identifier(file, fieldNode->m_name, 0, true);
	if(fieldNode->m_leftBracket)
	{
		generateCode_Token(file, fieldNode->m_leftBracket, 0, true);
	}
	if(fieldNode->m_rightBracket)
	{
		generateCode_Token(file, fieldNode->m_rightBracket, 0, true);
	}
	generateCode_Token(file, fieldNode->m_semicolon, 0, true);
}

void HeaderFileGenerator::generateCode_Property_Get(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->m_get->m_nativeName)
	{
		TokenNode* firstToken;
		if (0 != propertyNode->m_modifier)
		{
			firstToken = propertyNode->m_modifier;
		}
		else
		{
			if (propertyNode->m_compoundType->m_typeName->m_keyword)
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_keyword;
			}
			else
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_scopeNameList->m_scopeName->m_name;
			}
		}
		g_compiler.outputEmbededCodes(file, firstToken);
		file = 0;
	}

	ClassNode* classNode = static_cast<ClassNode*>(propertyNode->m_enclosing);
	if(propertyNode->m_modifier)
	{
		generateCode_Token(file, propertyNode->m_modifier, indentation, true);
		indentation = 0;
	}
	generateCode_ResultType(file, propertyNode->m_compoundType, propertyNode->m_get->m_byRef, propertyNode->m_enclosing, true, indentation, true);
	generateCode_Identifier(file, propertyNode->m_name, 0, true);

	writeStringToFile("(", file);

	if (!propertyNode->isSimple())
	{
		writeStringToFile("::pafcore::Iterator*", file);
	}
	if (propertyNode->isStatic())
	{
		writeStringToFile(");", file);
	}
	else
	{
		writeStringToFile(") const;", file);
	}
}

void HeaderFileGenerator::generateCode_Property_Set(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->m_set->m_nativeName)
	{
		TokenNode* firstToken;
		if (0 != propertyNode->m_modifier)
		{
			firstToken = propertyNode->m_modifier;
		}
		else
		{
			if (propertyNode->m_compoundType->m_typeName->m_keyword)
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_keyword;
			}
			else
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_scopeNameList->m_scopeName->m_name;
			}
		}
		g_compiler.outputEmbededCodes(file, firstToken);
		file = 0;
	}

	ClassNode* classNode = static_cast<ClassNode*>(propertyNode->m_enclosing);

	if(propertyNode->m_modifier)
	{
		generateCode_Token(file, propertyNode->m_modifier, indentation, true);
		indentation = 0;
	}
	if (propertyNode->isStatic())
	{
		generateCode_Identifier(file, "void ", indentation);
	}
	else
	{
		generateCode_Identifier(file, classNode->m_name, indentation, true);
		writeStringToFile("& ", file);
	}
	generateCode_Identifier(file, propertyNode->m_name, 0, true);
	
	writeStringToFile("(", file);
	
	if (!propertyNode->isSimple())
	{
		writeStringToFile("::pafcore::Iterator*, ", file);
	}
	generateCode_ResultType(file, propertyNode->m_compoundType, propertyNode->m_set->m_byRef, propertyNode->m_enclosing, true, 0, true);
	writeStringToFile(");", file);	
}

void HeaderFileGenerator::generateCode_Property_Enum(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->m_set->m_nativeName)
	{
		TokenNode* firstToken;
		if (0 != propertyNode->m_modifier)
		{
			firstToken = propertyNode->m_modifier;
		}
		else
		{
			if (propertyNode->m_compoundType->m_typeName->m_keyword)
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_keyword;
			}
			else
			{
				firstToken = propertyNode->m_compoundType->m_typeName->m_scopeNameList->m_scopeName->m_name;
			}
		}
		g_compiler.outputEmbededCodes(file, firstToken);
		file = 0;
	}

	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	CompoundTypeNode* enumResultType = (CompoundTypeNode*)newCompoundType(propertyNode->m_compoundType->m_typeName, tc_shared_array);

	generateCode_ResultType(file, enumResultType, nullptr, propertyNode->m_enclosing, false, indentation, true);
	writeStringToFile("enum_", file, 0);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	if (propertyNode->isStatic())
	{
		writeStringToFile("();", file);
	}
	else
	{
		writeStringToFile("() const;", file);
	}
}

void HeaderFileGenerator::generateCode_Property_Size(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	writeStringToFile("size_t size_", file, indentation);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	if (propertyNode->isStatic())
	{
		writeStringToFile("();", file);
	}
	else
	{
		writeStringToFile("() const;", file);
	}
}

void HeaderFileGenerator::generateCode_Property_Resize(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	writeStringToFile("void resize_", file, indentation);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	writeStringToFile("(size_t);", file);
}

void HeaderFileGenerator::generateCode_Property_Iterate(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	writeStringToFile("::pafcore::SharedPtr<::pafcore::Iterator> iterate_", file, indentation);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	writeStringToFile("();", file);
}

void HeaderFileGenerator::generateCode_Property_Insert(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	TokenNode* byRef = (TokenNode*)newToken('&');

	writeStringToFile("void insert_", file, indentation);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	writeStringToFile("(::pafcore::Iterator*, ", file);
	generateCode_ResultType(file, propertyNode->m_compoundType, byRef, propertyNode->m_enclosing, true, 0, true);
	writeStringToFile(");", file);
}

void HeaderFileGenerator::generateCode_Property_Erase(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isStatic())
	{
		writeStringToFile("static ", file, indentation);
		indentation = 0;
	}
	writeStringToFile("void erase_", file, indentation);
	writeStringToFile(propertyNode->m_name->m_str.c_str(), file);
	writeStringToFile("(::pafcore::Iterator*);", file);
}

void HeaderFileGenerator::generateCode_Property(FILE* file, PropertyNode* propertyNode, int indentation)
{
	if (propertyNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, propertyNode->m_filter);
		file = 0;
	}
	g_compiler.outputEmbededCodes(file, propertyNode->m_name);

	std::vector<PropertyAccessorNode*> propertyAccessors;
	propertyNode->m_accessorList->collectPropertyAccessors(propertyAccessors);
	for (PropertyAccessorNode* accessor : propertyAccessors)
	{
		if (accessor == propertyNode->m_get)
		{
			generateCode_Property_Get(file, propertyNode, indentation);
			writeStringToFile("\n", file);
		}
		if (accessor == propertyNode->m_set)
		{
			generateCode_Property_Set(file, propertyNode, indentation);
			writeStringToFile("\n", file);
		}
		if (accessor == propertyNode->m_enumerate)
		{
			generateCode_Property_Enum(file, propertyNode, indentation);
			writeStringToFile("\n", file);
		}
	}
	
	if (propertyNode->isFixedArray() || propertyNode->isDynamicArray())
	{
		generateCode_Property_Iterate(file, propertyNode, indentation);
		writeStringToFile("\n", file);
		generateCode_Property_Size(file, propertyNode, indentation);
		writeStringToFile("\n", file);
		if (propertyNode->isDynamicArray())
		{
			generateCode_Property_Resize(file, propertyNode, indentation);
			writeStringToFile("\n", file);
		}
	}
	else if (propertyNode->isList())
	{
		generateCode_Property_Iterate(file, propertyNode, indentation);
		writeStringToFile("\n", file);
		generateCode_Property_Insert(file, propertyNode, indentation);
		writeStringToFile("\n", file);
		generateCode_Property_Erase(file, propertyNode, indentation);
		writeStringToFile("\n", file);
	}

};

void HeaderFileGenerator::generateCode_Method(FILE* file, MethodNode* methodNode, int indentation)
{
	std::vector<VariableNode*> resultNodes;
	if (methodNode->m_resultList)
	{
		methodNode->m_resultList->collectVariableNodes(resultNodes);
	}
	if (methodNode->isNoCode())
	{
		g_compiler.outputEmbededCodes(file, methodNode->m_filter);
		file = 0;
	}
	else if (methodNode->m_nativeName)
	{
		TokenNode* firstToken;
		if (methodNode->m_modifier)
		{
			firstToken = methodNode->m_modifier;
		}
		else if (methodNode->m_resultList)
		{
			TypeNameNode* resultTypeName = resultNodes.front()->m_compoundType->m_typeName;
			if (resultTypeName->m_keyword)
			{
				firstToken = resultTypeName->m_keyword;
			}
			else
			{
				firstToken = resultTypeName->m_scopeNameList->m_scopeName->m_name;
			}
		}
		else
		{
			firstToken = methodNode->m_name;
		}
		g_compiler.outputEmbededCodes(file, firstToken);
		file = 0;
	}

	ClassNode* classNode = static_cast<ClassNode*>(methodNode->m_enclosing);
	if(classNode->isAdditionalMethod(methodNode))
	{
		writeStringToFile("\n", file);
	}
	if(methodNode->m_modifier)
	{
		generateCode_Token(file, methodNode->m_modifier, indentation, true);
		indentation = 0;
	}
	if(methodNode->m_resultList)
	{
		generateCode_ResultType(file, resultNodes.front()->m_compoundType,
			resultNodes.front()->m_byRef, methodNode->m_enclosing, true, indentation, true);
		indentation = 0;
	}
	else
	{
		if (methodNode->m_name->m_str != methodNode->m_enclosing->m_name->m_str)
		{
			generateCode_outputEmbededCodes(file, methodNode->m_name);
			generateCode_Identifier(file, "void ", indentation);
			indentation = 0;
		}
	}

	generateCode_Identifier(file, methodNode->m_name, indentation, true);
	generateCode_Token(file, methodNode->m_leftParenthesis, 0, true);
	if (resultNodes.size() > 1)
	{
		for (size_t i = 1; i < resultNodes.size(); ++i)
		{
			generateCode_Parameter(file, resultNodes[i], true, methodNode->m_enclosing, true);
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
				generateCode_Token(file, parameterNodes[i].first, 0, true);
			}
			generateCode_Parameter(file, parameterNodes[i].second, false, methodNode->m_enclosing, true);
		}
	}
	generateCode_Token(file, methodNode->m_rightParenthesis, 0, true);
	if (methodNode->m_constant)
	{
		generateCode_Token(file, methodNode->m_constant, 0, true);
	}
	generateCode_Token(file, methodNode->m_semicolon, 0, true);
}

