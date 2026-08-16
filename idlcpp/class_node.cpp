#include "class_node.h"
#include "type_name_node.h"
#include "scope_name_node.h"
#include "scope_name_list_node.h"
#include "identifier_node.h"
#include "identifier_list_node.h"
#include "type_name_list_node.h"
#include "variable_node.h"
#include "variable_list_node.h"
#include "compound_type_node.h"
#include "template_parameters_node.h"
#include "template_class_instance_node.h"
#include "member_node.h"
#include "member_list_node.h"
#include "method_node.h"
#include "property_node.h"
#include "field_node.h"
#include "namespace_node.h"
#include "program_node.h"
#include "error_list.h"
#include "raise_error.h"
#include "options.h"
#include "compiler.h"
#include "type_tree.h"
#include "utility.h"

#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

namespace
{
	struct BaseStat
	{
		int offset = 0;
		std::vector<int> objects;
		std::vector<int> interfaces;
		//std::vector<int> otherClasses;
		//std::vector<int> notClasses;
	};

	void StatBaseClass(BaseStat& stat, TypeNode* typeNode)
	{
		std::string typeName;
		typeNode->getFullName(typeName);
		if ("::pafcore::Object" == typeName)
		{
			stat.objects.push_back(stat.offset++);
		}
		else if ("::pafcore::Interface" == typeName)
		{
			stat.interfaces.push_back(stat.offset++);
		}
		//else
		//{
		//	stat.otherClasses.push_back(stat.offset++);
		//}

		ClassNode* classNode = nullptr;
		TemplateArguments* templateArguments = nullptr;
		if (typeNode->isTemplateClassInstance())
		{
			TemplateClassInstanceTypeNode* templateClassInstanceTypeNode = static_cast<TemplateClassInstanceTypeNode*>(typeNode);
			classNode = templateClassInstanceTypeNode->m_classNode;
			templateArguments = &templateClassInstanceTypeNode->m_templateClassInstanceNode->m_templateArguments;
		}
		else if (typeNode->isClass() && !typeNode->isTemplateClass())
		{
			classNode = static_cast<ClassTypeNode*>(typeNode)->m_classNode;
		}
		else
		{
			return;
			//assert(false);
			//std::string typeName;
			//typeNode->getFullName(typeName);
			//stat.offset++;
			//stat.notClasses.push_back(typeName);
		}
		if (classNode->m_baseList)
		{
			std::vector<TypeNameNode*> baseTypeNameNodes;
			classNode->m_baseList->collectTypeNameNodes(baseTypeNameNodes);
			for (TypeNameNode* baseTypeNameNode : baseTypeNameNodes)
			{
				TypeNode* baseTypeNode = baseTypeNameNode->getActualTypeNode(templateArguments);
				StatBaseClass(stat, baseTypeNode);
			}
		}
	}

