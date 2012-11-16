#include <stdio.h>
#include <stdlib.h>
#include "lsfr.h"

char * transform(char *x) {
	int l = strlen(x);
	char *output = (char *) malloc(l * sizeof(char));
	int i;
	for (i = 0; i < l; i++) {
		output[i] = x[i] - '0';
	}
	return output;
}

char * transform_to_str(char *x, int l) {
	char *output = (char *) malloc((l + 1) * sizeof(char));
	int i;
	for (i = 0; i < l; i++) {
		output[i] = x[i] + '0';
	}
	output[l] = 0;
	return output;
}

/**
 * Shift one position the LSFR and return the output bit. The result will be stored in
 * the state array. The polynom and the state array must contain only 0 and 1.
 * Be careful, not '0' and '1' (as chars) but 0 and 1 as values!!!
 */
char shift_lsfr(char *polynom, int poly_size, char state[], int state_size ) {
	char output_bit = 0;
	int j = 0;
	for (j = 1; j < poly_size; j++) {
		if (polynom[j]) {
			output_bit = (output_bit + state[j - 1]) % 2;
		}
	}
	for (j = state_size - 1; j > 0 ; j--) {
		state[j] = state[j - 1];
	}
	state[0] = output_bit;
	return output_bit;
}

/**
 * Shift the state_vector to the left and insert on the first position
 * the xored bits depending on the tap position. The input and output
 * arrays are string ones therefore they must contain only '0' or '1' chars.
 */
char* lsfr(char *polynom, char *initial_state, int output_length) {
	int poly_size = strlen(polynom);
	int state_size = strlen(initial_state);
	polynom = transform(polynom);
	char *state = transform(initial_state);
	if (state_size + 1 < poly_size) {
		return 0;
	}
	int i = 0;
	char *output = (char *) malloc(output_length * sizeof(char));
	for (i = 0; i < output_length; i++) {
		output[i] = shift_lsfr(polynom, poly_size, state, state_size);
	}

	return transform_to_str(output, output_length);
}

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
