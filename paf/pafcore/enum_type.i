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
		size_t _getEnumeratorCount_() const;
		EnumMember* _getEnumerator_(size_t index) const;
		EnumMember* _getEnumeratorByValue_(int value) const;
		EnumMember* _getEnumeratorByName_(string_t name) const;
#{
	public:
		EnumType(const char* name, uint32_t size, const char* declarationFile);
	public:
		EnumMember* findEnumerator(const char* name) const;
	public:
		virtual Metadata* findMember(const char* name) const override;
		virtual bool copyConstruct(void* dst, const void* src, size_t count) const override;
		virtual bool moveConstruct(void* dst, void* src, size_t count) const override;
		virtual bool copyAssign(void* dst, const void* src, size_t count) const override;
		virtual bool moveAssign(void* dst, void* src, size_t count) const override;
		virtual bool destruct(void* ptr, size_t count) const override;
	public:
		static pafcore::ErrorCode Enum_get__name_(pafcore::Variant const& that, pafcore::Variant& value);
	public:
		EnumMember* m_enumerators;
		size_t m_enumeratorCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
#}
	};
}