	void CheckBaseTypes(ClassNode* classNode, std::vector<TypeNameNode*>& baseTypeNameNodes, TemplateArguments* templateArguments)
	{
		int objectBaseCount = 0;
		int baseClassOffset = 0;
		int firstObjectBaseOffset = -1;

		std::vector<TypeNode*> baseTypeNodes;
		size_t count = baseTypeNameNodes.size();
		for (size_t i = 0; i < count; ++i)
		{
			TypeNameNode* typeNameNode = baseTypeNameNodes[i];
			TypeNode* typeNode = typeNameNode->getTypeNode(templateArguments);
			if (0 == typeNode)
			{
				continue;
			}
			TypeKind baseTypeKind = typeNode->getTypeKind(templateArguments);
			if (class_type != baseTypeKind)
			{
				char buf[4096];
				sprintf_s(buf, "\'%s\' : base type must be class type", typeNameNode->m_scopeNameList->m_scopeName->m_name->m_str.c_str());
				ErrorList_AddItem_CurrentFile(typeNameNode->m_scopeNameList->m_scopeName->m_name->m_lineNo,
					typeNameNode->m_scopeNameList->m_scopeName->m_name->m_columnNo, semantic_error_base_type, buf);
			}
			else
			{
				BaseStat stat;
				stat.offset = baseClassOffset;
				StatBaseClass(stat, typeNode);
				baseClassOffset = stat.offset;
				objectBaseCount += stat.objects.size();
				if (firstObjectBaseOffset == -1 && !stat.objects.empty())
				{
					firstObjectBaseOffset = stat.objects[0];
				}
				if (classNode->isInterface())
				{
					if (!stat.objects.empty())
					{
						char buf[4096];
						sprintf_s(buf, "\'%s\' : interface type must not derive from ::pafcore::Object", typeNameNode->m_scopeNameList->m_scopeName->m_name->m_str.c_str());
						ErrorList_AddItem_CurrentFile(typeNameNode->m_scopeNameList->m_scopeName->m_name->m_lineNo,
							typeNameNode->m_scopeNameList->m_scopeName->m_name->m_columnNo, semantic_error_base_type, buf);
					}
				}
			}
			auto it = std::find(baseTypeNodes.begin(), baseTypeNodes.begin() + i, typeNode);
			if (it != baseTypeNodes.begin() + i)
			{
				size_t index = it - baseTypeNodes.begin();
				char buf[4096];
				sprintf_s(buf, "\'%s\' : is already a direct base class, the previous declaration is at line %d, column %d",
					typeNameNode->m_scopeNameList->m_scopeName->m_name->m_str.c_str(),
					baseTypeNameNodes[index]->m_scopeNameList->m_scopeName->m_name->m_lineNo,
					baseTypeNameNodes[index]->m_scopeNameList->m_scopeName->m_name->m_columnNo);
				ErrorList_AddItem_CurrentFile(typeNameNode->m_scopeNameList->m_scopeName->m_name->m_lineNo,
					typeNameNode->m_scopeNameList->m_scopeName->m_name->m_columnNo, semantic_error_base_redeclared, buf);
			}
			baseTypeNodes.push_back(typeNode);
		}
		if (1 == objectBaseCount)
		{
			if (0 != firstObjectBaseOffset)
			{
				char buf[4096];
				sprintf_s(buf, "\'%s\' : First base class must derive from ::pafcore::Object", classNode->m_name->m_str.c_str());
				ErrorList_AddItem_CurrentFile(classNode->m_name->m_lineNo,
					classNode->m_name->m_columnNo, semantic_error_base_type, buf);
			}
		}
		else if (1 < objectBaseCount)
		{
			char buf[4096];
			sprintf_s(buf, "\'%s\' : class type must not derive from more than one ::pafcore::Object", classNode->m_name->m_str.c_str());
			ErrorList_AddItem_CurrentFile(classNode->m_name->m_lineNo,
				classNode->m_name->m_columnNo, semantic_error_base_type, buf);
		}

	}

	struct Overload
	{
		IdentifierNode* methodName;
		size_t parameterCount;
		std::string manglingName;
		bool operator < (const Overload& arg) const
		{
			int cmp = methodName->m_str.compare(arg.methodName->m_str);
			if (cmp != 0)
			{
				return cmp < 0;
			}
			if (parameterCount != arg.parameterCount)
			{
				return parameterCount < arg.parameterCount;
			}
			return manglingName < arg.manglingName;
		}
	};

	bool isDefaultConstructor(ClassNode* classNode, MethodNode* methodNode)
	{
		if (classNode->m_name->m_str == methodNode->m_name->m_str)
		{
			if (0 == methodNode->getParameterCount())
			{
				return true;
			}
		}
		return false;
	}

