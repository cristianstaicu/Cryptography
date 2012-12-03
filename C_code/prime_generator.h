#include <openssl/bn.h>
#include "sponge.h"

void initialize_rand(long seed_material);
BIGNUM* generate_random_no(int no_bits);
BIGNUM* get_long_prime_number(int no_bits);
polynom transform_to_poly(BIGNUM *n);
BIGNUM *transform_to_bignum(polynom p);
