#include <openssl/bio.h>
#include "prime_generator.h"

/**
 * The CSPRGN is based on the NIST specification 800-90.
 * (http://csrc.nist.gov/publications/nistpubs/800-90A/SP800-90A.pdf)
 *
 * The description of the algorithm can be found in paragraph
 * 10.1 DRBG Mechanisms Based on Hash Functions (page 39)
 */

const int STATE_SIZE = 520;
const int NO_CHECKS_PRIMALITY = 100;

polynom v;
BIGNUM *counter;
polynom c;
BIGNUM* c_bn;

polynom v_padded;
int i = 0;

unsigned char* compress(char x[], int len) {
	int l = len / 8;
	if (len % 8 != 0) {
		l++;
	}
	unsigned char *res = (unsigned char *) malloc(l * sizeof(char));
	int i = 0;
	for (i = 0; i < l; i++) {
		res[i] = x[8 * i] + 2 * x[8 * i + 1] + 4 * x[8 * i + 2]
				+ 8 * x[8 * i + 3] + 16 * x[8 * i + 4] + 32 * x[8 * i + 5]
				+ 64 * x[8 * i + 6] + 128 * x[8 * i + 7];
	}
	return res;
}

BIGNUM *transform_to_bignum(polynom p) {
	BIGNUM *n = BN_new();
	int len = p.size / 8;
	unsigned char* x = compress(p.p, p.size);
	if (p.size % 8 != 0) {
		len++;
	}
	BN_bin2bn(x, len, n);
	free(x);
	return n;
}

polynom transform_to_poly(BIGNUM *n) {
	polynom p;
	int len = BN_num_bytes(n);
	unsigned char *bn_bytes = (unsigned char*) malloc(len * sizeof(char));
	for (i = 0; i < len; i++) {
		bn_bytes[i] = 0;
	}
	BN_bn2bin(n, bn_bytes);
	p.size = len * 8;
	p.p = (char*) malloc(p.size * sizeof(char));
	int i = 0;
	for (i = 0; i < len; i++) {
		if (bn_bytes[i])
			p.p[i * 8] = bn_bytes[i] % 2;
		if ((bn_bytes[i] >> 1) > 0)
			p.p[i * 8 + 1] = (bn_bytes[i] >> 1) % 2;
		if ((bn_bytes[i] >> 2) > 0)
			p.p[i * 8 + 2] = (bn_bytes[i] >> 2) % 2;
		if ((bn_bytes[i] >> 3) > 0)
			p.p[i * 8 + 3] = (bn_bytes[i] >> 3) % 2;
		if ((bn_bytes[i] >> 4) > 0)
			p.p[i * 8 + 4] = (bn_bytes[i] >> 4) % 2;
		if ((bn_bytes[i] >> 5) > 0)
			p.p[i * 8 + 5] = (bn_bytes[i] >> 5) % 2;
		if ((bn_bytes[i] >> 6) > 0)
			p.p[i * 8 + 6] = (bn_bytes[i] >> 6) % 2;
		if ((bn_bytes[i] >> 7) > 0)
			p.p[i * 8 + 7] = (bn_bytes[i] >> 7) % 2;
	}
	free(bn_bytes);
	return p;
}

void initialize_rand(long seed_material) {
	/* Initialize v,c and counter */
	v = init_from_long(seed_material);
	v = sponge_poly(v, STATE_SIZE);
	/* Extend V with 0x00*/
	v_padded.size = v.size + 8;
	v_padded.p = (char *) malloc(v_padded.size * sizeof(char));
	for (i = 0; i < v_padded.size; i++) {
		if (i < v.size)
			v_padded.p[i] = v.p[i];
		else
			v_padded.p[i] = 0;
	}
	c = sponge_poly(v, STATE_SIZE);
	c_bn = transform_to_bignum(c);
	counter = BN_new();
	BN_add_word(counter, 1);
//	printf("---->%s\n", BN_bn2hex(counter));
}

BIGNUM* generate_random_no(int no_bits) {
	BIGNUM *res;
	polynom res_p;
//	print_binary("v",v);
	res_p = sponge_poly(v, no_bits);
//	print_binary("res",res_p);
	res = transform_to_bignum(res_p);
//	printf("%s\n", BN_bn2hex(res));
	/* Modify state*/
	/* Extend V with 0x03*/
	v_padded.size = v.size + 8;
	v_padded.p = (char *) malloc(v_padded.size * sizeof(char));
	for (i = 0; i < v_padded.size; i++) {
		if (i < v.size)
			v_padded.p[i] = v.p[i];
		else
			v_padded.p[i] = 0;
	}
	v_padded.p[v.size] = 1;
	v_padded.p[v.size + 1] = 1;
	v_padded = sponge_poly(v_padded, STATE_SIZE);

	BIGNUM *v_bn = transform_to_bignum(v);
//	printf("%s\n", BN_bn2hex(v_bn));
	BIGNUM *h_bn = transform_to_bignum(v_padded);
	BN_add(v_bn, v_bn, h_bn);
	BN_add(v_bn, v_bn, c_bn);
	BN_add(v_bn, v_bn, counter);
//	printf("%s\n\n", BN_bn2hex(v_bn));
	v = transform_to_poly(v_bn);

	/* Truncate to STATE_SIZE bits */
	v = truncate_poly(v, STATE_SIZE);
//	print_binary("v_end",v);
	BN_add_word(counter, 1);
//	printf("---->%s\n", BN_bn2hex(counter));
	return res;
}

BIGNUM* get_long_prime_number(int no_bits) {
	BIGNUM *n;
	n = generate_random_no(no_bits);
	while (!BN_is_prime(n, NO_CHECKS_PRIMALITY, NULL, NULL, NULL)) {
		n = generate_random_no(no_bits);
		//printf("%s\n", BN_bn2hex(n));
	}
	return n;
}
