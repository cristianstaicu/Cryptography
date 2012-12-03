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
const long SEED = 38362178;


int main(int argc, char ** argv) {
	/* Generate 2 big random numbers (512 bits) */
	primitive_p = initialize("1011011");
	initialize_rand(SEED);
	BIGNUM *p = get_long_prime_number(RSA_KEY_LENGTH);
	printf("p=%s\n", BN_bn2hex(p));
	BIGNUM *q = get_long_prime_number(RSA_KEY_LENGTH);
	printf("q=%s\n", BN_bn2hex(q));
	/* Compute phi = (p-1)*(q-1) and n = p*q */
	BIGNUM *phi, *n;
	BN_CTX *tmp;
	tmp = BN_CTX_new();
	n = BN_new();
	phi = BN_new();
	BN_copy(n, p);
	BN_mul(n, n, q, tmp);
	printf("n=%s\n", BN_bn2hex(n));
	BN_sub_word(p, 1);
	printf("p-1=%s\n", BN_bn2hex(p));
	BN_sub_word(q, 1);
	printf("p-1=%s\n", BN_bn2hex(q));
	phi = BN_new();
	BN_init(tmp);
	BN_mul(phi, p, q, tmp);
	printf("(p-1)(q-1)=%s\n", BN_bn2hex(phi));
	/* Find the smallest integer coprime with phi */
	BIGNUM * e = BN_new();
	BIGNUM *gcd = BN_new();
	BN_add_word(e, 3);
	for ( ; ; BN_add_word(e, 1)) {
		tmp = BN_CTX_new();
		BN_gcd(gcd, e, phi, tmp);
		if (BN_is_one(gcd))
			break;
	}
	printf("e=%s\n", BN_bn2hex(e));
	/* Find d, the inverse of e in Z_phi */
	BIGNUM * d = BN_new();
	BIGNUM * i = BN_new();
	BIGNUM * rem = BN_new();
	BIGNUM * prod = BN_new();
	BN_add_word(i, 1);
	for ( ; ; BN_add_word(i, 1)) {
		BN_copy(prod, phi);
		tmp = BN_CTX_new();
		BN_mul(prod, prod, i, tmp);
		BN_add_word(prod, 1);
		BN_div(d, rem, prod, e, tmp);
		if (BN_is_zero(rem)) {
			break;
		}
	}
	printf("d=%s\n", BN_bn2hex(d));
	return 0;
}
