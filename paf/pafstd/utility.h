#pragma once
#include "../pafcore/utility.h"

#if defined(_WIN32)
#if defined(PAFSTD_EXPORTS)
#define PAFSTD_EXPORT __declspec(dllexport)
#else
#define PAFSTD_EXPORT __declspec(dllimport)
#endif
#elif defined(PAFSTD_EXPORTS)
#define PAFSTD_EXPORT __attribute__((visibility("default")))
#else
#define PAFSTD_EXPORT
#endif

#define BEGIN_PAFSTD namespace pafstd {
#define END_PAFSTD }
