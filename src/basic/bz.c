#include "basic/bn.h"
#include "basic/bz.h"

////// Initialization //////

void bz_free(bz src) {
  bn_free(src.magnitude);
}

bz bz_imax(intmax_t src) {
  uintmax_t usrc;
  // the absolute value of INTMAX_MIN may not fit into uintmax_t
  if (src == INTMAX_MIN) {
    usrc = -(src + 1) + 1;
  }
  else {
    usrc = src < 0 ? -src : src;
  }

  bz out = { .isNeg = src < 0, .magnitude = bn_umax(usrc) };
  return out;
}

bz bz_copy(bz src) {
  bz out = { .isNeg = src.isNeg, .magnitude = bn_copy(src.magnitude) };
  return out;
}

////// Conversion //////

bz bz_positive(const bn* src) {
  bz out = { .isNeg = false, .magnitude = bn_copy(src) };
  return out;
}

bz bz_negative(const bn* src) {
  bz out = { .isNeg = true, .magnitude = bn_copy(src) };
  if (out.magnitude->len == 0) { out.isNeg = false; }
  return out;
}

bn* bz_magnitude(bz src) {
  bn* out = bn_copy(src.magnitude);
  return out;
}

////// Queries //////

bl_ord bz_cmp(bz a, bz b) {
  if (a.isNeg && !b.isNeg) {
    return BL_LT; 
  }
  else if (!a.isNeg && b.isNeg) {
    return BL_GT;
  }
  else if (!a.isNeg) {
    return bn_cmp(a.magnitude, b.magnitude);
  }
  else {
    return bn_cmp(b.magnitude, a.magnitude);
  }
}

bool bz_eq(bz a, bz b) {
  return bz_cmp(a, b) == BL_EQ;
}
bool bz_neq(bz a, bz b) {
  return bz_cmp(a, b) != BL_EQ;
}
bool bz_lt(bz a, bz b) {
  return bz_cmp(a, b) == BL_LT;
}
bool bz_lte(bz a, bz b) {
  return bz_cmp(a, b) != BL_GT;
}
bool bz_gt(bz a, bz b) {
  return bz_cmp(a, b) == BL_GT;
}
bool bz_gte(bz a, bz b) {
  return bz_cmp(a, b) != BL_LT;
}


////// Arithmetic //////

bz bz_neg(bz a) {
  bz out = { .isNeg = !a.isNeg, .magnitude = bn_copy(a.magnitude) };
  return out;
}

bz bz_inc(bz a) {
  if (a.isNeg) {
    bz out = { .isNeg = true, .magnitude = bn_dec(a.magnitude) };
    if (out.magnitude->len == 0) {
      out.isNeg = false;
    }
    return out;
  }
  else {
    bz out = { .isNeg = false, .magnitude = bn_inc(a.magnitude) };
    return out;
  }
}

bz bz_add(bz a, bz b) {
  bz out;
  if (a.isNeg == b.isNeg) {
    out.isNeg = a.isNeg;
    out.magnitude = bn_add(a.magnitude, b.magnitude);
  }
  else {
    bl_ord ord = bn_cmp(a.magnitude, b.magnitude);
    switch (ord) {
      case BL_GT: {
        out.isNeg = a.isNeg;
        out.magnitude = bn_sub(a.magnitude, b.magnitude);
      }; break;
      case BL_EQ: {
        out = bz_imax(0);
      } break;
      case BL_LT: {
        out.isNeg = b.isNeg;
        out.magnitude = bn_sub(b.magnitude, a.magnitude);
      } break;
    }
  }
  return out;
}

bz bz_dec(bz a) {
  if (a.isNeg) {
    bz out = { .isNeg = true, .magnitude = bn_inc(a.magnitude) };
    return out;
  }
  else if (a.magnitude->len == 0) {
    return bz_imax(-1);
  }
  else {
    bz out = { .isNeg = false, .magnitude = bn_dec(a.magnitude) };
    return out;
  }
}

bz bz_sub(bz a, bz b) {
  b.isNeg = !b.isNeg;
  return bz_add(a, b);
}

bz bz_mul(bz a, bz b) {
  bn* m = bn_mul(a.magnitude, b.magnitude);
  if (m->len == 0) {
    bz out = { .isNeg = false, .magnitude = m };
    return out;
  }
  else {
    bz out = { .isNeg = a.isNeg ^ b.isNeg, .magnitude = m };
    return out;
  }
}

struct bz_divmod bz_divmod(bz a, bz b) {
  struct bn_divmod res = bn_divmod(a.magnitude, b.magnitude);
  struct bz_divmod out = {
    .div = { .isNeg = false, .magnitude = res.div },
    .mod = { .isNeg = false, .magnitude = res.mod }
  };
  if (res.mod->len == 0) {
    out.div.isNeg = (a.isNeg ^ b.isNeg) & (out.div.magnitude->len != 0);
  }
  else if (a.isNeg & !b.isNeg) {
    out.div.isNeg = out.div.magnitude->len != 0;
    {
      bn* tmp = bn_inc(out.div.magnitude);
      bn_free(out.div.magnitude);
      out.div.magnitude = tmp;
    }
    {
      bn* tmp = bn_sub(b.magnitude, out.mod.magnitude);
      bn_free(out.mod.magnitude);
      out.mod.magnitude = tmp;
    }
  }
  else if (!a.isNeg & b.isNeg) {
    out.div.isNeg = out.div.magnitude->len != 0;
  }
  else if (a.isNeg & b.isNeg) {
    {
      bn* tmp = bn_inc(out.div.magnitude);
      bn_free(out.div.magnitude);
      out.div.magnitude = tmp;
    }
    {
      bn* tmp = bn_sub(b.magnitude, out.mod.magnitude);
      bn_free(out.mod.magnitude);
      out.mod.magnitude = tmp;
    }
  }
  return out;
}

bz bz_div(bz a, bz b) {
  struct bz_divmod r = bz_divmod(a, b);
  bz_free(r.mod);
  return r.div;
}

bz bz_mod(bz a, bz b) {
  struct bz_divmod r = bz_divmod(a, b);
  bz_free(r.div);
  return r.mod;
}