	void checkMemberNames(ClassNode* classNode, std::vector<MemberNode*>& memberNodes, TemplateArguments* templateArguments)
	{
		std::set<IdentifierNode*, CompareIdentifierPtr> methodNames;
		std::set<IdentifierNode*, CompareIdentifierPtr> staticMethodNames;
		std::set<IdentifierNode*, CompareIdentifierPtr> otherNames;
		std::set<Overload> methods;
		std::set<Overload> staticMethods;

		size_t count = memberNodes.size();
		IdentifierNode* collisionNode = 0;
		for (size_t i = 0; i < count; ++i)
		{
			MemberNode* memberNode = memberNodes[i];
			bool nameCollision = false;
			IdentifierNode* identifier = memberNode->m_name;
			if (0 == identifier)
			{
				//invalid operator has no name
				continue;
			}
			if (snt_method == memberNode->m_nodeType)
			{
				MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
				if (methodNode->m_name->m_str == classNode->m_name->m_str && !methodNode->isStatic())
				{
					if (nullptr != methodNode->m_resultList)
					{
						char buf[4096];
						sprintf_s(buf, "\'%s\' : constructor with return type", identifier->m_str.c_str());
						ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
							identifier->m_columnNo, semantic_error_constructor_with_return_type, buf);
						continue;
					}
					if (nullptr != methodNode->m_modifier)
					{
						char buf[4096];
						sprintf_s(buf, "\'%s\' : constructor cannot be declared %s", identifier->m_str.c_str(),
							KeywardTokenToString(methodNode->m_modifier));
						ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
							identifier->m_columnNo, semantic_error_constructor_with_modifier, buf);
						continue;
					}
				}
				auto it = otherNames.find(identifier);
				if (otherNames.end() != it)
				{
					collisionNode = *it;
					nameCollision = true;
				}
				else
				{
					if (methodNode->isStatic())
					{
						it = methodNames.find(identifier);
						if (methodNames.end() != it)
						{
							collisionNode = *it;
							nameCollision = true;
						}
					}
					else
					{
						it = staticMethodNames.find(identifier);
						if (staticMethodNames.end() != it)
						{
							collisionNode = *it;
							nameCollision = true;
						}
					}
					if (!nameCollision)
					{
						Overload overload;
						overload.methodName = identifier;
						overload.parameterCount = methodNode->getParameterCount();
						methodNode->calcManglingName(overload.manglingName, templateArguments);
						if (methodNode->isStatic())
						{
							auto res = staticMethods.insert(overload);
							if (!res.second)
							{
								collisionNode = res.first->methodName;
								nameCollision = true;
							}
							else
							{
								staticMethodNames.insert(identifier);
							}
						}
						else
						{
							++overload.parameterCount;
							auto res = methods.insert(overload);
							if (!res.second)
							{
								collisionNode = res.first->methodName;
								nameCollision = true;
							}
							else
							{
								methodNames.insert(identifier);
							}
						}
					}
				}
			}
			else
			{
				if (identifier->m_str == classNode->m_name->m_str)
				{
					char buf[4096];
					sprintf_s(buf, "\'%s\' : class member name cannot equal to class name", identifier->m_str.c_str());
					ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
						identifier->m_columnNo, semantic_error_member_name_equal_to_class_name, buf);
				}
				auto it = otherNames.find(identifier);
				if (otherNames.end() != it)
				{
					collisionNode = *it;
					nameCollision = true;
				}
				else
				{
					otherNames.insert(identifier);
				}
				if ((it = methodNames.find(identifier)) != methodNames.end())
				{
					collisionNode = *it;
					nameCollision = true;
				}
				else if ((it = staticMethodNames.find(identifier)) != staticMethodNames.end())
				{
					collisionNode = *it;
					nameCollision = true;
				}
			}
			if (nameCollision)
			{
				char buf[4096];
				sprintf_s(buf, "\'%s\' : member already defined at line %d, column %d", identifier->m_str.c_str(),
					collisionNode->m_lineNo, collisionNode->m_columnNo);
				ErrorList_AddItem_CurrentFile(identifier->m_lineNo,
					identifier->m_columnNo, semantic_error_member_redefined, buf);
			}
		}
	}

	static void ParseConceptList(
		IdentifierNode*& metadataKind,
		//ClassNode::LazyBool& copyableFlag,
		IdentifierListNode* conceptList)
	{
		const char* s_kinds[] =
		{
			"enum_member",
			"instance_field",
			"static_field",
			"instance_property",
			"static_property",
			"instance_method",
			"static_method",
			"function_argument",
			"function_result",
			"primitive_type",
			"enum_type",
			"class_type",
			"type_alias",
			"name_space",
			"dummy_metadata",
			"dummy_type",
		};

		std::vector<IdentifierNode*> identifierNodes;
		conceptList->collectIdentifierNodes(identifierNodes);
		for (IdentifierNode* identifierNode : identifierNodes)
		{
/*			if (identifierNode->m_str == "copyable")
			{
				copyableFlag = ClassNode::lb_true;
			}
			else if (identifierNode->m_str == "noncopyable")
			{
				copyableFlag = ClassNode::lb_false;
			}
			else */if (0 == metadataKind)
			{
				for (int i = 0; i < sizeof(s_kinds) / sizeof(s_kinds[0]); ++i)
				{
					if (identifierNode->m_str == s_kinds[i])
					{
						metadataKind = identifierNode;
						break;
					}
				}
			}
		}
	}
}

