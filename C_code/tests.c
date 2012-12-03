#include <stdio.h>
#include "mintest.h"
#include "all5.h"
#include "maj5.h"
#include <time.h>
#include "prime_generator.h"
#include "rsa.c"
#include <openssl/bn.h>
#include <openssl/bio.h>

int tests_run = 0;
int failed = 0;

/* Wikipedia example */
static char * test_addition() {
	polynom a = initialize("01010011");
	polynom b = initialize("11001010");
	polynom c = add(a, b);
	mu_assert("Addition is not working in finite field arithmetic!",
			equals(c, "10011001"));
	return 0;
}

/* Rindjael wiki example: 53 x CA*/
static char * test_mult() {
	polynom a = initialize("11001010");
	polynom b = initialize("01010011");
	polynom p = initialize("100011011");
	polynom c = mult(a, b, p);
	mu_assert("Multiplication is not working in finite field arithmetic!",
			equals(c, "00000001"));
	return 0;
}

static char * test_keys() {
	polynom k = initialize("100100011011101001100000");
	polynom *round_keys = compute_keys(k);
	mu_assert("K0 wrong!", equals(round_keys[0], "110011100111101010001010"));
	mu_assert("K1 wrong!", equals(round_keys[1], "101000100011001011100001"));
	mu_assert("K2 wrong!", equals(round_keys[2], "000001001111000001001010"));
	mu_assert("K3 wrong!", equals(round_keys[3], "100001000011100110100000"));
	mu_assert("K4 wrong!", equals(round_keys[4], "101111101011000101101011"));
	mu_assert("K5 wrong!", equals(round_keys[5], "100000110111001001001011"));
	mu_assert("K6 wrong!", equals(round_keys[6], "000110010111000000100010"));
	mu_assert("K7 wrong!", equals(round_keys[7], "000011111100010111101000"));
	mu_assert("K8 wrong!", equals(round_keys[8], "001001001100100000100001"));
	mu_assert("K9 wrong!", equals(round_keys[9], "111101001010110111100001"));
	mu_assert("K10 wrong!", equals(round_keys[10], "001110001111001001101110"));
	mu_assert("K11 wrong!", equals(round_keys[11], "101110101110011011111110"));
	mu_assert("K12 wrong!", equals(round_keys[12], "011001001111000000101110"));
	mu_assert("K13 wrong!", equals(round_keys[13], "010010111110001111100111"));
	mu_assert("K14 wrong!", equals(round_keys[14], "100111010000100001111100"));
	mu_assert("K15 wrong!", equals(round_keys[15], "101100000111010101101010"));
	return 0;
}

static char * test_mixing_layer() {
	polynom *e = (polynom *) malloc(4 * sizeof(polynom));
	e[0] = initialize("110011");
	e[1] = initialize("100100");
	e[2] = initialize("000101");
	e[3] = initialize("100000");
	polynom *res = MixingLayer(e);
	mu_assert("ML applied on e1 is wrong!", equals(res[0], "011011"));
	mu_assert("ML applied on e2 is wrong!", equals(res[1], "001000"));
	mu_assert("ML applied on e3 is wrong!", equals(res[2], "101100"));
	mu_assert("ML applied on e4 is wrong!", equals(res[3], "110111"));
	res = InvMixingLayer(res);
	mu_assert("INVML applied on e1 is wrong!", equals(res[0], "110011"));
	mu_assert("INVML applied on e2 is wrong!", equals(res[1], "100100"));
	mu_assert("INVML applied on e3 is wrong!", equals(res[2], "000101"));
	mu_assert("INVML applied on e4 is wrong!", equals(res[3], "100000"));
	return 0;
}

static char * test_bunny_int() {
	polynom m = initialize("001001110101100000111100");
	polynom k = initialize("111101001101010011010000");
	polynom c = BunnyTn(m, k);
	mu_assert("You've got yourself a sad bunny! :(",
			equals(c,"111110010000010001100110"));

	m = initialize("011100101101101011011100");
	k = initialize("100010100011001110011110");
	c = BunnyTn(m, k);
	mu_assert("You've got yourself a sad bunny(2)! :(",
			equals(c,"101111110000110000101011"));

	m = initialize("101100110000000000000000");
	k = initialize("111111111111111111111111");
	c = BunnyTn(m, k);
	mu_assert("You've got yourself a sad bunny(3)! :(",
			equals(c,"001110000011110000000001"));

	return 0;
}

