#ifndef BIGLIT_COMMON
#define BIGLIT_COMMON

// This header exposes the part of the api which is not particular to any one of ℕ, ℤ, ℚ, or ℝ.

#include <assert.h>

static_assert(CHAR_BIT == 8, "target does not have an 8-bit byte");
static_assert(sizeof(uint8_t) == 1, "target uses more than one byte to store uint8_t");
static_assert((int16_t)-1 >> 1 == -1, "target does not perform arithmetic shift on uint16_t");

typedef enum {
  BL_OK = 0,
  BL_OVERFLOW,
  BL_DIVZERO
} bl_result;

typedef enum {
  BL_LT = -1,
  BL_EQ = 0,
  BL_GT = 1
} bl_ord;

#endif