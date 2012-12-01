#include <openssl/bn.h>

const int NO_CHECKS_PRIMALITY = 100;

void initialize_rand(long seed) {
	/* Add the seed to the state */
}

BIGNUM* generate_random_no(int no_bits) {
	BIGNUM *n = BN_new();
	BN_hex2bn(&n, "DF3");
	return n;
}

BIGNUM* get_long_prime_number(long seed, int no_bits) {
	BIGNUM *n;
	initialize_rand(seed);
	n = generate_random_no(no_bits);
	while (!BN_is_prime(n, NO_CHECKS_PRIMALITY, NULL, NULL, NULL)) {
		n = generate_random_no(no_bits);
	}
	return n;
}
