#include "iterator.h"
#include "iterator.mh"
#include "iterator.ic"
#include "iterator.mc"

BEGIN_PAFCORE

bool Iterator::isEnd()
{
	return true;
}

void Iterator::moveNext()
{
}

void Iterator::reset()
{
}

bool Iterator::equal(Iterator* other)
{
	return false;
}

END_PAFCORE
