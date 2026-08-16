#import "metadata.i"

#{
#include "std_unordered_set.h"
#include "std_vector.h"
#}

namespace pafcore
{

	class(name_space)#PAFCORE_EXPORT NameSpace : Metadata
	{
		size_t _getMemberCount_() const;
		Metadata* _getMember_(size_t index) const;
		Metadata* _findMember_(string_t name) const;
#{
	private:
		struct Hash_Metadata
		{
			size_t operator ()(const Metadata* metadata) const;
		};
		struct Equal_Metadata
		{
			bool operator() (const Metadata* lhs, const Metadata* rhs) const;
		};
	public:
		NameSpace(const char* name);
		~NameSpace();
		NameSpace(const NameSpace&) = delete;
		NameSpace& operator=(const NameSpace&) = delete;
	public:
		NameSpace* getNameSpace(const char* name);
		ErrorCode registerMember(Metadata* metadata);
		Metadata* findMember(const char* name) const;
		void unregisterMember(Metadata* metadata);
	public:
		Metadata* enclosing() const
		{
			return m_enclosing;
		}
	protected:	
		typedef pafcore::unordered_set<Metadata*, Hash_Metadata, Equal_Metadata> MetadataContainer;
		pafcore::vector<std::unique_ptr<NameSpace>> m_nameSpaces;
		MetadataContainer m_members;
		Metadata* m_enclosing;
	public:
		static NameSpace* GetGlobalNameSpace();
#}
	};

}
