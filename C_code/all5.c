#include "maj5.c"

int KEY_SIZE = 64;

int T1 = 9;
int T2 = 11;
int T3 = 11;
int T4 = 5;
int T5 = 7;

char FRAME_LENGTH = 22;
char STEP4_IGNORED_CYCLES = 100;

/**
 *  INPUT:  k (key of 64 bits)
 *          n (length of the output stream)
 *  OUTPUT: s (stream of n bits)
 *
 */
char * ALL5(char *key, int n) {
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
	for (i = 0; i < FRAME_LENGTH; i++) {
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

	/* STEP 5 */
	char *result = (char *)malloc(n * sizeof(char));
	for (i = 0; i < n; i++) {
		char x1 = register1[p1.size - 2];
		char x2 = register2[p2.size - 2];
		char x3 = register3[p3.size - 2];
		char x4 = register4[p4.size - 2];
		char x5 = register5[p5.size - 2];

		result[i] = (x1 * x4 + x2 * x3 + x2 * x5 + x3 * x4) % 2;

		shift_lsfr(p1.p, p1.size, register1, p1.size - 1);
		shift_lsfr(p2.p, p2.size, register2, p2.size - 1);
		shift_lsfr(p3.p, p3.size, register3, p3.size - 1);
		shift_lsfr(p4.p, p4.size, register4, p4.size - 1);
		shift_lsfr(p5.p, p5.size, register5, p5.size - 1);

	}

//		print_register(register1, p1.size - 1);
//		print_register(register2, p2.size - 1);
//		print_register(register3, p3.size - 1);
//		print_register(register4, p4.size - 1);
//		print_register(register5, p5.size - 1);
	free(register1);
	free(register2);
	free(register3);
	free(register4);
	free(register5);
	return transform_to_str(result, n);;
}
