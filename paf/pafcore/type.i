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
		Type(const char* name, MetadataKind kind, const char* declarationFile);
		~Type();
	public:
		virtual void destruct(void* address, size_t count);
		virtual void assign(void* dst, const void* src);
		virtual Metadata* findMember(const char* name) = 0;
	public:
		bool isPrimitive() const;
		bool isEnum() const;
		bool isClass() const;
		const char* getDeclarationFile() const;
		size_t size() const
		{
			return m_size;
		}
		MetadataKind kind() const
		{
			return m_kind;
		}
	protected:
		MetadataKind m_kind;
		size_t m_size;
		Metadata* m_enclosing;
		const char* m_declarationFile;//file full path
#}
	};

#{


#}

}