static char * test_split_concat() {
	polynom m = initialize("001001110101100000111100");
	polynom *splitm = split(m, 4);
	mu_assert("Split performed incorrect!", equals(splitm[0], "001001"));
	m = concat(splitm, 4);
	mu_assert("Concat performed incorrect!",
			equals(m, "001001110101100000111100"));
	return 0;
}

static char * test_inv_sboxs() {
	polynom p = initialize("101010");
	polynom inv_p = INV_Sbox1(p);
	mu_assert("Sbox1 inverse not working!", equals(inv_p, "011100"));
	p = initialize("100101");
	inv_p = INV_Sbox2(p);
	mu_assert("Sbox2 inverse not working!", equals(inv_p, "110010"));
	p = initialize("100011");
	inv_p = INV_Sbox3(p);
	mu_assert("Sbox3 inverse not working!", equals(inv_p, "100010"));
	p = initialize("001001");
	inv_p = INV_Sbox4(p);
	mu_assert("Sbox4 inverse not working!", equals(inv_p, "111110"));
	return 0;
}

static char * test_dec_bunny_int() {
	polynom c = initialize("111110010000010001100110");
	polynom k = initialize("111101001101010011010000");
	polynom m = DecBunnyTn(c, k);
	mu_assert("You've got yourself a sad bunny -> bad decryption! :(",
			equals(m,"001001110101100000111100"));
	return 0;
}

static char * test_lsfr() {
	char *res = lsfr("1011", "101", 10);
	mu_assert("LSFR is not working!", strcmp(res, "1100101110") == 0);
	res = lsfr("1011", "110", 10);
	mu_assert("LSFR is not working!", strcmp(res, "1001011100") == 0);
	res = lsfr("101000000001", "10111100011", 20);
	mu_assert("LSFR is not working!", strcmp(res, "10101100100100011110") == 0);
	return 0;
}

const char* quads[] = { "0000", "0001", "0010", "0011", "0100", "0101", "0110",
		"0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111" };

const char * hex_to_bin_quad(unsigned char c) {
	if (c >= '0' && c <= '9')
		return quads[c - '0'];
	if (c >= 'A' && c <= 'F')
		return quads[10 + c - 'A'];
	if (c >= 'a' && c <= 'f')
		return quads[10 + c - 'a'];
	return -1;
}

char * hex_to_binary(char* hex_array) {
	int len = strlen(hex_array);
	char *res = (char*) malloc((len * 4 + 1) * sizeof(char));
	int i = 0;
	for (i = 0; i < len; i++) {
		char *quad = hex_to_bin_quad(hex_array[i]);
		int j;
		for (j = 0; j < 4; j++) {
			res[i * 4 + j] = quad[j];
		}
	}
	res[len * 4 + 1] = 0;
	return res;
}

char * binary_to_hex(char * bin_array) {
	int len = strlen(bin_array);
	int i, j = 0;
	char *res = (char*) malloc((len / 4 + 1) * sizeof(char));
	for (i = 0; i < len; i += 4) {
		int n = (bin_array[i] - '0') * 8 + (bin_array[i + 1] - '0') * 4
				+ (bin_array[i + 2] - '0') * 2 + (bin_array[i + 3] - '0');
		if (n < 10) {
			res[j++] = '0' + n;
		} else {
			res[j++] = 'A' + (n - 10);
		}
	}
	res[len / 4] = 0;
	return res;
}

static char * hex_to_bin_test() {
	char *res = hex_to_binary("faC0");
	mu_assert("Hex to bin conversion is not working!", strlen(res) == 16);
	mu_assert("Hex to bin conversion is not working!",
			strcmp(res, "1111101011000000") == 0);
	res = binary_to_hex(res);
	mu_assert("Bin to hex conversion is not working!",
			strcmp(res, "FAC0") == 0);
	return 0;
}

