#include "iterator.h"
#include "iterator.mh"
#include "iterator.ic"
#include "iterator.mc"

BEGIN_PAFCORE

bool Iterator::isEnd() const
{
	return true;
}

void Iterator::advance(int offset)
{
}

void Iterator::reset()
{
}

bool Iterator::equal(Iterator* other) const
{
	return false;
}

END_PAFCORE
