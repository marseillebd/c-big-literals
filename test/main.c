#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/bn.h"

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

int main() {
  bl_result err;

  printf("=== bn__blank ===\n"); {
    printf("%zu\n", bn__sizeof(1024) - sizeof(bn_));
    printf("%zu\n", bn__sizeof(1023) - sizeof(bn_));
    printf("%zu\n", bn__sizeof(7) - sizeof(bn_));
    printf("%zu\n", bn__sizeof(0) - sizeof(bn_));
  }

  // initialization
  bn_* a; {
    a = malloc(bn__sizeof(1024));
    assert(a != NULL);
    a->len = 128;
  }
  bn_* b; {
    b = malloc(bn__sizeof(1024));
    assert(b != NULL);
    b->len = 128;
  }
  bn_* c; {
    c = malloc(bn__sizeof(1024));
    assert(c != NULL);
    c->len = 128;
  }
  bn_* d; {
    d = malloc(bn__sizeof(1024));
    assert(d != NULL);
    d->len = 128;
  }

  printf("=== bn__blank ===\n"); {
    a->base256le[0] = 1;
    a->base256le[4] = 0xff;
    a->base256le[127] = 0x80;
    bn__blank(a);
    printbn(a);
  }

  printf("=== bn__umax ===\n"); {
    err = bn__umax(a, 0); printbn(a); assert(err == BL_OK); a->len = 128;
    err = bn__umax(a, 0xdeadbeef); assert(err == BL_OK); printbn(a); a->len = 128;
    err = bn__umax(a, UINT64_MAX); assert(err == BL_OK); printbn(a); a->len = 128;
  }

  printf("=== bn__bit ===\n"); {
    bn__umax(a, 0xf0);
    for (int i = 0; i < 8; ++i) {
      printf("%d", bn__bit(a, i));
    }
    printf(" %d", bn__bit(a, 87));
    printf(" %d", bn__bit(a, 8*128 - 1));
    printf(" %d\n", bn__bit(a, 8*256));
    a->len = 128; bn__blank(a);
  }

  printf("=== bn__wrbit ===\n"); {
    a->len = 32;
    err = bn__wrbit(a, 42, 1); printbn(a); assert(err == BL_OK);
    err = bn__wrbit(a, 137, 2); printbn(a); assert(err == BL_OK);
    err = bn__wrbit(a, 41, 1); printbn(a); assert(err == BL_OK);
    err = bn__wrbit(a, 42, 0); printbn(a); assert(err == BL_OK);
    err = bn__wrbit(a, 10, 0); printbn(a); assert(err == BL_OK);
    a->len = 128; bn__blank(a);
  }

  printf("=== bn__shr ===\n"); {
    bn__umax(a, 0x54321); printbn(a);
    bn__shr(a, 8); printbn(a);
    bn__shr(a, 1); printbn(a);
    a->len = 128; bn__umax(a, 0x54321); printbn(a);
    bn__shr(a, 17); printbn(a);
    a->len = 128; bn__blank(a);
  }

  printf("=== bn__shl ===\n"); {
    bn__umax(a, 0x54321); a ->len = 4; printbn(a);
    bn__shl(a, 8); printbn(a);
    bn__shl(a, 1); printbn(a);
    a->len = 128; bn__umax(a, 0x54321); a ->len = 5; printbn(a);
    bn__shl(a, 17); printbn(a);
    a->len = 128; bn__blank(a);
  }

  printf("=== bn__cmp ===\n"); {
    bn__umax(a, 0x123);
    bn__umax(b, 0x123);
    bn__umax(c, 0x9876543210);
    printf("%d ", bn__cmp(a, a));
    printf("%d ", bn__cmp(a, b));
    printf("%d ", bn__cmp(b, a));
    printf("%d ", bn__cmp(a, c));
    printf("%d ", bn__cmp(c, a));
    printf("%d\n", bn__cmp(b, c));
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__add ===\n"); {
    bn__umax(a, 0x123);
    bn__umax(b, 0x9876543210);
    c->len = bn__sizeof_add(a, b); bn__blank(c);
    err = bn__add(c, a, b); printbn(c); assert(err == BL_OK);
    bn__umax(a, 0x4);
    bn__umax(b, 0xFFFF);
    c->len = bn__sizeof_add(a, b); bn__blank(c);
    err = bn__add(c, a, b); printbn(c); assert(err == BL_OK);
    bn__umax(a, 0x4);
    bn__umax(b, 0xFFF0);
    c->len = bn__sizeof_add(a, b); bn__blank(c);
    err = bn__add(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__sub ===\n"); {
    bn__umax(a, 0xFFFF); bn__umax(b, 0x4);
    c->len = bn__sizeof_sub(a, b);
    err = bn__sub(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = 128; bn__umax(a, 0x9876543210); bn__umax(b, 0x123);
    c->len = bn__sizeof_sub(a, b);
    err = bn__sub(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
    a->len = b->len = 128; bn__umax(a, 0x01fe); bn__umax(b, 0x00ff);
    c->len = 2;
    err = bn__sub(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__mul ===\n"); {
    bn__umax(a, 0x10);
    bn__umax(b, 0x10);
    c->len = bn__sizeof_mul(a, b);
    err = bn__mul(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(c);

    bn__umax(a, 0x4);
    bn__umax(b, 0x1000);
    c->len = bn__sizeof_mul(a, b);
    err = bn__mul(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(c);

    bn__umax(a, 0xFF);
    bn__umax(b, 0xFFFF);
    c->len = bn__sizeof_mul(a, b);
    err = bn__mul(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(c);

    bn__umax(a, 0x123);
    bn__umax(b, 0x9876543210);
    c->len = bn__sizeof_mul(a, b);
    err = bn__mul(c, a, b); printbn(c); assert(err == BL_OK);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__divmod ===\n"); {
    bn__umax(a, 0x137);
    bn__umax(b, 0x42);
    c->len = bn__sizeof_div(a, b); bn__blank(c);
    d->len = bn__sizeof_mod(a, b); bn__blank(d);
    err = bn__divmod(c, d, a, b); printbn(c); printbn(d); assert(err == BL_OK);
    a->len = b->len = c->len = d->len = 128; bn__blank(a); bn__blank(b); bn__blank(c); bn__blank(d);
    bn__umax(a, 0x42);
    bn__umax(b, 0x137);
    c->len = bn__sizeof_div(a, b); bn__blank(c);
    d->len = bn__sizeof_mod(a, b); bn__blank(d);
    err = bn__divmod(c, d, a, b); printbn(c); printbn(d); assert(err == BL_OK);
    a->len = b->len = c->len = d->len = 128; bn__blank(a); bn__blank(b); bn__blank(c); bn__blank(d);
  }

  printf("=== bn__and ===\n"); {
    bn__umax(a, 0x321);
    bn__umax(b, 0x23);
    c->len = bn__sizeof_and(a, b);
    bn__and(c, a, b); printbn(c);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__or ===\n"); {
    bn__umax(a, 0x321);
    bn__umax(b, 0x23);
    c->len = bn__sizeof_or(a, b);
    bn__or(c, a, b); printbn(c);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  printf("=== bn__xor ===\n"); {
    bn__umax(a, 0x321);
    bn__umax(b, 0x23);
    c->len = bn__sizeof_xor(a, b);
    bn__xor(c, a, b); printbn(c);
    a->len = b->len = c->len = 128; bn__blank(a); bn__blank(b); bn__blank(c);
  }

  // done
  free(a); free(b); free(c);
  return 0;
}
