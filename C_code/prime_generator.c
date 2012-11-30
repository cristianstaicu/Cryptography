#include <openssl/bn.h>


BIGNUM* get_long_prime_number(int no_digits) {
	BIGNUM *n = BN_new();
	BN_hex2bn(&n, "FF2312312AC9B");
	return n;
}