ClassNode::ClassNode(TokenNode* keyword, IdentifierListNode* conceptList, IdentifierNode* name)
{
	m_nodeType = snt_class;
	m_keyword = keyword;
	m_conceptList = conceptList;
	m_name = name;
	ParseConceptList(m_metadataKind, /*m_copyableFlag, */conceptList);
}

bool ClassNode::isInterface() const
{
	return m_keyword->m_nodeType == snt_keyword_interface;
}

bool ClassNode::isStruct() const
{
	return m_keyword->m_nodeType == snt_keyword_struct;
}

bool ClassNode::isClass() const
{
	return m_keyword->m_nodeType == snt_keyword_class;
}

bool ClassNode::isDerivedFromObject() const
{
	if (lb_unknown == m_isDerivedFromObject)
	{
		BaseStat stat;
		StatBaseClass(stat, m_typeNode);
		if (!stat.objects.empty())
		{
			m_isDerivedFromObject = lb_true;
		}
		else
		{
			m_isDerivedFromObject = lb_false;
		}
	}
	return (lb_true == m_isDerivedFromObject);
}

bool ClassNode::isDerivedFromInterface() const
{
	if (lb_unknown == m_isDerivedFromInterface)
	{
		BaseStat stat;
		StatBaseClass(stat, m_typeNode);
		if (!stat.interfaces.empty())
		{
			m_isDerivedFromInterface = lb_true;
		}
		else
		{
			m_isDerivedFromInterface = lb_false;
		}
	}
	return (lb_true == m_isDerivedFromInterface);
}

bool ClassNode::hasAdditionalMethods() const
{
	return !m_additionalMethods.empty();
}

void ClassNode::setTemplateParameters(TemplateParametersNode* templateParametersNode)
{
	assert(templateParametersNode);
	m_templateParametersNode = templateParametersNode;
}

void ClassNode::setMemberList(TokenNode* leftBrace, MemberListNode* memberList, TokenNode* rightBrace)
{
	m_leftBrace = leftBrace;
	m_memberList = memberList;
	m_rightBrace = rightBrace;
	m_memberList->initializeMembersEnclosing(this);
}

void ClassNode::extendInternalCode(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	if (m_templateParametersNode)
	{
		assert(0 == templateArguments);
		templateArguments = &m_templateArguments;
	}

	buildAdditionalMethods();

	//size_t count = m_additionalMethods.size();
	//for (size_t i = 0; i < count; ++i)
	//{
	//	MethodNode* methodNode = m_additionalMethods[i];
	//	assert(methodNode->m_resultTypeName);
	//	methodNode->m_resultTypeName->calcTypeNodes(m_typeNode, templateArguments);
	//}

	std::vector<MemberNode*> memberNodes;
	m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for (size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		switch (memberNode->m_nodeType)
		{
		case snt_class:
			static_cast<ClassNode*>(memberNode)->extendInternalCode(m_typeNode, templateArguments);
			break;
		}
	}

}

extern int yytokenno;
extern int yylineno;
extern int yycolumnno;

