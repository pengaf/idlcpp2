#import "type.i"
#import "instance_property.i"
#import "instance_field.i"

#{
#include "memory.h"
#include "flat_set.h"
#}

namespace pafcore
{
#{
	class InstanceField;
	class InstanceProperty;
	class InstanceMethod;
	class StaticField;
	class StaticProperty;
	class StaticArrayProperty;
	class StaticMethod;
	class EnumMember;
	class TypeAlias;
	class ScriptInvoker;
	class Variant;
#}
	struct #PAFCORE_EXPORT ClassTypeIterator
	{
		nocode ClassTypeIterator* next();
		nocode ClassType* value();
#{
	public:
		ClassTypeIterator(ClassTypeIterator* iterator, ClassType* classType)
		{
			m_next = iterator;
			m_classType = classType;
		}
		ClassTypeIterator* next()
		{
			return m_next;
		}
		ClassType* value()
		{
			return m_classType;
		}
	protected:
		ClassTypeIterator* m_next;
		ClassType* m_classType;
#}
	};

	class(class_type)#PAFCORE_EXPORT ClassType : Type
	{
		size_t _getMemberCount_(bool includeBaseClasses) const;
		Metadata* _getMember_(size_t index, bool includeBaseClasses) const;
		Metadata* _findMember_(string_t name, bool includeBaseClasses) const;
		size_t _getBaseClassCount_()  const;
		Metadata* _getBaseClass_(size_t index) const;
		ClassTypeIterator* _getFirstDerivedClass_() const;
		size_t _getInstancePropertyCount_(bool includeBaseClasses) const;
		InstanceProperty* _getInstanceProperty_(size_t index, bool includeBaseClasses) const;
		size_t _getInstanceFieldCount_(bool includeBaseClasses) const;
		InstanceField* _getInstanceField_(size_t index, bool includeBaseClasses) const;
#{
	public:
		struct BaseClass
		{
			ClassType* m_type;
			ptrdiff_t m_offset;
		};
		enum SpecialClass : uint8_t 
		{
			not_special_class,
			string_class,
		};
	public:
		ClassType(const char* name, MetadataKind kind, RefCountPolicy refCountPolicy, uint32_t size, const char* declarationFile);
	public:
		virtual Metadata* findMember(const char* name) const override;
		virtual SharedPtr<Interface> createInterfaceProxy(ScriptInvoker* scriptInvoker) const;
		Metadata* findMember(const char* name, bool includeBaseClasses) const;
		Metadata* findClassMember(const char* name, bool includeBaseClasses, bool typeAliasToType) const;
	public:
		bool isType(const ClassType* otherType) const;
		bool getClassOffset_(size_t& offset, const ClassType* otherType) const;
		bool getClassOffset(size_t& offset, const ClassType* otherType) const;
		Type* findNestedType(const char* name, bool includeBaseClasses, bool typeAliasToType) const;
		TypeAlias* findNestedTypeAlias(const char* name, bool includeBaseClasses) const;
		InstanceField* findInstanceField(const char* name, bool includeBaseClasses) const;
		StaticField* findStaticField(const char* name, bool includeBaseClasses) const;
		InstanceProperty* findInstanceProperty(const char* name, bool includeBaseClasses) const;
		StaticProperty* findStaticProperty(const char* name, bool includeBaseClasses) const;
		InstanceMethod* findInstanceMethod(const char* name, bool includeBaseClasses) const;
		StaticMethod* findStaticMethod(const char* name, bool includeBaseClasses) const;
		bool hasDynamicInstanceField(bool includeBaseClasses) const;
		bool hasDynamicInstanceField() const;
		bool isStringClass() const;
	public:
		InstanceProperty* getInstancePropertyBaseClassFirst(size_t index) const;
		InstanceField* getInstanceFieldBaseClassFirst(size_t index) const;
	private:
		InstanceProperty* getInstancePropertyBaseClassFirst_(size_t& index) const;
		InstanceField* getInstanceFieldBaseClassFirst_(size_t& index) const;
		InstanceProperty* getInstanceProperty_(size_t& index) const;
		InstanceField* getInstanceField_(size_t& index) const;
	public:
		BaseClass* m_baseClasses;
		size_t m_baseClassCount;
		ClassTypeIterator* m_classTypeIterators;
		ClassTypeIterator* m_firstDerivedClass;
		Metadata** m_members;
		size_t m_memberCount;
		Metadata** m_classMembers;
		size_t m_classMemberCount;
		Type** m_nestedTypes;
		size_t m_nestedTypeCount;
		TypeAlias** m_nestedTypeAliases;
		size_t m_nestedTypeAliasCount;
		InstanceField* m_instanceFields;
		size_t m_instanceFieldCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
		InstanceMethod* m_instanceMethods;
		size_t m_instanceMethodCount;
		StaticField* m_staticFields;
		size_t m_staticFieldCount;
		StaticProperty* m_staticProperties;
		size_t m_staticPropertyCount;
		StaticMethod* m_staticMethods;
		size_t m_staticMethodCount;
		mutable FlatSet<Metadata*, CompareMetaDataPtrByName> m_resolvedMembers;
		bool m_hasDynamicInstanceField;
		SpecialClass m_specialClass;//
#}
	};

#{
	inline InstanceProperty* ClassType::getInstancePropertyBaseClassFirst(size_t index) const
	{
		InstanceProperty* res = getInstancePropertyBaseClassFirst_(index);
		return res;
	}

	inline InstanceField* ClassType::getInstanceFieldBaseClassFirst(size_t index) const
	{
		InstanceField* res = getInstanceFieldBaseClassFirst_(index);
		return res;
	}

	inline bool ClassType::hasDynamicInstanceField() const
	{
		return m_hasDynamicInstanceField;
	}

	inline bool ClassType::isStringClass() const
	{
		return string_class == m_specialClass;
	}

#}

}
