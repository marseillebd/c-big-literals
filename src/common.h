#ifndef BIGLIT_COMMON
#define BIGLIT_COMMON

// This header exposes the part of the api which is not particular to any one of ℕ, ℤ, ℚ, or ℝ.

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