void ClassNode::generateConstruct(const char* methodName, MethodNode* constructor)
{
	//CompoundTypeNode* resultType = (CompoundTypeNode*)newCompoundType(newTypeName(newScopeNameList(0,newScopeName(m_name, 0, 0, 0))), tc_shared_ptr);
	//IdentifierNode* resultName = (IdentifierNode*)newIdentifier("result");
	//VariableNode* resultVariable = (VariableNode*)newVariable(resultType, nullptr, resultName, nullptr);
	//VariableListNode* resultList = (VariableListNode*)newVariableList(nullptr, nullptr, resultVariable);

	CompoundTypeNode* addressType = (CompoundTypeNode*)newCompoundType(newTypeName(newScopeNameList(0,newScopeName(newIdentifier("size_t"), 0, 0, 0))), tc_none);
	IdentifierNode* addressName = (IdentifierNode*)newIdentifier("address");
	VariableNode* addressVariable = (VariableNode*)newVariable(addressType, nullptr, addressName, nullptr);

	VariableListNode* paramList = MergeVariableList(addressVariable, constructor->m_parameterList);
	IdentifierNode* name = (IdentifierNode*)newIdentifier(methodName);

	MethodNode* method = (MethodNode*)newMethod(nullptr, name,
		constructor->m_leftParenthesis, paramList,
		constructor->m_rightParenthesis);
	TokenNode* modifier = (TokenNode*)newToken(snt_keyword_static);
	setMethodModifier(method, modifier);
	setMemberSemicolon(method, constructor->m_semicolon);
	method->m_enclosing = this;
	m_additionalMethods.push_back(method);
}

void ClassNode::generateConstructArray(const char* methodName, MethodNode* constructor)
{
	//CompoundTypeNode* resultType = (CompoundTypeNode*)newCompoundType(newTypeName(newScopeNameList(0, newScopeName(m_name, 0, 0, 0))), tc_shared_array);
	//IdentifierNode* resultName = (IdentifierNode*)newIdentifier("result");
	//VariableNode* resultVariable = (VariableNode*)newVariable(resultType, nullptr, resultName, nullptr);
	//VariableListNode* resultList = (VariableListNode*)newVariableList(nullptr, nullptr, resultVariable);
	CompoundTypeNode* addressType = (CompoundTypeNode*)newCompoundType(newTypeName(newScopeNameList(0, newScopeName(newIdentifier("size_t"), 0, 0, 0))), tc_none);
	IdentifierNode* addressName = (IdentifierNode*)newIdentifier("address");
	VariableNode* addressVariable = (VariableNode*)newVariable(addressType, nullptr, addressName, nullptr);

	//CompoundTypeNode* paramType = (CompoundTypeNode*)newCompoundType(newPrimitiveType(newToken(snt_keyword_unsigned), pt_uint), tc_none);
	CompoundTypeNode* countType = (CompoundTypeNode*)newCompoundType(newTypeName(newScopeNameList(0, newScopeName(newIdentifier("size_t"), 0, 0, 0))), tc_none);
	IdentifierNode* countName = (IdentifierNode*)newIdentifier("count");
	VariableNode* countVariable = (VariableNode*)newVariable(countType, nullptr, countName, nullptr);

	//VariableListNode* paramList = (VariableListNode*)newVariableList(nullptr, nullptr, paramVariable);
	VariableListNode* paramList = MergeVariableList(addressVariable, countVariable);
	IdentifierNode* name = (IdentifierNode*)newIdentifier(methodName);
	MethodNode* method = (MethodNode*)newMethod(nullptr, name,
		constructor->m_leftParenthesis, paramList,
		constructor->m_rightParenthesis);

	TokenNode* modifier = (TokenNode*)newToken(snt_keyword_static);
	setMethodModifier(method, modifier);
	setMemberSemicolon(method, constructor->m_semicolon);
	method->m_enclosing = this;
	m_additionalMethods.push_back(method);
}


