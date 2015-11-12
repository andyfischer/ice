// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

// Public includes
#include "ice.h"

#define __STDC_CONSTANT_MACROS

#ifdef _MSC_VER
// Special handling for Windows

#undef max
#undef min

// Disable warnings for zero-length array.
#pragma warning(disable:4200)
#pragma warning(disable:4624)

#define _USE_MATH_DEFINES
#include <math.h>
#include <direct.h> 

#endif // Windows

// Standard libraries
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>

namespace ice {

void* ice_malloc(u32 newSize);
void* ice_realloc(void* data, u32 newSize);
void internal_error(const char* msg);

// Assert macros
//
// ice_assert will, when enabled, call internal_error() if the condition is false.
//
// ice_test_assert does the same, but it is only enabled in "test" builds, so it's
// intended to be called in places that significantly harm performance.
#ifdef DEBUG

#ifdef CIRCA_TEST_BUILD

// Test build
#define ice_assert(x) ice::ice_assert_((x), #x, __LINE__, __FILE__)
#define ice_test_assert(x) ice_assert(x)
#define stat_increment(x)

#else

// Debug build
#define ice_assert(x) ice::ice_assert_((x), #x, __LINE__, __FILE__)
#define ice_test_assert(x)
#define stat_increment(x)

#endif

#else

// Release build
#define ice_assert(x)
#define ice_test_assert(x)
#define stat_increment(x)

#endif

void ice_assert_(bool result, const char* expr, int line, const char* file);

} // namespace ice
