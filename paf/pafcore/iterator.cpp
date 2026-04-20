#include "iterator.h"
#include "iterator.mh"
#include "iterator.ic"
#include "iterator.mc"

BEGIN_PAFCORE

bool Iterator::isEnd() const
{
	return true;
}

void Iterator::moveNext()
{
}

void Iterator::reset()
{
}

bool Iterator::equal(Iterator* other) const
{
	return false;
}

long_t Iterator::addRef()
{
	return ++m_refCount;
}

long_t Iterator::release()
{
	long_t refCount = --m_refCount;
	if (0 == refCount)
	{
		paf_delete(this);
	}
	return refCount;
}

END_PAFCORE
