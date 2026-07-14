#import "metadata.i"

namespace pafcore
{
#{ class Variant; #}
	abstract class(dummy_type) #PAFCORE_EXPORT Type : Metadata
	{
		size_t _size_ { get };
#{
	public:
		Type(const char* name, MetadataKind kind, const char* declarationFile);
		~Type();
	public:
		virtual void destruct(void* address, size_t count);
		virtual void copyConstruct(void* address);
		virtual bool copyAssign(void* dst, const void* src);
		virtual Metadata* findMember(const char* name) = 0;
	public:
		bool isPrimitive() const;
		bool isEnum() const;
		bool isValue() const;
		bool isRcObject() const;
		bool isClass() const;
		const char* getDeclarationFile() const;
		size_t size() const
		{
			return m_size;
		}
	protected:
		MetadataKind m_kind;
		size_t m_size;
		Metadata* m_enclosing;
		const char* m_declarationFile;//�������������ļ�·��
#}
	};

#{

inline size_t Type::_size_() const
	{
		return m_size;
	}

	inline bool Type::isPrimitive() const
	{
		return primitive_instance == m_kind;
	}
	
	inline bool Type::isEnum() const
	{
		return enum_instance == m_kind;
	}
	
	inline bool Type::isValue() const
	{
		return value_instance == m_kind;
	}
	
	inline bool Type::isRcObject() const
	{
		return object_instance <= m_kind;
	}

	inline bool Type::isClass() const
	{
		return isValue() || isRcObject();
	}
	
	inline const char* Type::getDeclarationFile() const
	{
		return m_declarationFile;
	}

#}

}

