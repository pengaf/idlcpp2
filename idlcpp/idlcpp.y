%{
#include "yyfuncs.h"
int yylex(void);
%}

%union
{
	struct SyntaxNode* sn;
}

%token <sn> ',' ':' ';' '(' ')' '[' ']' '{' '}' '<' '>' '*' '&' '^' '=' '?'
%token <sn> NOMETA NOCODE VOID SCOPE BOOL CHAR WCHAR_T SHORT LONG INT FLOAT DOUBLE SIGNED UNSIGNED STRING_T
%token <sn> NAMESPACE ENUM CLASS STRUCT INTERFACE VIRTUAL STATIC CONST TYPEDEF TYPENAME TEMPLATE EXPORT
%token <sn> GET SET IDENTIFIER STRING U8STRING
%type <sn> identifier_list enumerator_0 enumerator enumerator_list_0 enumerator_list enum compound_type variable variable_list
%type <sn> property_accessor_0 property_accessor property_accessor_list_0 property_accessor_list
%type <sn> field_0 field property_0 property method_0 method_1 method class_member_0 class_member_1 class_member
%type <sn> primitive scope_name scope_name_list_0 scope_name_list type_name base_type_name base_type_name_list type_name_list class_member_list
%type <sn> template_params template_class_instance_0 template_class_instance
%type <sn> class_0 class_1 class_2 class namespace_member_0 namespace_member_1 namespace_member_2 namespace_member namespace_member_list namespace program
%type <sn> type_declaration attribute attribute_list_0 attribute_list attribute_list_opt native_name_opt typedef by_ref_opt allow_null_opt class_struct_interface

%start program

%%

primitive					: BOOL															{$$ = newPrimitiveType($1, pt_bool);}
							| CHAR															{$$ = newPrimitiveType($1, pt_char);}
							| SIGNED CHAR													{$$ = newPrimitiveType($1, pt_schar);}
							| UNSIGNED CHAR													{$$ = newPrimitiveType($1, pt_uchar);}
							| WCHAR_T														{$$ = newPrimitiveType($1, pt_wchar_t);}
							| SHORT															{$$ = newPrimitiveType($1, pt_short);}
							| SHORT INT														{$$ = newPrimitiveType($1, pt_short);}
							| SIGNED SHORT													{$$ = newPrimitiveType($1, pt_short);}
							| SIGNED SHORT INT												{$$ = newPrimitiveType($1, pt_short);}
							| UNSIGNED SHORT												{$$ = newPrimitiveType($1, pt_ushort);}
							| UNSIGNED SHORT INT											{$$ = newPrimitiveType($1, pt_ushort);}
							| LONG															{$$ = newPrimitiveType($1, pt_long);}
							| LONG INT														{$$ = newPrimitiveType($1, pt_long);}
							| SIGNED LONG													{$$ = newPrimitiveType($1, pt_long);}
							| SIGNED LONG INT												{$$ = newPrimitiveType($1, pt_long);}
							| UNSIGNED LONG													{$$ = newPrimitiveType($1, pt_ulong);}
							| UNSIGNED LONG INT												{$$ = newPrimitiveType($1, pt_ulong);}
							| LONG LONG														{$$ = newPrimitiveType($1, pt_longlong);}
							| LONG LONG INT													{$$ = newPrimitiveType($1, pt_longlong);}
							| SIGNED LONG LONG												{$$ = newPrimitiveType($1, pt_longlong);}
							| SIGNED LONG LONG INT											{$$ = newPrimitiveType($1, pt_longlong);}
							| UNSIGNED LONG LONG											{$$ = newPrimitiveType($1, pt_ulonglong);}
							| UNSIGNED LONG LONG INT										{$$ = newPrimitiveType($1, pt_ulonglong);}
							| INT															{$$ = newPrimitiveType($1, pt_int);}
							| SIGNED INT													{$$ = newPrimitiveType($1, pt_int);}
							| SIGNED														{$$ = newPrimitiveType($1, pt_int);}
							| UNSIGNED INT													{$$ = newPrimitiveType($1, pt_uint);}
							| UNSIGNED														{$$ = newPrimitiveType($1, pt_uint);}
							| FLOAT															{$$ = newPrimitiveType($1, pt_float);}
							| DOUBLE														{$$ = newPrimitiveType($1, pt_double);}
							| LONG DOUBLE													{$$ = newPrimitiveType($1, pt_long_double);}
							| STRING_T														{$$ = newPrimitiveType($1, pt_string_t);}
