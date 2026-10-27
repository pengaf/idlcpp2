#pragma once

#include "utility.h"

BEGIN_PAFCORE

inline uint64_t fnv1a_64(const uint8_t* data, size_t len, uint64_t hash = 0xcbf29ce484222325ull)
{
	const uint64_t fnv64_prime = 0x00000100000001b3ull;
	for (size_t i = 0; i < len; ++i)
	{
		hash ^= data[i];
		hash *= fnv64_prime;
	}
	return hash;
}

template<typename T>
inline uint64_t fnv1a_64(const T& value, uint64_t hash = 0xcbf29ce484222325ull)
{
	return fnv1a_64(reinterpret_cast<const uint8_t*>(&value), sizeof(T), hash);
}

END_PAFCORE