static char * test_cipher_block_chaining() {
	mu_assert("Hex to bin failed!", strlen(hex_to_binary("123456")) == 24);
	polynom p = cipher_block_chaining_enc(hex_to_binary("123456"),
			hex_to_binary("479399"), hex_to_binary("732904"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("0070F4")));
	p = cipher_block_chaining_enc(hex_to_binary("123456a"),
			hex_to_binary("7C89A6"), hex_to_binary("AC6B46"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("BCEC1CD46DE1")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aa"),
			hex_to_binary("985854"), hex_to_binary("2B9BD3"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("EE815ACB6E8D")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaa"),
			hex_to_binary("977D38"), hex_to_binary("AB614F"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("7901FB1BA6D4")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaa"),
			hex_to_binary("6AC642"), hex_to_binary("F8275D"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("FA71CEDF5E76")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaaa"),
			hex_to_binary("0EF91F"), hex_to_binary("98846D"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("BC9320EB5504")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaaaa"),
			hex_to_binary("42551B"), hex_to_binary("77C989"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("C797C44F2FDB")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaaaaa"),
			hex_to_binary("5E3E14"), hex_to_binary("C1196C"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("44C372B882B04971C0")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaaaaaa"),
			hex_to_binary("1A0F5F"), hex_to_binary("C6B107"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("F5980C958B84516B86")));
	p = cipher_block_chaining_enc(hex_to_binary("123456aaaaaaaaa"),
			hex_to_binary("95DDB3"), hex_to_binary("ABD6FE"));
	mu_assert("Cipher block chaining is not working!",
			equals(p, hex_to_binary("C0D43E7B7962E87347")));
	return 0;
}

static char * test_cipher_block_chaining_dec() {
	polynom p = cipher_block_chaining_dec(hex_to_binary("C0D43E7B7962E87347"),
			hex_to_binary("95DDB3"), hex_to_binary("ABD6FE"));
	mu_assert("Cipher block chaining decryption is not working!",
			equals(p, hex_to_binary("123456aaaaaaaaa000")));
	return 0;
}

static char * test_maj5() {
	char * res = MAJ5(hex_to_binary("48C4A2E691D5B3F7"), 228);
	mu_assert("MAJ5 is not working!",
			strcmp(binary_to_hex(res), "2E4ED25CBB2DDF55B8277286A7D07EB160C252B6B936DEAEC95C3C2CC") == 0);
	return 0;
}

static char * test_all5() {
	char * res = ALL5(hex_to_binary("48C4A2E691D5B3F7"), 228);
	mu_assert("ALL5 is not working!",
			strcmp(binary_to_hex(res), "291203071AA0318E45A288A297684B2008A1421A3A1D1811029E20972") == 0);
	return 0;
}

static char * test_sponge() {
	char * res = SPONGEBUNNY("1011001100");
	mu_assert("SPONGEBUNNY is not working!",
			strcmp(res, "0011100000111100000001001100010011001110010001110111100101000101111001111001000010001001000011011000111001110010011101111111101001101000111100010101101110100011") == 0);
	res = SPONGEBUNNY("01000111000011000011100111001111100110101111110011");
	mu_assert("SPONGEBUNNY is not working!",
			strcmp(res, "0110001110000111111100011011111010110101111011011011000000001101000001100100100001111110010100100100001110000100001101000110011010110101011000001000010001100100") == 0);
	int i = 0;
	polynom p = initialize("1000101010100100100010110000100111111100101011000001100101010110100111100101101000110001100010110010111100100001101101001100111000101010001101010111100110101100000010010101001010111110111001111001011000101101011011010101110111101001000010111010100011101111111110111000100101100111101000110111001001001001110011110000111111010101010000101110000110101111111111000101001111111011000001101101111110111110101110110010000100001110011111100000100101110001111000011011101010010110000101010010111010011110000110101000101000000001");
	polynom r;
	r = sponge_poly(p, 20);
//	print_binary("",r);
	mu_assert("sponge_poly not working", equals(r, "11010011000010001100"));
	return 0;
}

static char * test_a51() {
	char * res = a51(hex_to_binary("48C4A2E691D5B3F7"), 228);
	mu_assert("A51 is not working!",
			strcmp(binary_to_hex(res), "A9A7552C17F40A8D5B70C2AD3946049FA6B46BABF6CA4DA65F20DBE35") == 0);
	return 0;
}

static char *test_prime_gen() {
	initialize_rand(38362178);
	BIGNUM *prime_no = get_long_prime_number(512);
	BIO *out=BIO_new(BIO_s_file());
	BIO_set_fp(out,stdout,BIO_NOCLOSE);
	mu_assert("CSPRNG not working", strcmp(BN_bn2dec(prime_no), "2965176724656851839220370116884140168375810923886655990419267657610300066314101848643233278138166935770389908952050642963808793902904855433310166587886791") == 0);
	return 0;
}

static char *test_transformations() {
	polynom p;
	p = initialize("00011011");
	BIGNUM *n = transform_to_bignum(p);
	mu_assert("Transformation poly -> BN not working", strcmp(BN_bn2dec(n), "27") == 0);
	polynom poly = transform_to_poly(n);
	mu_assert("Transformation BN -> poly not working", poly.size != 0);
	mu_assert("Transformation BN -> poly not working", equals(poly,"00011011"));
	return 0;
}

static char *test_rsa() {
	char *MSG =  "BAC03F";
	BIGNUM *n = BN_new();
	BN_hex2bn(&n, "1327D4FEA8411577878917E8C99180156C4450B7ACD55B50D96E356D20398C133BD032B32782AE38F629CDB8409ECB80421DCD5A90A2824DB3A2D68644F242827F162B85905B8FC93FF7AF4BF7BBCA5CFD9A492E07069534773C003FC04DCED88D82E84D8617219BB683DF1D9A0AF368226831A3F7E23C6C9DA68705ECDE2C47");
	BIGNUM *e = BN_new();
	BN_hex2bn(&e, "07");
	BIGNUM *d = BN_new();
	BN_hex2bn(&d, "0AF2309184B779FB28E09FF2BC53249E8702774462C30F9BE9F5D562EDD7BDC1D909418AF20188208CAA2C69497F4FB7013599EA9BCA939A1D819F282765DCDC890F4981DB462E75E3A560B283BD3C2C9A2727AB08AD64F1325BD979D8FBB826A7058215FE1C781E46C2909793AA746FB54844E660FCB056C3A40F9D02D421B7");
	BIGNUM *msg = BN_new();
	BN_hex2bn(&msg, MSG);
	BIGNUM *c = enc_dec(msg, e, n);
	BIGNUM *dec = enc_dec(c, d, n);
	mu_assert("RSA Encryption/Decryption not working", strcmp(MSG, BN_bn2hex(dec)) == 0);
	return 0;
}
static char * all_tests() {
	mu_run_test(test_addition);
	mu_run_test(test_mult);
	mu_run_test(test_keys);
	mu_run_test(test_mixing_layer);
	mu_run_test(test_bunny_int);
	mu_run_test(test_split_concat);
	mu_run_test(test_inv_sboxs);
	mu_run_test(test_dec_bunny_int);
	mu_run_test(test_lsfr);
	mu_run_test(hex_to_bin_test);
	mu_run_test(test_cipher_block_chaining);
	mu_run_test(test_cipher_block_chaining_dec);
	mu_run_test(test_maj5);
	mu_run_test(test_all5);
	mu_run_test(test_sponge);
	mu_run_test(test_a51);
	mu_run_test(test_transformations);
	mu_run_test(test_prime_gen);
	mu_run_test(test_rsa);
	return 0;
}

int main(int argc, char ** argv) {
	primitive_p = initialize("1011011");
	char *result = all_tests();
	if (result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\nTests failed: %d\n", tests_run, failed);
	return result != 0;
}
