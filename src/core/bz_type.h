#ifndef BIGLIT_CORE_BZ_TYPE
#define BIGLIT_CORE_BZ_TYPE

#include "core/bn.h"


typedef struct bz_ {
  bool isNeg;
  bn_* magnitude;
} bz_;


#endif
