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

bool Iterator::equal(ObserverPtr<Iterator> other) const
{
	return false;
}

END_PAFCORE
