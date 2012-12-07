#include "field.h"
#include "maj5.h"
#include <string.h>
#include <stdlib.h>
#include "lsfr.h"

int K_SIZE = 64;

int TAP1 = 9;
int TAP2 = 11;
int TAP3 = 11;
int TAP4 = 5;
int TAP5 = 7;

char FR_LEN = 22;
char IGNORED_CYCLES = 100;

/**
 *  INPUT:  k (key of 64 bits)
 *          n (length of the output stream)
 *  OUTPUT: s (stream of n bits)
 *
 */
char * MAJ5(char *key, int n) {
	if (strlen(key) != K_SIZE) {
		printf("Bad key size!");
		return 0;
	}
	polynom p1 = initialize("11100100000000000001");
	polynom p2 = initialize("11000000000000000000001");
	polynom p3 = initialize("111000000000000100000001");
	polynom p4 = initialize("100000000101");
	polynom p5 = initialize("10000000011011");
	char f[] = {0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	/* STEP 1 */
	int i = 0;
	char *register1 = (char *)malloc((p1.size - 1) * sizeof(char));
	for (i = 0; i < p1.size-1; i++) {
		register1[i] = 0;
	}
	char *register2 = (char *)malloc((p2.size - 1) * sizeof(char));
	for (i = 0; i < p2.size-1; i++) {
		register2[i] = 0;
	}
	char *register3 = (char *)malloc((p3.size - 1) * sizeof(char));
	for (i = 0; i < p3.size-1; i++) {
		register3[i] = 0;
	}
	char *register4 = (char *)malloc((p4.size - 1) * sizeof(char));
	for (i = 0; i < p4.size-1; i++) {
		register4[i] = 0;
	}
	char *register5 = (char *)malloc((p5.size - 1) * sizeof(char));
	for (i = 0; i < p5.size-1; i++) {
		register5[i] = 0;
	}

	/* STEP 2 */
	for (i = 0; i < strlen(key); i++) {
		char key_byte = key[i] - '0';
		shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		shift_lsfr(p5.p, p5.size, register5, p5.size - 1);
		register1[0] = (register1[0] + key_byte) % 2;
		register2[0] = (register2[0] + key_byte) % 2;
		register3[0] = (register3[0] + key_byte) % 2;
		register4[0] = (register4[0] + key_byte) % 2;
		register5[0] = (register5[0] + key_byte) % 2;
	}

	/* STEP 3 */
	for (i = 0; i < FR_LEN; i++) {
			shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
			shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
			shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
			shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
			shift_lsfr(p5.p, p5.size, register5, p5.size - 1);
			register1[0] = (register1[0] + f[i]) % 2;
			register2[0] = (register2[0] + f[i]) % 2;
			register3[0] = (register3[0] + f[i]) % 2;
			register4[0] = (register4[0] + f[i]) % 2;
			register5[0] = (register5[0] + f[i]) % 2;
		}

	/* STEP 4 */
	for (i = 0; i < IGNORED_CYCLES; i++) {
		char maj_bit = get_majority_bit(register1[TAP1 - 1], register2[TAP2 - 1], register3[TAP3 - 1], register4[TAP4 - 1], register5[TAP5 - 1]);
		if (maj_bit == register1[TAP1 - 1])
			shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		if (maj_bit == register2[TAP2 - 1])
			shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		if (maj_bit == register3[TAP3 - 1])
			shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		if (maj_bit == register4[TAP4 - 1])
			shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		if (maj_bit == register5[TAP5 - 1])
			shift_lsfr(p5.p, p5.size, register5, p5.size - 1);
	}
//
//	print_register(register1, p1.size - 1);
//	print_register(register2, p2.size - 1);
//	print_register(register3, p3.size - 1);
//	print_register(register4, p4.size - 1);
//	print_register(register5, p5.size - 1);

	/* STEP 5 */
	char *result = (char *)malloc(n * sizeof(char));
	for (i = 0; i < n; i++) {
		result[i] = (register1[p1.size - 2]+register2[p2.size - 2]+register3[p3.size - 2]+register4[p4.size - 2]+register5[p5.size - 2]) % 2;

		char maj_bit = get_majority_bit(register1[TAP1 - 1], register2[TAP2 - 1], register3[TAP3 - 1], register4[TAP4 - 1], register5[TAP5 - 1]);
		if (maj_bit == register1[TAP1 - 1])
			shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		if (maj_bit == register2[TAP2 - 1])
			shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		if (maj_bit == register3[TAP3 - 1])
			shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		if (maj_bit == register4[TAP4 - 1])
			shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		if (maj_bit == register5[TAP5 - 1])
			shift_lsfr(p5.p, p5.size, register5, p5.size - 1);
	}
	free(register1);
	free(register2);
	free(register3);
	free(register4);
	free(register5);
	return transform_to_str(result, n);;
}

/* Assumes strlen a == strlen b*/
char * xor_str(char *a, char *b) {
	char *res = malloc((strlen(a) + 1) * sizeof(char));
	int i;
	for (i = 0; i < strlen(a); i++) {
		res[i] = ((a[i]-'0') ^ (b[i] - '0')) + '0';
	}
	res[strlen(a)] = 0;
	return res;
}

char * MAJ5ENC(char *key, char* message) {
	char *keystream = MAJ5(key, strlen(message));
	printf("KS=%s-\n", keystream);
	return xor_str(keystream, message);
}

char * MAJ5DEC(char *key, char* ciphertext) {
	char *keystream = MAJ5(key, strlen(ciphertext));
	printf("KS=%s-\n", keystream);
	printf("MSG=%s-\n", ciphertext);
	return xor_str(keystream, ciphertext);
}
