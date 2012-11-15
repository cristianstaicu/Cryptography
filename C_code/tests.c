#include <stdio.h>
//#include "field.h"
#include "mintest.h"
#include "bunny.c"
#include "lsfr.c"

int tests_run = 0;
int failed = 0;

/* Wikipedia example */
static char * test_addition() {
	polynom a = initialize("01010011");
	polynom b = initialize("11001010");
	polynom c = add(a, b);
	mu_assert("Addition is not working in finite field arithmetic!", equals(c, "10011001"));
	return 0;
}

/* Rindjael wiki example: 53 x CA*/
static char * test_mult() {
	polynom a = initialize("11001010");
	polynom b = initialize("01010011");
	polynom p = initialize("100011011");
	polynom c = mult(a, b, p);
	mu_assert("Multiplication is not working in finite field arithmetic!", equals(c, "00000001"));
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
	polynom *e = (polynom *)malloc(4 * sizeof(polynom));
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
	polynom c = BunnyTn(m,k);
	mu_assert("You've got yourself a sad bunny! :(", equals(c,"111110010000010001100110"));

	m = initialize("011100101101101011011100");
	k = initialize("100010100011001110011110");
	c = BunnyTn(m,k);
	mu_assert("You've got yourself a sad bunny(2)! :(", equals(c,"101111110000110000101011"));

	return 0;
}

static char * test_split_concat() {
	polynom m = initialize("001001110101100000111100");
	polynom *splitm = split(m, 4);
	mu_assert("Split performed incorrect!", equals(splitm[0], "001001"));
	m = concat(splitm, 4);
	mu_assert("Concat performed incorrect!", equals(m, "001001110101100000111100"));
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
	polynom m = DecBunnyTn(c,k);
	mu_assert("You've got yourself a sad bunny -> bad decryption! :(", equals(m,"001001110101100000111100"));
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

const char* quads[] = {"0000", "0001", "0010", "0011", "0100", "0101",
                     "0110", "0111", "1000", "1001", "1010", "1011",
                     "1100", "1101", "1110", "1111"};

const char * hex_to_bin_quad(unsigned char c)
{
  if (c >= '0' && c <= '9') return quads[     c - '0'];
  if (c >= 'A' && c <= 'F') return quads[10 + c - 'A'];
  if (c >= 'a' && c <= 'f') return quads[10 + c - 'a'];
  return -1;
}

char * hex_to_binary(char* hex_array) {
	int len = strlen(hex_array);
	char *res = (char*)malloc((len*4+1)*sizeof(char));
	int i = 0;
	for (i = 0; i < len; i++ ) {
		char *quad = hex_to_bin_quad(hex_array[i]);
		int j;
		for (j = 0; j < 4; j++) {
			res[i * 4 + j] = quad[j];
		}
	}
	res[len*4 + 1] = 0;
	return res;
}

char * binary_to_hex(char * bin_array) {
	int len = strlen(bin_array);
	int i,j = 0;
	char *res = (char*)malloc((len/4 + 1) * sizeof(char));
	for (i = 0; i < len; i+=4) {
		int n = (bin_array[i]-'0')*8 + (bin_array[i + 1]-'0') * 4 + (bin_array[i + 2]-'0') * 2 + (bin_array[i + 3]-'0');
		if (n < 10) {
			res[j++] = '0' + n;
		} else {
			res[j++] = 'A' + (n - 10);
		}
	}
	res[len/4] = 0;
	return res;
}

static char * hex_to_bin_test() {
	char *res = hex_to_binary("faC0");
	mu_assert("Hex to bin conversion is not working!", strlen(res) == 16);
	mu_assert("Hex to bin conversion is not working!", strcmp(res, "1111101011000000") == 0);
	res = binary_to_hex(res);
	mu_assert("Bin to hex conversion is not working!", strcmp(res, "FAC0") == 0);
	return 0;
}

static char * test_cipher_block_chaining() {
	mu_assert("Hex to bin failed!", strlen(hex_to_binary("123456")) == 24);
	polynom p = cipher_block_chaining(hex_to_binary("123456"), hex_to_binary("479399"), hex_to_binary("732904"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("0070F4")));
	p = cipher_block_chaining(hex_to_binary("123456a"), hex_to_binary("7C89A6"), hex_to_binary("AC6B46"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("BCEC1CD46DE1")));
	p = cipher_block_chaining(hex_to_binary("123456aa"), hex_to_binary("985854"), hex_to_binary("2B9BD3"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("EE815ACB6E8D")));
	p = cipher_block_chaining(hex_to_binary("123456aaa"), hex_to_binary("977D38"), hex_to_binary("AB614F"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("7901FB1BA6D4")));
	p = cipher_block_chaining(hex_to_binary("123456aaaa"), hex_to_binary("6AC642"), hex_to_binary("F8275D"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("FA71CEDF5E76")));
	p = cipher_block_chaining(hex_to_binary("123456aaaaa"), hex_to_binary("0EF91F"), hex_to_binary("98846D"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("BC9320EB5504")));
	p = cipher_block_chaining(hex_to_binary("123456aaaaaa"), hex_to_binary("42551B"), hex_to_binary("77C989"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("C797C44F2FDB")));
	p = cipher_block_chaining(hex_to_binary("123456aaaaaaa"), hex_to_binary("5E3E14"), hex_to_binary("C1196C"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("44C372B882B04971C0")));
	p = cipher_block_chaining(hex_to_binary("123456aaaaaaaa"), hex_to_binary("1A0F5F"), hex_to_binary("C6B107"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("F5980C958B84516B86")));
	p = cipher_block_chaining(hex_to_binary("123456aaaaaaaaa"), hex_to_binary("95DDB3"), hex_to_binary("ABD6FE"));
	mu_assert("Cipher block chaining is not working!", equals(p, hex_to_binary("C0D43E7B7962E87347")));
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
