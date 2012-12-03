#include <openssl/bn.h>
#include <openssl/bio.h>

BIGNUM * enc_dec(BIGNUM * a, BIGNUM *b, BIGNUM *n) {
	BIGNUM *res = BN_new();
	BN_copy(res, a);
	BN_CTX *tmp;
	tmp = BN_CTX_new();
	BN_mod_exp(res, a, b, n, tmp);
	return res;
}
