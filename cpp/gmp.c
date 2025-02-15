#include <gmp.h>
#include <stdio.h>

int main() {
  mpz_t n;
  mpz_init(n);
  for(unsigned i = 1; i <= 101; i++) {
    mpz_ui_pow_ui(n, i, 101-i);
    printf("%u^%u=", i, 101-i);
    gmp_printf("%Zd\n", n);
  }

  return 0;
}

// gcc -lgmp gmp.c
// https://www.zhihu.com/question/11842379652