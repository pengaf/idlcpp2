#include "object.h"
#include "object.mh"
#include "object.ic"
#include "object.mc"
#include "metadata.h"
#include "utility.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#ifdef _DEBUG
#include "debug.h"
#endif

BEGIN_PAFCORE

#ifdef _DEBUG

class ObjectLeakReporter
{
public:
	~ObjectLeakReporter()
	{
		m_liveObjects.lock();
		for (auto& item : m_liveObjects.m_objects)
		{
			Object* object = item.first;
			size_t serialNumber = item.second;
			if (!object->isTypeOf<Metadata>())
			{
				char buf[1024];
				pafcore::ClassType* classType = object->getType();
				sprintf_s(buf, "pafcore Warning: Live Object at 0x%p Type: %s, SerialNumber:%zu\n",
					object, classType->_name_().c_str(), serialNumber);
				OutputDebugStringA(buf);
			}
		}
		m_liveObjects.unlock();
	}
public:
	void onObjectConstruct(Object* object)
	{
		m_liveObjects.addPtr(object);
	}
	void onObjectDestruct(Object* object)
	{
		m_liveObjects.removePtr(object);
	}
	void travel(ObjectDebugTraveler* traveler)
	{
		m_liveObjects.lock();
		for (auto& item : m_liveObjects.m_objects)
		{
			Object* object = item.first;
			traveler->onDebugTravel(ObserverPtr<Object>(object));
		}
		m_liveObjects.unlock();
	}
public:
	LiveObjects<Object> m_liveObjects;
public:
	static ObjectLeakReporter* GetInstance()
	{
		static ObjectLeakReporter s_instance;
		return &s_instance;
	}
};

Object::Object()
{
	ObjectLeakReporter::GetInstance()->onObjectConstruct(this);
}

Object::Object(const Object&)
{
	ObjectLeakReporter::GetInstance()->onObjectConstruct(this);
}

Object::~Object()
{
	ObjectLeakReporter::GetInstance()->onObjectDestruct(this);
}

#endif//_DEBUG

#ifndef _DEBUG

Object::~Object()
{
}

#endif//_DEBUG

bool Object::isTypeOf(ClassType* classType)
{
	ClassType* thisType = getType();
	return thisType->isType(classType);
}

void* Object::castTo(ClassType* classType)
{
	size_t offset;
	ClassType* thisType = getType();
	if (thisType->getClassOffset(offset, classType))
	{
		size_t address = reinterpret_cast<size_t>(this);
		return reinterpret_cast<void*>(address + offset);
	}
	return 0;
}

void IncSharedInterfaceStrong(void* rootAddress)
{
	if (0 == rootAddress)
	{
		return;
	}
	Object* object = reinterpret_cast<Object*>(rootAddress);
	RefCountHeader* rc = GetRefCountFromObjectAddress(rootAddress);
	if (pafcore::atomic_rc_object == object->getType()->m_category)
	{
		AtomicInc(rc->strongCount);
	}
	else
	{
		NonAtomicInc(rc->strongCount);
	}
}

void ReleaseSharedInterfaceStrong(void* rootAddress)
{
	if (0 == rootAddress)
	{
		return;
	}
	Object* object = reinterpret_cast<Object*>(rootAddress);
	ClassType* type = object->getType();
	RefCountHeader* rc = GetRefCountFromObjectAddress(rootAddress);
	int32_t strongCount;
	if (pafcore::atomic_rc_object == type->m_category)
	{
		strongCount = AtomicDec(rc->strongCount);
	}
	else
	{
		strongCount = NonAtomicDec(rc->strongCount);
	}
	if (0 == strongCount)
	{
		type->destroyInstance(rootAddress);
	}
}

void Object::DebugTravel(ObserverPtr<ObjectDebugTraveler> traveler)
{
#ifdef _DEBUG
	ObjectLeakReporter::GetInstance()->travel(traveler);
#endif//_DEBUG
}

END_PAFCORE