;

class_struct_interface		: CLASS															{$$ = $1;}
							| STRUCT														{$$ = $1;}
							| INTERFACE														{$$ = $1;}
;

attribute					: IDENTIFIER													{$$ = newAttribute($1, NULL, 0);}
							| IDENTIFIER '=' STRING											{$$ = newAttribute($1, $3, 0);}
							| IDENTIFIER '=' U8STRING										{$$ = newAttribute($1, $3, 1);}
;

attribute_list_0			: attribute														{$$ = newAttributeList(NULL, $1);}
							| attribute_list_0 ',' attribute								{$$ = newAttributeList($1, $3);}
;

attribute_list				: '[' attribute_list_0 ']'										{$$ = $2;}
							| '[' attribute_list_0 ',' ']'									{$$ = $2;}
							| '[' ']'														{$$ = NULL;}
;

attribute_list_opt			: attribute_list												{$$ = $1;}
							| /*empty*/														{$$ = NULL;}
;

native_name_opt				: '=' STRING													{$$ = $2;}
							| /*empty*/														{$$ = NULL;}
;

by_ref_opt					: '&'															{$$ = $1;}
							| /*empty*/														{$$ = NULL;}
;

allow_null_opt				: '?'															{$$ = $1;}
							| /*empty*/														{$$ = NULL;}
;

identifier_list				: IDENTIFIER													{$$ = newIdentifierList(NULL, NULL, $1);}
							| identifier_list ',' IDENTIFIER								{$$ = newIdentifierList($1, $2, $3);}
;

scope_name					: IDENTIFIER													{$$ = newScopeName($1, NULL, NULL, NULL);}
							| IDENTIFIER '<' type_name_list '>'								{$$ = newScopeName($1, $2, $3, $4);}	
;

scope_name_list_0			: scope_name													{$$ = newScopeNameList(NULL, $1);}
							| scope_name_list_0 SCOPE scope_name							{$$ = newScopeNameList($1, $3);}
;

scope_name_list				: scope_name_list_0												{$$ = $1;}
							| SCOPE scope_name_list_0										{$$ = $2; setScopeNameListGlobal($$);}
;

type_name					: primitive														{$$ = $1;}
							| scope_name_list												{$$ = newTypeName($1);}
;

type_name_list				: type_name														{$$ = newTypeNameList(NULL, NULL, $1);}
							| type_name_list ',' type_name									{$$ = newTypeNameList($1, $2, $3);}
;

base_type_name				: type_name														{$$ = $1;}
							| NOCODE base_type_name											{$$ = $2; setTypeNameNoCode($$);}
							| NOMETA base_type_name											{$$ = $2; setTypeNameNoMeta($$);}
;

base_type_name_list			: base_type_name												{$$ = newTypeNameList(NULL, NULL, $1);}
							| base_type_name_list ',' base_type_name						{$$ = newTypeNameList($1, $2, $3);}
;

compound_type				: type_name														{$$ = newCompoundType($1, tc_none);}
							| type_name '*'													{$$ = newCompoundType($1, tc_raw_ptr);}
							| type_name '^'													{$$ = newCompoundType($1, tc_shared_ptr);}
							| type_name ':'													{$$ = newCompoundType($1, tc_observer_ptr);}
							| type_name '[' ']' '^'											{$$ = newCompoundType($1, tc_shared_array);}
							| type_name '[' ']' ':'											{$$ = newCompoundType($1, tc_observer_array);}
;

typedef						: TYPEDEF type_name IDENTIFIER									{$$ = newTypedef($1, $2, $3);}
;

type_declaration			: TYPENAME IDENTIFIER											{$$ = newTypeDeclaration($2, primitive_type);}
							| ENUM IDENTIFIER												{$$ = newTypeDeclaration($2, enum_type);}
							| class_struct_interface IDENTIFIER								{$$ = newTypeDeclaration($2, class_type);}
//							| CLASS IDENTIFIER												{$$ = newTypeDeclaration($2, rc_object_type);}
;

