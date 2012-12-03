#include <openssl/bn.h>
#include <stdio.h>
#include "prime_generator.h"

const int RSA_KEY_LENGTH = 512;
const int SEED = 11562;


int main(int argc, char ** argv) {
	initialize_rand(SEED);
	BIGNUM *p;
	p = generate_random_no(RSA_KEY_LENGTH);
//	printf("%s", BN_bn2hex(p));
	return 0;
}
