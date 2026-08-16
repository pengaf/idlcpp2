#import "metadata.i"

namespace pafcore
{
#{ 
	class NameSpace; 
#}

	class(dummy_type) #PAFCORE_EXPORT Type : Metadata
	{
		size_t _size_ { get };
#{
		friend class NameSpace;
	public:
		Type(const char* name, MetadataKind kind, RefCountPolicy refCountPolicy, uint32_t size, const char* declarationFile);
		~Type();
	public:
		virtual Metadata* findMember(const char* name) const = 0;
		virtual bool copyConstruct(void* dst, const void* src, size_t count) const = 0;
		virtual bool moveConstruct(void* dst, void* src, size_t count) const = 0;
		virtual bool copyAssign(void* dst, const void* src, size_t count) const = 0;
		virtual bool moveAssign(void* dst, void* src, size_t count) const = 0;
		virtual bool destruct(void* ptr, size_t count) const = 0;
	public:
		void incSharedPtrRefCount(void* ptr) const;
		void decSharedPtrRefCount(void* ptr) const;
		void incSharedArrayRefCount(void* ptr) const;
		void decSharedArrayRefCount(void* ptr) const;
	public:
		bool isPrimitive() const;
		bool isEnum() const;
		bool isClass() const;
		uint32_t size() const;
		MetadataKind kind() const;
		RefCountPolicy refCountPolicy() const;
		Metadata* enclosing() const;
		const char* declarationFile() const;
	protected:
		MetadataKind m_kind;
		RefCountPolicy m_refCountPolicy;
		uint32_t m_size;
		Metadata* m_enclosing;
		const char* m_declarationFile;//file full path
#}
	};

#{

	inline size_t Type::_size_() const
	{
		return m_size;
	}

	inline bool Type::isPrimitive() const
	{
		return MetadataKind::primitive_instance == m_kind;
	}

	inline bool Type::isEnum() const
	{
		return MetadataKind::enum_instance == m_kind;
	}

	inline bool Type::isClass() const
	{
		return MetadataKind::class_instance <= m_kind;
	}

	inline uint32_t Type::size() const
	{
		return m_size;
	}
	
	inline MetadataKind Type::kind() const
	{
		return m_kind;
	}
	
	inline RefCountPolicy Type::refCountPolicy() const
	{
		return m_refCountPolicy;
	}

	inline Metadata* Type::enclosing() const
	{
		return m_enclosing;
	}

	inline const char* Type::declarationFile() const
	{
		return m_declarationFile;
	}


#}

}

