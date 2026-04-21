#import "type.i"
#import "instance_property.i"
#import "instance_field.i"

#{
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
	class Enumerator;
	class TypeAlias;
	class SubclassInvoker;
	class Variant;
#}
	abstract struct #PAFCORE_EXPORT ClassTypeIterator
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

	abstract class(class_type)#PAFCORE_EXPORT ClassType : Type
	{
		size_t _getMemberCount_(bool includeBaseClasses);
		Metadata* _getMember_(size_t index, bool includeBaseClasses);
		Metadata* _findMember_(const char* name, bool includeBaseClasses);
		size_t _getBaseClassCount_();
		Metadata* _getBaseClass_(size_t index);
		ClassTypeIterator* _getFirstDerivedClass_();
		size_t _getInstancePropertyCount_(bool includeBaseClasses);	
		InstanceProperty* _getInstanceProperty_(size_t index, bool includeBaseClasses);//������property��ǰ	
		size_t _getInstanceFieldCount_(bool includeBaseClasses);
		InstanceField* _getInstanceField_(size_t index, bool includeBaseClasses);//������field��ǰ	
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
		ClassType(const char* name, Category category, const char* declarationFile);
	public:
		virtual Metadata* findMember(const char* name) override;
		virtual bool getSmartPointer(Variant& value, const void* address, bool constant, Metadata::TypeCompound typeCompound) override;
		virtual bool setSmartPointer(void* address, Variant& value, Metadata::TypeCompound typeCompound) override;
		virtual void* createSubclassProxy(SubclassInvoker* subclassInvoker);
		virtual void destroySubclassProxy(void* subclassProxy);
		Metadata* findMember(const char* name, bool includeBaseClasses);
		Metadata* findClassMember(const char* name, bool includeBaseClasses, bool typeAliasToType);
	public:
		bool isType(ClassType* otherType);
		bool getClassOffset_(size_t& offset, ClassType* otherType);
		bool getClassOffset(size_t& offset, ClassType* otherType);
		Type* findNestedType(const char* name, bool includeBaseClasses, bool typeAliasToType);
		TypeAlias* findNestedTypeAlias(const char* name, bool includeBaseClasses);
		InstanceField* findInstanceField(const char* name, bool includeBaseClasses);
		StaticField* findStaticField(const char* name, bool includeBaseClasses);
		InstanceProperty* findInstanceProperty(const char* name, bool includeBaseClasses);
		StaticProperty* findStaticProperty(const char* name, bool includeBaseClasses);
		InstanceMethod* findInstanceMethod(const char* name, bool includeBaseClasses);
		StaticMethod* findStaticMethod(const char* name, bool includeBaseClasses);
		bool hasDynamicInstanceField(bool includeBaseClasses) const;
		bool hasDynamicInstanceField() const;
		bool isStringClass() const;
	public:
		InstanceProperty* getInstancePropertyBaseClassFirst(size_t index);//����property��ǰ
		InstanceField* getInstanceFieldBaseClassFirst(size_t index);//����field��ǰ
	private:
		InstanceProperty* getInstancePropertyBaseClassFirst_(size_t& index);//����property��ǰ
		InstanceField* getInstanceFieldBaseClassFirst_(size_t& index);//����field��ǰ
		InstanceProperty* getInstanceProperty_(size_t& index);//����property��ǰ
		InstanceField* getInstanceField_(size_t& index);//����field��ǰ
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
		FlatSet<Metadata*, CompareMetaDataPtrByName> m_resolvedMembers;
		bool m_hasDynamicInstanceField;
		SpecialClass m_specialClass;//�ַ����������⴦��
		//bool m_hasDynamicInstanceProperty;
#}
	};

#{
	inline InstanceProperty* ClassType::getInstancePropertyBaseClassFirst(size_t index)
	{
		InstanceProperty* res = getInstancePropertyBaseClassFirst_(index);
		return res;
	}

	inline InstanceField* ClassType::getInstanceFieldBaseClassFirst(size_t index)
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
