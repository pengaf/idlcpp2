#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

struct SyntaxNode
{
	int m_nodeType;
};
typedef struct SyntaxNode SyntaxNode;

enum PredefinedType
{
	pt_bool,
	pt_char,
	pt_schar,
	pt_uchar,
	pt_wchar_t,
	pt_short,
	pt_ushort,
	pt_long,
	pt_ulong,
	pt_longlong,
	pt_ulonglong,
	pt_int,
	pt_uint,
	pt_float,
	pt_double,
	pt_long_double,
	pt_string_t,
	pt_end,
};

typedef enum PredefinedType PredefinedType;

enum TypeKind
{
	unknown_type,
	//type_not_found,
	primitive_type,
	enum_type,
	class_type,
	//class_template,
	//template_parameter,
};
typedef enum TypeKind TypeKind;


enum TypeCompound
{
	tc_none,
	tc_raw_ptr,
	tc_observer_ptr,
	tc_shared_ptr,
	tc_observer_array,
	tc_shared_array,
};

typedef enum TypeCompound TypeCompound;

enum SyntaxNodeType
{
	snt_keyword_nometa = 258,
	snt_keyword_nocode,
	snt_keyword_void,
	snt_keyword_scope, snt_begin_output = snt_keyword_scope,
	snt_keyword_bool, snt_keyword_begin_primitive = snt_keyword_bool,
	snt_keyword_char,
	snt_keyword_wchar_t,
	snt_keyword_short,
	snt_keyword_long,
	snt_keyword_int,
	snt_keyword_signed,
	snt_keyword_unsigned,
	snt_keyword_float,
	snt_keyword_double,
	snt_keyword_string_t, snt_keyword_end_primitive = snt_keyword_string_t,
	snt_keyword_namespace,
	snt_keyword_enum,
	snt_keyword_class,
	snt_keyword_struct,
	snt_keyword_interface,
	snt_keyword_template,
	snt_keyword_virtual,
	snt_keyword_static,
	snt_keyword_const,
	snt_keyword_get,
	snt_keyword_set,
	snt_keyword_typedef,
	snt_keyword_typename,
	snt_keyword_export, snt_end_output = snt_keyword_export,

	snt_identifier,
	snt_identifier_list,
	snt_enumerator,
	snt_enumerator_list,
	snt_enum,
	snt_scope_name,
	snt_scope_name_list,
	snt_type_name,
	snt_type_name_list,
	snt_compound_type,
	snt_property_accessor,
	snt_property_accessor_list,
	snt_variable,
	snt_variable_list,

	snt_template_parameters,
	snt_field,
	snt_property,
	snt_method,
	snt_class,
	snt_template_class_instance,
	snt_typedef,
	snt_type_declaration,
	snt_namespace,
	snt_member_list,
	snt_token_list,
	snt_attribute,
	snt_attribute_list,
};

enum PropertyKind
{
	simple_property,
	fixed_array_property,
	dynamic_array_property,
	list_property,
};

typedef enum PropertyKind PropertyKind;

void newCodeBlock(const char* str);
SyntaxNode* newToken(int nodeType);
SyntaxNode* newIdentifier(const char* str);
SyntaxNode* newString(const char* str);

SyntaxNode* newPrimitiveType(SyntaxNode* keyword, PredefinedType type);

SyntaxNode* newIdentifierList(SyntaxNode* identifierList, SyntaxNode* comma, SyntaxNode* identifier);

SyntaxNode* newAttribute(SyntaxNode* name, SyntaxNode* content, int u8content);
SyntaxNode* newAttributeList(SyntaxNode* attributeList, SyntaxNode* attribute);

void setEntityAttributeListOpt(SyntaxNode* entity, SyntaxNode* attributeList);

SyntaxNode* newScopeName(SyntaxNode* identifier, SyntaxNode* lts, SyntaxNode* parameterList, SyntaxNode* gts);
SyntaxNode* newScopeNameList(SyntaxNode* scopeNameList, SyntaxNode* scopeName);
void setScopeNameListGlobal(SyntaxNode* scopeNameList);
SyntaxNode* newTypeName(SyntaxNode* scopeNameList);
SyntaxNode* newTypeNameList(SyntaxNode* typeNameList, SyntaxNode* comma, SyntaxNode* typeName);
void setTypeNameNoCode(SyntaxNode* syntaxNode);
void setTypeNameNoMeta(SyntaxNode* syntaxNode);

