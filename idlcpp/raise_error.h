#pragma once

struct IdentifierNode;
struct TypeNameNode;
struct ScopeNameListNode;
struct TokenNode;
struct ParameterNode;
struct MethodNode;
struct OperatorNode;
struct PropertyNode;
struct FieldNode;
struct DelegateNode;

void RaiseError_NestedTemplateClass(IdentifierNode* node);
void RaiseError_InvalidTypeName(IdentifierNode* node);
void RaiseError_InvalidTypeName(ScopeNameListNode* node);
void RaiseError_InvalidTypeName(TypeNameNode* node);
void RaiseError_InvalidParameterType(ParameterNode* node);
void RaiseError_InvalidResultType(MethodNode* node);
void RaiseError_InvalidResultType(OperatorNode* node);
void RaiseError_InvalidResultType(DelegateNode* node);
void RaiseError_InvalidFieldType(FieldNode* node);
void RaiseError_InvalidPropertyType(PropertyNode* node);
void RaiseError_InvalidClassTemplateName(IdentifierNode* node);
void RaiseError_InvalidClassTemplateName(TypeNameNode* node);
void RaiseError_TooManyTemplateArguments(IdentifierNode* node);
void RaiseError_TooFewTemplateArguments(IdentifierNode* node);
void RaiseError_InvalidTemplateArgument(TypeNameNode* node);
void RaiseError_TemplateParameterRedefinition(IdentifierNode* node);
void RaiseError_TemplateClassInstanceInvalidTypeName(const char* typeName, IdentifierNode* node);
void RaiseError_TemplateInterfaceNotSupported(IdentifierNode* node);
void RaiseError_MissingRcObjectBaseType(IdentifierNode* node);
void RaiseError_InterfaceMethodIsNotVirtual(IdentifierNode* node);
void RaiseError_TooFewFormalParameters(OperatorNode* node);
void RaiseError_TooManyFormalParameters(OperatorNode* node);
void RaiseError_StaticOperator(OperatorNode* node);
