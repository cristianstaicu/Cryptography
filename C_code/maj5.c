#include "field.h"
#include "maj5.h"
#include <string.h>
#include <stdlib.h>
#include "lsfr.c"

const int KEY_SIZE = 64;

const int T1 = 9;
const int T2 = 11;
const int T3 = 11;
const int T4 = 5;
const int T5 = 7;

const char FRAME_LEN = 22;
const char STEP4_IGNORED_CYCLES = 100;

void print_register(char *reg, int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("%d", reg[i]);
	}
	printf("\n");
}

char get_majority_bit(char b1, char b2, char b3, char b4, char b5) {
	char sum = b1 + b2 + b3 + b4 + b5;
	if (sum > 2)
		return 1;
	return 0;
}

/**
 *  INPUT:  k (key of 64 bits)
 *          n (length of the output stream)
 *  OUTPUT: s (stream of n bits)
 *
 */
char * MAJ5(char *key, int n) {
	if (strlen(key) != KEY_SIZE) {
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
	char *register1 = (char *)malloc((p1.size - 1) * sizeof(char));
	char *register2 = (char *)malloc((p2.size - 1) * sizeof(char));
	char *register3 = (char *)malloc((p3.size - 1) * sizeof(char));
	char *register4 = (char *)malloc((p4.size - 1) * sizeof(char));
	char *register5 = (char *)malloc((p5.size - 1) * sizeof(char));

	/* STEP 2 */
	int i = 0;
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
	for (i = 0; i < FRAME_LEN; i++) {
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
	for (i = 0; i < STEP4_IGNORED_CYCLES; i++) {
		char maj_bit = get_majority_bit(register1[T1 - 1], register2[T2 - 1], register3[T3 - 1], register4[T4 - 1], register5[T5 - 1]);
		if (maj_bit == register1[T1 - 1])
			shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		if (maj_bit == register2[T2 - 1])
			shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		if (maj_bit == register3[T3 - 1])
			shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		if (maj_bit == register4[T4 - 1])
			shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		if (maj_bit == register5[T5 - 1])
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

		char maj_bit = get_majority_bit(register1[T1 - 1], register2[T2 - 1], register3[T3 - 1], register4[T4 - 1], register5[T5 - 1]);
		if (maj_bit == register1[T1 - 1])
			shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		if (maj_bit == register2[T2 - 1])
			shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		if (maj_bit == register3[T3 - 1])
			shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		if (maj_bit == register4[T4 - 1])
			shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		if (maj_bit == register5[T5 - 1])
			shift_lsfr(p5.p, p5.size, register5, p5.size - 1);
	}
	return transform_to_str(result, n);;
}