void ClassNode::buildAdditionalMethods()
{
	int backupToken = yytokenno;
	int backupLine = yylineno;
	int backupColumn = yycolumnno;
	yytokenno = 0;
	yylineno = 0;
	yycolumnno = 0;

	MethodNode* defaultConstructor = 0;
	std::vector<MethodNode*> constructors;	
	std::vector<MemberNode*> memberNodes;
	m_memberList->collectMemberNodes(memberNodes);
	size_t memberCount = memberNodes.size();
	for(size_t i = 0; i < memberCount; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		if(snt_method == memberNode->m_nodeType)
		{
			MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
			if(memberNode->m_name->m_str == m_name->m_str)
			{
				if(isDefaultConstructor(this, methodNode))
				{
					assert(0 == defaultConstructor);
					defaultConstructor = methodNode;
				}
				constructors.push_back(methodNode);
			}
		}
	}

	size_t count = constructors.size();

	for (size_t i = 0; i < count; ++i)
	{
		MethodNode* constructor = constructors[i];
		generateConstruct("Construct", constructor);
	}
	if (0 != defaultConstructor)
	{
		generateConstructArray("ConstructArray", defaultConstructor);
	}

	yytokenno = backupToken;
	yylineno = backupLine;
	yycolumnno = backupColumn;
}

//bool ClassNode::isAbstractClass()
//{
//	if(lb_unknown == m_abstractFlag)
//	{
//		if(m_modifier)
//		{
//			m_abstractFlag = lb_true;
//			return true;
//		}
//		std::vector<MemberNode*> memberNodes;
//		m_memberList->collectMemberNodes(memberNodes);
//		size_t count = memberNodes.size();
//		for(size_t i = 0; i < count; ++i)
//		{
//			MemberNode* memberNode = memberNodes[i];
//			if(snt_method == memberNode->m_nodeType)
//			{
//				MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
//				if(methodNode->isAbstract())
//				{
//					m_abstractFlag = lb_true;
//					return true;
//				}
//			}
//		}
//		m_abstractFlag = lb_false;
//	}
//	return (lb_true == m_abstractFlag);
//}
//
//bool ClassNode::isCopyableClass(TemplateArguments* templateArguments)
//{
//	if (lb_unknown == m_copyableFlag)
//	{
//		bool baseClassCopyable = true;
//		std::vector<TypeNameNode*> baseTypeNameNodes;
//		m_baseList->collectTypeNameNodes(baseTypeNameNodes);
//		size_t count = baseTypeNameNodes.size();
//		for (size_t i = 0; i < count; ++i)
//		{
//			TypeNameNode* typeNameNode = baseTypeNameNodes[i];
//			TypeNode* typeNode = typeNameNode->getActualTypeNode(templateArguments);
//			if (typeNode->isTemplateClassInstance())
//			{
//				TemplateClassInstanceTypeNode* templateClassInstanceTypeNode = static_cast<TemplateClassInstanceTypeNode*>(typeNode);
//				if (!templateClassInstanceTypeNode->m_classNode->isCopyableClass(&templateClassInstanceTypeNode->m_templateClassInstanceNode->m_templateArguments))
//				{
//					baseClassCopyable = false;
//					break;
//				}
//			}
//			else
//			{
//				if (typeNode->isClass() && !typeNode->isTemplateClass())
//				{
//					ClassTypeNode* classTypeNode = static_cast<ClassTypeNode*>(typeNode);
//					if (classTypeNode->m_classNode
//						&& !classTypeNode->m_classNode->isCopyableClass(0))
//					{
//						baseClassCopyable = false;
//						break;
//					}
//				}
//			}
//		}
//		m_copyableFlag = baseClassCopyable ? lb_true : lb_false;
//	}
//	return (lb_true == m_copyableFlag);
//}