enumerator_0				: IDENTIFIER													{$$ = newEnumerator($1, NULL);}
							| IDENTIFIER '='												{$$ = newEnumerator($1, $2);}
;

enumerator					: attribute_list_opt enumerator_0								{$$ = $2; setEntityAttributeListOpt($$, $1);}
;

enumerator_list_0			: enumerator													{$$ = newEnumeratorList(NULL, NULL, $1);}
							| enumerator_list_0 ',' enumerator								{$$ = newEnumeratorList($1, $2, $3);}
;

enumerator_list				: enumerator_list_0												{$$ = $1;}
							| enumerator_list_0 ','											{$$ = $1;}
							| /*empty*/														{$$ = NULL;}
;

enum						: ENUM IDENTIFIER '{' enumerator_list '}'						{$$ = newEnum($1, NULL, $2, $3, $4, $5);}
							| ENUM CLASS IDENTIFIER '{' enumerator_list '}'					{$$ = newEnum($1, $2, $3, $4, $5, $6);}
;

field_0						: compound_type IDENTIFIER										{$$ = newField($1, $2, NULL, NULL);}
							| compound_type IDENTIFIER '[' ']'								{$$ = newField($1, $2, $3, $4);}
;

field						: field_0														{$$ = $1;}
							| STATIC field_0												{$$ = $2; setFieldModifier($$, $1);}
;

property_accessor_0			: GET															{$$ = $1;}
							| SET															{$$ = $1;}
							| ENUM															{$$ = $1;}
;

property_accessor			: property_accessor_0 by_ref_opt native_name_opt				{$$ = newPropertyAccessor($1, $2, $3);}
;

property_accessor_list_0	: property_accessor												{$$ = newPropertyAccessorList(NULL, $1);}
							| property_accessor_list_0 property_accessor					{$$ = newPropertyAccessorList($1, $2);}
;

property_accessor_list		: '{' property_accessor_list_0 '}'								{$$ = $2;}
;

property_0					: compound_type IDENTIFIER property_accessor_list				{$$ = newProperty($1, $2, $3, simple_property);}
							| compound_type IDENTIFIER '[' ']' property_accessor_list		{$$ = newProperty($1, $2, $5, fixed_array_property);}
							| compound_type IDENTIFIER '[' '?' ']' property_accessor_list	{$$ = newProperty($1, $2, $6, dynamic_array_property);}
							| compound_type IDENTIFIER '[' '*' ']' property_accessor_list	{$$ = newProperty($1, $2, $6, list_property);}
;

property					: property_0													{$$ = $1;}
							| STATIC property_0												{$$ = $2; setPropertyModifier($$, $1); }
;

variable					: compound_type by_ref_opt IDENTIFIER allow_null_opt			{$$ = newVariable($1, $2, $3, $4);}
;

variable_list 				: variable														{$$ = newVariableList(NULL, NULL, $1);}
							| variable_list ',' variable									{$$ = newVariableList($1, $2, $3);}
							| /*empty*/														{$$ = NULL;}
;

method_0					: '(' variable_list ')' IDENTIFIER '(' variable_list ')'		{$$ = newMethod($2, $4, $5, $6, $7);}
							| IDENTIFIER '(' variable_list ')'								{$$ = newMethod(NULL, $1, $2, $3, $4);}
							| VOID IDENTIFIER '(' variable_list ')'							{$$ = newMethod(NULL, $2, $3, $4, $5);}
							| compound_type IDENTIFIER '(' variable_list ')'				{$$ = newMethod2($1, NULL, $2, $3, $4, $5);}
							| compound_type '&' IDENTIFIER '(' variable_list ')'			{$$ = newMethod2($1, $2, $3, $4, $5, $6);}
;

method_1					: method_0														{$$ = $1;}
							| VIRTUAL method_0												{$$ = $2; setMethodModifier($$, $1);}
							| STATIC method_0												{$$ = $2; setMethodModifier($$, $1);}
;

method						: method_1														{$$ = $1;}
							| method_1 CONST												{$$ = $1; setMethodConst($$, $2);}
;

