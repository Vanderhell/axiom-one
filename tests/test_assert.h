#ifndef ONE_TEST_ASSERT_H
#define ONE_TEST_ASSERT_H

/* CMake defines NDEBUG for Release builds; tests must keep their checks. */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>

#endif