void ClassNode::collectOverrideMethods(std::vector<MethodNode*>& methodNodes, TemplateArguments* templateArguments)
{
	std::vector<MemberNode*> memberNodes;
	m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		if(snt_method == memberNode->m_nodeType)
		{
			MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
			if(methodNode->isVirtual())
			{
				methodNodes.push_back(methodNode);
			}
		}
	}
	std::vector<TypeNameNode*> baseTypeNameNodes;
	m_baseList->collectTypeNameNodes(baseTypeNameNodes);
	count = baseTypeNameNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = baseTypeNameNodes[i];
		TypeNode* typeNode = typeNameNode->getActualTypeNode(templateArguments);
		if (typeNode->isTemplateClassInstance())
		{
			TemplateClassInstanceTypeNode* templateClassInstanceTypeNode = static_cast<TemplateClassInstanceTypeNode*>(typeNode);
			templateClassInstanceTypeNode->m_classNode->collectOverrideMethods(methodNodes,
				&templateClassInstanceTypeNode->m_templateClassInstanceNode->m_templateArguments);
		}
		else
		{
			if (typeNode->isClass() && !typeNode->isTemplateClass())
			{
				ClassTypeNode* classTypeNode = static_cast<ClassTypeNode*>(typeNode);
				if (classTypeNode->m_classNode)
				{
					classTypeNode->m_classNode->collectOverrideMethods(methodNodes, 0);
				}
			}
		}
	}
}

bool ClassNode::needInterfaceProxy(TemplateArguments* templateArguments)
{
	if (isInterface())
	{
		if (hasOverrideMethod(templateArguments))
		{
			return true;
		}
	}
	return false;
}

