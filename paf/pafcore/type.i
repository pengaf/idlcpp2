#import "metadata.i"

namespace pafcore
{
#{ class Variant; #}
	abstract class #PAFCORE_EXPORT Type : Metadata
	{
		size_t _size_ { get };
#{
	public:
		Type(const char* name, Category category, const char* declarationFile);
		~Type();
	public:
		virtual void destroyInstance(void* address);
		virtual void destroyArray(void* address);
		virtual bool assign(void* dst, const void* src);
		virtual bool getSmartPointer(Variant& value, const void* address, bool constant, Metadata::TypeCompound typeCompound);
		virtual bool setSmartPointer(void* address, Variant& value, Metadata::TypeCompound typeCompound);
		virtual Metadata* findMember(const char* name) = 0;
	public:
		bool isPrimitive() const;
		bool isEnum() const;
		bool isValue() const;
		bool isRcObject() const;
		bool isClass() const;
		const char* getDeclarationFile() const;
	public:
		Category m_category;
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
		return primitive_object == m_category;
	}
	
	inline bool Type::isEnum() const
	{
		return enum_object == m_category;
	}
	
	inline bool Type::isValue() const
	{
		return value_object == m_category;
	}
	
	inline bool Type::isRcObject() const
	{
		return rc_object <= m_category;
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

