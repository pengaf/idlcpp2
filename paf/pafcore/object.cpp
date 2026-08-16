#include "object.h"
#include "object.mh"
#include "object.ic"
#include "object.mc"
#include "class_type.h"
#include "memory.h"

BEGIN_PAFCORE

bool Introspectable::isTypeOf(ClassType* classType)
{
	ClassType* thisType = getType();
	return thisType->isType(classType);
}

void* Introspectable::castTo(ClassType* classType)
{
	size_t offset;
	ClassType* thisType = getType();
	if (thisType->getClassOffset(offset, classType))
	{
		size_t address = reinterpret_cast<size_t>(this);
		return reinterpret_cast<void*>(address + offset);
	}
	return nullptr;
}

uint32_t STRCObject::incStrongRefCount() noexcept
{
	return IncStrongRefCount<STRCHeader>(this);
}

uint32_t STRCObject::incWeakRefCount() noexcept
{
	return IncWeakRefCount<STRCHeader>(this);
}

uint32_t STRCObject::decStrongRefCount() noexcept
{
	return DecStrongRefCount<STRCHeader>(this);
}

uint32_t STRCObject::decWeakRefCount() noexcept
{
	return DecWeakRefCount<STRCHeader>(this);
}

uint32_t STRCObject::getStrongRefCount() noexcept
{
	return GetStrongRefCount<STRCHeader>(this);
}

uint32_t STRCObject::getWeakRefCount() noexcept
{
	return GetWeakRefCount<STRCHeader>(this);
}

uint32_t STRCObject::refCountOperation(RefCountOp op) noexcept
{
	switch(op)
	{
	case RefCountOp::inc_strong:
		return incStrongRefCount();
	case RefCountOp::inc_weak:
		return incWeakRefCount();
	case RefCountOp::dec_strong:
		return decStrongRefCount();
	case RefCountOp::dec_weak:
		return decWeakRefCount();
	case RefCountOp::get_strong:
		return getStrongRefCount();
	case RefCountOp::get_weak:
		return getWeakRefCount();
	}
	return 0;
}


uint32_t MTRCObject::incStrongRefCount() noexcept
{
	return IncStrongRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::incWeakRefCount() noexcept
{
	return IncWeakRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::decStrongRefCount() noexcept
{
	return DecStrongRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::decWeakRefCount() noexcept
{
	return DecWeakRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::getStrongRefCount() noexcept
{
	return GetStrongRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::getWeakRefCount() noexcept
{
	return GetWeakRefCount<MTRCHeader>(this);
}

uint32_t MTRCObject::refCountOperation(RefCountOp op) noexcept
{
	switch (op)
	{
	case RefCountOp::inc_strong:
		return incStrongRefCount();
	case RefCountOp::inc_weak:
		return incWeakRefCount();
	case RefCountOp::dec_strong:
		return decStrongRefCount();
	case RefCountOp::dec_weak:
		return decWeakRefCount();
	case RefCountOp::get_strong:
		return getStrongRefCount();
	case RefCountOp::get_weak:
		return getWeakRefCount();
	}
	return 0;
}

END_PAFCORE
