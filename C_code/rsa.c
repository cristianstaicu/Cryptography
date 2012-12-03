#include <openssl/bn.h>
#include <openssl/bio.h>

BIGNUM * enc_dec(BIGNUM * a, BIGNUM *b, BIGNUM *n) {
	BIGNUM *res = BN_new();
	BN_copy(res, a);
	BN_CTX *tmp;
	tmp = BN_CTX_new();
//	printf("Ress: %s %s %s\n", BN_bn2dec(a), BN_bn2dec(b), BN_bn2dec(n));
	BN_mod_exp(res, a, b, n, tmp);
//	printf("Ress: %s \n", BN_bn2dec(res));
	return res;
}
