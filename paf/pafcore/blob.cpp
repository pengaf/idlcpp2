#include "blob.h"
#include "blob.mh"
#include "blob.ic"
#include "blob.mc"

BEGIN_PAFCORE

//Blob::Blob(void* data, size_t size) : m_data(data), m_size(size)
//{}

Blob::Blob(size_t size)
{
	m_data = Malloc(size);
	m_size = size;
}

Blob::~Blob()
{
	if (m_data)
	{
		Free(m_data);
	}
}

END_PAFCORE
