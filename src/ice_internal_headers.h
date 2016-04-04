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
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "perf.h"

#define REFCOUNT_PERM ((u8)(255))

#include "managed_allocations.h"
#include "iterator.h"

void internal_error(const char* msg);

#ifdef DEBUG
  #define stat_inc(x) stat_inc_(x)
#else
  #define stat_inc(x)
#endif

#if ICE_NO_OVERRIDE_MALLOC
#else
  #define malloc ice_malloc
  #define realloc ice_realloc
  #define free ice_free
#endif