class_member_0				: field															{$$ = $1;}
							| property														{$$ = $1;}
							| method														{$$ = $1;}
							| class															{$$ = $1;}
							| enum															{$$ = $1;}
							| typedef														{$$ = $1;}
							| type_declaration												{$$ = $1;}
;

class_member_1				: class_member_0												{$$ = $1;}		
							| NOCODE class_member_0											{$$ = $2; setMemberFilter($$, $1);}
							| NOMETA class_member_0											{$$ = $2; setMemberFilter($$, $1);}
;
			
class_member				: attribute_list_opt class_member_1 native_name_opt ';'			{$$ = $2; setEntityAttributeListOpt($$, $1); setMemberNativeNameOpt($$, $3); setMemberSemicolon($$, $4);}
;

class_member_list			: class_member													{$$ = newMemberList(NULL, $1);}
							| ';'															{$$ = NULL;}
							| class_member_list class_member								{$$ = newMemberList($1, $2);}
							| class_member_list ';'											{$$ = $1;}
;


template_params				: TEMPLATE '<' identifier_list '>'								{$$ = newTemplateParameters($1, $2, $3, $4);}
;

class_0						: class_struct_interface IDENTIFIER								{$$ = newClass($1, NULL, $2);}
							| class_struct_interface '(' identifier_list ')' IDENTIFIER 	{$$ = newClass($1, $3, $5);}
//							| STRUCT IDENTIFIER												{$$ = newClass($1, NULL, $2);}
//							| STRUCT '(' identifier_list ')' IDENTIFIER 					{$$ = newClass($1, $3, $5);}
;

class_1						: class_0														{$$ = $1;}
							| class_0 ':' base_type_name_list								{$$ = $1; setClassBaseList($$, $2, $3);}
;

class_2						: class_1 '{' '}'												{$$ = $1; setClassMemberList($$, $2, NULL, $3);}
							| class_1 '{' class_member_list '}'								{$$ = $1; setClassMemberList($$, $2, $3, $4);}
;

class						: class_2														{$$ = $1;}
							| template_params class_2										{$$ = $2; setClassTemplateParameters($$, $1);}
;


template_class_instance_0	: EXPORT IDENTIFIER '<' type_name_list '>'						{$$ = newTemplateClassInstance($2, $4);}
;
							
template_class_instance		: template_class_instance_0										{$$ = $1;}
							| template_class_instance_0 '{' '}'								{$$ = $1;}
							| template_class_instance_0 '{' identifier_list '}'				{$$ = $1; setTemplateClassInstanceReservedMemberList($1, $3);}
							| template_class_instance_0 '{' identifier_list ',' '}'			{$$ = $1; setTemplateClassInstanceReservedMemberList($1, $3);}
;

namespace_member_0			: class															{$$ = $1;}
							| enum															{$$ = $1;}
							| template_class_instance										{$$ = $1;}
							| typedef														{$$ = $1;}
							| type_declaration												{$$ = $1;}

namespace_member_1			: namespace_member_0 native_name_opt ';'						{$$ = $1; setMemberNativeNameOpt($$, $2); setMemberSemicolon($$, $3);}
							| namespace														{$$ = $1;}
;

namespace_member_2			: namespace_member_1											{$$ = $1;}		
							| NOCODE namespace_member_1										{$$ = $2; setMemberFilter($$, $1);}
							| NOMETA namespace_member_1										{$$ = $2; setMemberFilter($$, $1);}
;

namespace_member			: attribute_list_opt namespace_member_2							{$$ = $2; setEntityAttributeListOpt($$, $1);}
;

namespace_member_list		: namespace_member												{$$ = newMemberList(NULL, $1);}
							| ';'															{$$ = NULL;}
							| namespace_member_list namespace_member						{$$ = newMemberList($1, $2);}
							| namespace_member_list ';'										{$$ = $1;}
;

namespace					: NAMESPACE	IDENTIFIER '{' '}'									{$$ = newNamespace($1, $2, $3, NULL, $4);}
							| NAMESPACE	IDENTIFIER '{' namespace_member_list '}'			{$$ = newNamespace($1, $2, $3, $4, $5);}
;

program						:																{$$ = newProgram(NULL); attachSyntaxTree($$);}
							| namespace_member_list											{$$ = newProgram($1); attachSyntaxTree($$);}
;
