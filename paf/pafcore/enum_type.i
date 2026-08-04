#import "type.i"
#import "enum_member.i"


namespace pafcore
{
#{
	class Variant;
	class InstanceProperty;
#}

	class(enum_type)#PAFCORE_EXPORT EnumType : Type
	{
		size_t _getEnumeratorCount_();
		EnumMember* _getEnumerator_(size_t index);
		EnumMember* _getEnumeratorByValue_(int value);
		EnumMember* _getEnumeratorByName_(string_t name);
#{
	public:
		EnumType(const char* name, const char* declarationFile);
	public:
		EnumMember* findEnumerator(const char* name);
		virtual Metadata* findMember(const char* name) override;
	public:
		static pafcore::ErrorCode Enum_get__name_(pafcore::InstanceProperty* instanceProperty, pafcore::Variant* that, pafcore::Variant* value);
	public:
		EnumMember* m_enumerators;
		size_t m_enumeratorCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
#}
	};
}