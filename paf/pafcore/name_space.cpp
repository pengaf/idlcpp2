#include "name_space.h"
#include "name_space.mh"
#include "name_space.ic"
#include "name_space.mc"
#include "metadata.h"
#include "type.h"
#include "string.h"
#include <assert.h>

BEGIN_PAFCORE

size_t NameSpace::Hash_Metadata::operator ()(const Metadata* metadata) const
{
	return StringToHash(metadata->m_name);
}

bool NameSpace::Equal_Metadata::operator() (const Metadata* lhs, const Metadata* rhs) const
{
	return (0 == strcmp(lhs->m_name, rhs->m_name));
};

NameSpace::NameSpace(const char* name) : 
	Metadata(name),
	m_enclosing(0)
{
}

NameSpace::~NameSpace()
{
	auto it = m_members.begin();
	auto end = m_members.end();
	for (; it != end; ++it)
	{
		Metadata* member = *it;
		MetadataKind kind = member->_kind_();
		switch (kind)
		{
		case MetadataKind::name_space:
			PAF_ASSERT(static_cast<NameSpace*>(member)->m_enclosing == this);
			static_cast<NameSpace*>(member)->m_enclosing = 0;
			break;
		case MetadataKind::type_alias:
			PAF_ASSERT(static_cast<TypeAlias*>(member)->m_enclosing == this);
			static_cast<TypeAlias*>(member)->m_enclosing = 0;
			break;
		default:
			PAF_ASSERT(MetadataKind::primitive_type == kind || MetadataKind::enum_type == kind || MetadataKind::class_type == kind);
			PAF_ASSERT(static_cast<Type*>(member)->m_enclosing == this);
			static_cast<Type*>(member)->m_enclosing = 0;
		}
	}
}

NameSpace* NameSpace::getNameSpace(const char* name)
{
	char buffer[sizeof(Metadata)];
	Metadata* fakeMetadata = (Metadata*)buffer;
	fakeMetadata->m_name = name;
	NameSpace* subNameSpace = nullptr;
	if(0 != this)
	{
		auto it = m_members.find(fakeMetadata);
		if(m_members.end() == it)
		{
			std::unique_ptr<NameSpace> nameSpace = std::make_unique<NameSpace>(name);
			subNameSpace = nameSpace.get();
			subNameSpace->m_enclosing = this;
			m_members.insert(subNameSpace);
			m_nameSpaces.push_back(std::move(nameSpace));
		}
		else
		{
			Metadata* member = *it;
			if(MetadataKind::name_space == member->_kind_())
			{
				subNameSpace = static_cast<NameSpace*>(member);
				PAF_ASSERT(this == subNameSpace->m_enclosing);
			}
		}
	}
	return subNameSpace;
}

ErrorCode NameSpace::registerMember(Metadata* member)
{
	if(0 == this)
	{
		return ErrorCode::e_invalid_namespace;
	}
	MetadataKind kind = member->_kind_();
	if (MetadataKind::type_alias == kind)
	{
		static_cast<TypeAlias*>(member)->m_enclosing = this;
	}
	else
	{
		PAF_ASSERT(MetadataKind::primitive_type == kind || MetadataKind::enum_type == kind || MetadataKind::class_type == kind);
		static_cast<Type*>(member)->m_enclosing = this;
	}
	return m_members.insert(member).second ? ErrorCode::s_ok : ErrorCode::e_name_conflict;
}

void NameSpace::unregisterMember(Metadata* metadata)
{
	m_members.erase(metadata);
}

Metadata* NameSpace::_findMember_(string_t name) const
{
	Metadata* member = nullptr;
	char buffer[sizeof(Metadata)];
	Metadata* fakeMetadata = (Metadata*)buffer;
	fakeMetadata->m_name = name;
	auto it = m_members.find(fakeMetadata);
	if (m_members.end() != it)
	{
		member = *it;
	}
	return member;
}

size_t NameSpace::_getMemberCount_() const
{
	return m_members.size();
}

Metadata* NameSpace::_getMember_(size_t index) const
{
	if (index < m_members.size())
	{
		auto it = m_members.begin();
		std::advance(it, index);
		return *it;
	}
	else
	{
		return nullptr;
	}
}

Metadata* NameSpace::findMember(const char * name) const
{
	Metadata* member = _findMember_(name);
	if(0 != member)
	{
		if(member->_kind_() == MetadataKind::type_alias)
		{
			member = static_cast<TypeAlias*>(member)->m_type;
		}
	}
	return member;
}

NameSpace* NameSpace::GetGlobalNameSpace()
{
	static NameSpace s_globalNameSpace("");
	return &s_globalNameSpace;
}

END_PAFCORE
