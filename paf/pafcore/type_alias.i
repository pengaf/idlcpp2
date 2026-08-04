#import "type.i"

namespace pafcore
{
#{ 
	class NameSpace;
#}

	class(type_alias)#PAFCORE_EXPORT TypeAlias : Metadata
	{
		Type* type { get };
#{
		friend class NameSpace;
	public:
		TypeAlias(const char* name, Type* type, const char* declarationFile);
		~TypeAlias();
	public:
		const char* getDeclarationFile() const;
	public:
		Type* m_type;
		Metadata* m_enclosing;
		const char* m_declarationFile;//file full path
#}
	};

#{
	inline const char* TypeAlias::getDeclarationFile() const
	{
		return m_declarationFile;
	}

#}
}