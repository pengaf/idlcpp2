#pragma once

struct IdentifierNode;
struct TypeNameNode;
struct ScopeNameListNode;
struct TokenNode;
struct MethodNode;
struct OperatorNode;
struct PropertyNode;
struct FieldNode;
struct PropertyAccessorNode;

void RaiseError_NestedTemplateClass(IdentifierNode* node);
//void RaiseError_InvalidTypeName(IdentifierNode* node);
//void RaiseError_InvalidTypeName(TypeNameNode* node);
void RaiseError_InvalidTypeName(ScopeNameListNode* node);
//void RaiseError_InvalidParameterType(ParameterNode* node);
//void RaiseError_InvalidFieldType(FieldNode* node);
//void RaiseError_InvalidPropertyType(PropertyNode* node);
void RaiseError_PropertyAccessorAlreadyExist(PropertyAccessorNode* node, PropertyAccessorNode* existNode);
void RaiseError_PropertyEnumNotAllowPtr(PropertyAccessorNode* node);
void RaiseError_InvalidClassTemplateName(IdentifierNode* node);
void RaiseError_InvalidClassTemplateName(TypeNameNode* node);
void RaiseError_TooManyTemplateArguments(IdentifierNode* node);
void RaiseError_TooFewTemplateArguments(IdentifierNode* node);
void RaiseError_InvalidTemplateArgument(TypeNameNode* node);
void RaiseError_TemplateParameterRedefinition(IdentifierNode* node);
void RaiseError_TemplateClassInstanceInvalidTypeName(const char* typeName, IdentifierNode* node);
void RaiseError_TemplateInterfaceNotSupported(IdentifierNode* node);
void RaiseError_InvalidBaseType(IdentifierNode* node, const char* kind, const char* baseTypeName);
//void RaiseError_InterfaceMethodIsNotVirtual(IdentifierNode* node);
