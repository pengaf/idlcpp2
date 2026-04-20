#import "type.i"

namespace pafcore
{
	abstract class(type_alias)#PAFCORE_EXPORT TypeAlias : Metadata
	{
		Type* type { get };
#{
	public:
		TypeAlias(const char* name, Type* type, const char* declarationFile);
		~TypeAlias();
	public:
		const char* getDeclarationFile() const;
	public:
		Type* m_type;
		Metadata* m_enclosing;
		const char* m_declarationFile;//�������������ļ�·��
#}
	};

#{
	inline const char* TypeAlias::getDeclarationFile() const
	{
		return m_declarationFile;
	}

#}
}