bool ClassNode::hasOverrideMethod(TemplateArguments* templateArguments)
{
	std::vector<MemberNode*> memberNodes;
	m_memberList->collectMemberNodes(memberNodes);
	size_t count = memberNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		if(snt_method == memberNode->m_nodeType)
		{
			MethodNode* methodNode = static_cast<MethodNode*>(memberNode);
			if(methodNode->isVirtual())
			{
				return true;
			}
		}
	}
	std::vector<TypeNameNode*> baseTypeNameNodes;
	m_baseList->collectTypeNameNodes(baseTypeNameNodes);
	count = baseTypeNameNodes.size();
	for(size_t i = 0; i < count; ++i)
	{
		TypeNameNode* typeNameNode = baseTypeNameNodes[i];
		TypeNode* typeNode = typeNameNode->getActualTypeNode(templateArguments);
		if (typeNode->isTemplateClassInstance())
		{
			TemplateClassInstanceTypeNode* templateClassInstanceTypeNode = static_cast<TemplateClassInstanceTypeNode*>(typeNode);
			if (templateClassInstanceTypeNode->m_classNode->hasOverrideMethod(&templateClassInstanceTypeNode->m_templateClassInstanceNode->m_templateArguments))
			{
				return true;
			}
		}
		else
		{
			if (typeNode->isClass() && !typeNode->isTemplateClass())
			{
				ClassTypeNode* classTypeNode = static_cast<ClassTypeNode*>(typeNode);
				if (classTypeNode->m_classNode
					&& classTypeNode->m_classNode->hasOverrideMethod(0))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ClassNode::isAdditionalMethod(MethodNode* methodNode)
{
	auto it = m_additionalMethods.begin();
	auto end = m_additionalMethods.end();
	for(; it != end; ++it)
	{
		if (methodNode == *it)
		{
			return true;
		}
	}
	return false;
}

TypeNode* ClassNode::getTypeNode()
{
	return m_typeNode;
}

void ClassNode::getLocalName(std::string& name, TemplateArguments* templateArguments)
{
	if (m_templateParametersNode)
	{
		assert(templateArguments 
			&& templateArguments->m_classTypeNode->isTemplateClassInstance()
			&& templateArguments->m_className == m_name->m_str
			&& templateArguments->m_arguments.size() == m_templateArguments.m_arguments.size());
		name = static_cast<TemplateClassInstanceTypeNode*>(templateArguments->m_classTypeNode)->m_localName;
	}
	else
	{
		name = m_name->m_str;
	}
}

void ClassNode::collectTypes(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	assert(enclosingTypeNode);
	assert(0 == m_typeNode);
	if (0 != m_templateParametersNode)
	{		
		if (!m_templateParametersNode->checkSemantic())
		{
			return;
		}
	}
	switch (enclosingTypeNode->m_kind)
	{
	case tc_namespace:
		m_typeNode = static_cast<NamespaceTypeNode*>(enclosingTypeNode)->addClass(this);
		break;
	case tc_class_type:
		if (0 == m_templateParametersNode)
		{
			m_typeNode = static_cast<ClassTypeNode*>(enclosingTypeNode)->addClass(this);
		}
		else
		{
			RaiseError_NestedTemplateClass(m_name);
		}
		break;
	default:
		assert(false);
	}
	if(m_typeNode)
	{
		if (m_templateParametersNode)
		{
			m_templateArguments.m_className = m_name->m_str;
			m_templateArguments.m_classTypeNode = m_typeNode;
			auto it = m_typeNode->m_parameterNodes.begin();
			auto end = m_typeNode->m_parameterNodes.end();
			for (; it != end; ++it)
			{
				TemplateParameterTypeNode* typeNode = *it;
				TemplateArgument arg;
				arg.m_name = typeNode->m_name;
				arg.m_typeNode = typeNode;
				m_templateArguments.m_arguments.push_back(arg);
			}
			assert(0 == templateArguments);
			templateArguments = &m_templateArguments;
		}
		if (m_memberList)
		{
			m_memberList->collectTypes(m_typeNode, templateArguments);
		}
	}
}

void ClassNode::checkTypeNames(TypeNode* enclosingTypeNode, TemplateArguments* templateArguments)
{
	if (m_templateParametersNode)
	{
		assert(0 == templateArguments);
		templateArguments = &m_templateArguments;
	}
	std::vector<TypeNameNode*> baseTypeNameNodes;
	m_baseList->collectTypeNameNodes(baseTypeNameNodes);
	std::vector<TypeNode*> baseTypeNodes;
	size_t baseCount = baseTypeNameNodes.size();
	for (size_t i = 0; i < baseCount; ++i)
	{
		TypeNameNode* typeNameNode = baseTypeNameNodes[i];
		typeNameNode->calcTypeNodes(enclosingTypeNode, templateArguments);
	}
	m_memberList->checkTypeNames(m_typeNode, templateArguments);


	size_t count = m_additionalMethods.size();
	for (size_t i = 0; i < count; ++i)
	{
		MethodNode* methodNode = m_additionalMethods[i];
		methodNode->checkTypeNames(m_typeNode, templateArguments);
	}
}

void ClassNode::checkSemantic(TemplateArguments* templateArguments)
{
	MemberNode::checkSemantic(templateArguments);

	if (m_templateParametersNode)
	{
		if (0 == templateArguments)
		{
			return;
		}
	}
	assert(m_typeNode && m_typeNode->m_enclosing);
	std::vector<TypeNameNode*> baseTypeNameNodes;
	m_baseList->collectTypeNameNodes(baseTypeNameNodes);
	std::vector<TypeNode*> baseTypeNodes;
	size_t baseCount = baseTypeNameNodes.size();
	for (size_t i = 0; i < baseCount; ++i)
	{
		TypeNameNode* typeNameNode = baseTypeNameNodes[i];
		TypeNode* typeNode = typeNameNode->getTypeNode(templateArguments);
		if (0 != typeNode)
		{
			baseTypeNodes.push_back(typeNode);
			g_compiler.useType(typeNode, templateArguments, tu_use_definition, typeNameNode);
		}
	}


	CheckBaseTypes(this, baseTypeNameNodes, templateArguments);
	if (0 == baseCount)
	{
		if (isInterface())
		{
			std::string typeName;
			m_typeNode->getFullName(typeName);
			if (typeName != "::pafcore::Interface")
			{
				RaiseError_InvalidBaseType(m_name, "interface", "::pafcore::Interface");
			}
		}
	}

	std::vector<MemberNode*> memberNodes;
	m_memberList->collectMemberNodes(memberNodes);

	size_t memberCount = memberNodes.size();
	for (size_t i = 0; i < memberCount; ++i)
	{
		MemberNode* memberNode = memberNodes[i];
		memberNodes[i]->checkSemantic(templateArguments);
	}
	checkMemberNames(this, memberNodes, templateArguments);
}
