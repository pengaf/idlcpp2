#pragma once
#include "utility.h"
#include "std_unordered_map.h"
#include <mutex>

BEGIN_PAFCORE

template<typename T>
class LiveObjects
{
public:
	LiveObjects() : m_serialNumber(0)
	{
	}
	~LiveObjects()
	{
	}
public:
	void addPtr(T* p)
	{
		m_mutex.lock();
		m_objects.insert(std::make_pair(p, m_serialNumber++));
		m_mutex.unlock();
	}
	void removePtr(T* p)
	{
		m_mutex.lock();
		m_objects.erase(p);
		m_mutex.unlock();
	}
	void lock()
	{
		m_mutex.lock();
	}
	void unlock()
	{
		m_mutex.unlock();
	}
public:
	pafcore::unordered_map<T*, size_t> m_objects;
	std::mutex m_mutex;
	size_t m_serialNumber;
};


END_PAFCORE
