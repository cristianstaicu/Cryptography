#include <stdio.h>
#include "mintest.h"
#include "all5.h"
#include "maj5.h"
#include <time.h>
#include "prime_generator.h"
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <stdlib.h>

const int RSA_KEY_LENGTH = 512;
const long SEED = 38712781324;


int main(int argc, char ** argv) {
	primitive_p = initialize("1011011");
	initialize_rand(SEED);
	BIGNUM *p = get_long_prime_number(RSA_KEY_LENGTH);
	printf("p=%s\n", BN_bn2hex(p));
	BIGNUM *q = get_long_prime_number(RSA_KEY_LENGTH);
	printf("q=%s\n", BN_bn2hex(q));
	BIGNUM *prod;
	BN_CTX *tmp;
	BN_sub_word(p, 1);
	printf("p-1=%s\n", BN_bn2hex(p));
	BN_sub_word(q, 1);
	printf("p-1=%s\n", BN_bn2hex(q));
	prod = BN_CTX_new();
	tmp = BN_new();
	BN_init(tmp);
	BN_mul(prod, p, q, tmp);
	printf("(p-1)(q-1)=%s\n", BN_bn2hex(prod));
	return 0;
}