SyntaxNode* newCompoundType(SyntaxNode* typeName, TypeCompound typeCompound);
SyntaxNode* newTypedef(SyntaxNode* keyword, SyntaxNode* typeName, SyntaxNode* name);
SyntaxNode* newTypeDeclaration(SyntaxNode* name, TypeKind typeKind);

SyntaxNode* newEnumerator(SyntaxNode* name, SyntaxNode* equalSign);
SyntaxNode* newEnumeratorList(SyntaxNode* enumeratorList, SyntaxNode* comma, SyntaxNode* enumerator);
SyntaxNode* newEnum(SyntaxNode* keyword, SyntaxNode* keyword2, SyntaxNode* name, SyntaxNode* leftBrace, SyntaxNode* enumeratorList, SyntaxNode* rightBrace);

SyntaxNode* newField(SyntaxNode* compoundType, SyntaxNode* name, SyntaxNode* leftBracket, SyntaxNode* rightBracket);
void setFieldModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier);

SyntaxNode* newPropertyAccessor(SyntaxNode* keyword, SyntaxNode* byRef, SyntaxNode* nativeName);
SyntaxNode* newPropertyAccessorList(SyntaxNode* propertyAccessorList, SyntaxNode* propertyAccessor);
SyntaxNode* newProperty(SyntaxNode* compoundType, SyntaxNode* name, SyntaxNode* accessorList, PropertyKind propertyKind);
void setPropertyModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier);

SyntaxNode* newVariable(SyntaxNode* compoundType, SyntaxNode* byRef, SyntaxNode* name, SyntaxNode* allowNull);
SyntaxNode* newVariableList(SyntaxNode* variableList, SyntaxNode* comma, SyntaxNode* variable);
SyntaxNode* newMethod(SyntaxNode* resultList, SyntaxNode* name, SyntaxNode* leftParenthesis, SyntaxNode* parameterList, SyntaxNode* rightParenthesis);
SyntaxNode* newMethod2(SyntaxNode* compoundType, SyntaxNode* byRefOpt, SyntaxNode* name, SyntaxNode* leftParenthesis, SyntaxNode* parameterList, SyntaxNode* rightParenthesis);
void setMethodModifier(SyntaxNode* syntaxNode, SyntaxNode* modifier);
void setMethodConst(SyntaxNode* syntaxNode, SyntaxNode* constant);

void setMemberFilter(SyntaxNode* syntaxNode, SyntaxNode* filter);
void setMemberNativeNameOpt(SyntaxNode* member, SyntaxNode* nativeName);
void setMemberSemicolon(SyntaxNode* member, SyntaxNode* semicolon);
SyntaxNode* newMemberList(SyntaxNode* memberList, SyntaxNode* member);
SyntaxNode* newTemplateParameters(SyntaxNode* keyword, SyntaxNode* lts, SyntaxNode* parameterList, SyntaxNode* gts);

SyntaxNode* newClass(SyntaxNode* keyword, SyntaxNode* conceptList, SyntaxNode* name);

void setClassBaseList(SyntaxNode* cls, SyntaxNode* colon, SyntaxNode* baseList);
void setClassMemberList(SyntaxNode* cls, SyntaxNode* leftBrace, SyntaxNode* memberList, SyntaxNode* rightBrace);
void setClassTemplateParameters(SyntaxNode* cls, SyntaxNode* parameters);

SyntaxNode* newTemplateClassInstance(SyntaxNode* name, SyntaxNode* typeNameList);
void setTemplateClassInstanceReservedMemberList(SyntaxNode* tci, SyntaxNode* identifierList);

SyntaxNode* newNamespace(SyntaxNode* keyword, SyntaxNode* name, SyntaxNode* leftBrace, SyntaxNode* memberList, SyntaxNode* rightBrace);
SyntaxNode* newProgram(SyntaxNode* memberList);

void yyerror(const char* s);
void invalidString(char* s);
void unterminatedCode();
void unterminatedComment();
void addSourceFile(const char* fileName);
void attachSyntaxTree(SyntaxNode* tree);

#ifdef __cplusplus
}
#endif
