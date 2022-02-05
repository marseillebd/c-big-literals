#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic/bz.h"

void printbn(bn_* src) {
  if (src->len == 0) {
    printf("0\n");
  }
  else {
    for (size_t i = src->len; i > 0; --i) {
      printf("%02X", src->base256le[i-1]);
    }
    printf("\n");
  }
}

void printbz(bz src) {
  if (src.isNeg) { printf("-"); }
  printbn(src.magnitude);
}

int main() {
  bz a, b, c;

  printf("=== bz_imax ===\n"); {
    a = bz_imax(0x42); printbz(a); bz_free(a);
    a = bz_imax(-0x137); printbz(a); bz_free(a);
    a = bz_imax(INTMAX_MAX); printbz(a); bz_free(a);
    a = bz_imax(INTMAX_MIN); printbz(a); bz_free(a);
  }

  printf("=== bz_copy ===\n"); {
    a = bz_imax(-0x137);
    b = bz_copy(a); printbz(a);
    assert(a.magnitude != b.magnitude);
    bz_free(a); bz_free(b);
  }

  printf("=== bz_cmp ===\n"); {
    a = bz_imax(-0x137);
    b = bz_imax(-0x137);
    printf("%d ", bz_cmp(a, b));
    printf("%d ", bz_cmp(b, a));
    b.isNeg = false;
    printf("%d ", bz_cmp(a, b));
    printf("%d ", bz_cmp(b, a));
    a.isNeg = false;
    printf("%d ", bz_cmp(a, b));
    printf("%d ", bz_cmp(b, a));
    b.isNeg = true;
    printf("%d ", bz_cmp(a, b));
    printf("%d\n", bz_cmp(b, a));
    bz_free(a); bz_free(b);

    a = bz_imax(-0x42);
    b = bz_imax(-0x137);
    printf("%d ", bz_cmp(a, b));
    printf("%d\n", bz_cmp(b, a));
    bz_free(a); bz_free(b);
  }

  printf("=== bz_add ===\n"); {
    a = bz_imax(0xdeadbeef);
    b = bz_imax(0x11);
    c = bz_add(a, b); printbz(c); bz_free(c);
    c = bz_add(b, a); printbz(c); bz_free(c);
    a.isNeg = false; b.isNeg = true;
    c = bz_add(a, b); printbz(c); bz_free(c);
    c = bz_add(b, a); printbz(c); bz_free(c);
    a.isNeg = true; b.isNeg = false;
    c = bz_add(a, b); printbz(c); bz_free(c);
    c = bz_add(b, a); printbz(c); bz_free(c);
    a.isNeg = true; b.isNeg = true;
    c = bz_add(a, b); printbz(c); bz_free(c);
    c = bz_add(b, a); printbz(c); bz_free(c);
    bz_free(a); bz_free(b);

    a = bz_imax(0xdeadbeef);
    b = a; b.isNeg = true;
    c = bz_add(a, b); printbz(c); bz_free(c);
    bz_free(a);
  }

  printf("=== bz_sub ===\n"); {
    a = bz_imax(0xdeadbeef);
    b = bz_imax(0x11);
    c = bz_sub(a, b); printbz(c); bz_free(c);
    c = bz_sub(b, a); printbz(c); bz_free(c);
    bz_free(a); bz_free(b);
  }

  printf("=== bz_neg ===\n"); {
    a = bz_imax(0xdeadbeef);
    b = bz_neg(a); printbz(b); bz_free(b);
    assert(a.magnitude != b.magnitude);
    bz_free(a);
  }

  printf("=== bz_mul ===\n"); {
    a = bz_imax(0xFFFF);
    b = bz_imax(0xF);
    c = bz_mul(a, b); printbz(c); bz_free(c);
    a.isNeg = true; b.isNeg = false;
    c = bz_mul(a, b); printbz(c); bz_free(c);
    a.isNeg = true; b.isNeg = true;
    c = bz_mul(a, b); printbz(c); bz_free(c);
    a.isNeg = false; b.isNeg = true;
    c = bz_mul(a, b); printbz(c); bz_free(c);

    bz_free(b); b = bz_imax(0);
    a.isNeg = false;
    c = bz_mul(a, b); printbz(c); bz_free(c);
    a.isNeg = true;
    c = bz_mul(a, b); printbz(c); bz_free(c);
    bz_free(a); bz_free(b);
  }

  printf("=== bz_divmod ===\n"); {
    a = bz_imax(0x5032);
    b = bz_imax(0x2000);
    struct bz_divmod r;
    r = bz_divmod(a, b); printbz(r.div); printbz(r.mod); bz_free(r.div); bz_free(r.mod);
    a.isNeg = true; b.isNeg = false;
    r = bz_divmod(a, b); printbz(r.div); printbz(r.mod); bz_free(r.div); bz_free(r.mod);
    a.isNeg = true; b.isNeg = true;
    r = bz_divmod(a, b); printbz(r.div); printbz(r.mod); bz_free(r.div); bz_free(r.mod);
    a.isNeg = false; b.isNeg = true;
    r = bz_divmod(a, b); printbz(r.div); printbz(r.mod); bz_free(r.div); bz_free(r.mod);
    bz_free(a); bz_free(b);
  }

  return 0;
}