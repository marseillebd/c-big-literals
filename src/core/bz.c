#include "core/bz.h"

void bz__normalize(bz_ dst) {
  bn__normalize(dst.magnitude);
  if (dst.magnitude->len == 0) {
    dst.isNeg = false;
  }
}
