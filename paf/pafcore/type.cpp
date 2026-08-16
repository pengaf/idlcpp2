#include "type.h"
#include "type.mh"
#include "type.ic"
#include "type.mc"
#include "name_space.h"

BEGIN_PAFCORE

Type::Type(const char* name, MetadataKind kind, RefCountPolicy refCountPolicy, uint32_t size, const char* declarationFile) :
	Metadata(name),
	m_kind(kind),
	m_refCountPolicy(refCountPolicy),
	m_size(size),
	m_enclosing(nullptr),
	m_declarationFile(declarationFile)
{}

Type::~Type()
{
	if (m_enclosing && MetadataKind::name_space == m_enclosing->_kind_())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}

void Type::incSharedPtrRefCount(void* ptr) const
{
	PAF_ASSERT(RefCountPolicy::single_thread == m_refCountPolicy || RefCountPolicy::multi_thread == m_refCountPolicy);
	if (RefCountPolicy::single_thread == m_refCountPolicy)
	{
		IncStrongRefCount<STRCHeader>(ptr);
	}
	else
	{
		IncStrongRefCount<MTRCHeader>(ptr);
	}
}

void Type::decSharedPtrRefCount(void* ptr) const
{
	PAF_ASSERT(RefCountPolicy::single_thread == m_refCountPolicy || RefCountPolicy::multi_thread == m_refCountPolicy);
	if (RefCountPolicy::single_thread == m_refCountPolicy)
	{
		DecStrongRefCount<STRCHeader>(ptr, [this](void* object) { this->destruct(object, 1); });
	}
	else
	{
		DecStrongRefCount<MTRCHeader>(ptr, [this](void* object) { this->destruct(object, 1); });
	}
}

void Type::incSharedArrayRefCount(void* ptr) const
{
	IncArrayRefCount(ptr);
}

void Type::decSharedArrayRefCount(void* ptr) const
{
	DecArrayRefCount(ptr, [this](void* array, size_t count) { this->destruct(array, count); });
}

END_PAFCORE
