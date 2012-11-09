#include <stdio.h>
#include <stdlib.h>

char * transform(char *x) {
	int l = strlen(x);
	int i;
	for (i = 0; i < l; i++) {
		x[i] -= '0';
	}
	return x;
}

char * transform_to_str(char *x, int l) {
	int i;
	for (i = 0; i < l; i++) {
		x[i] += '0';
	}
	x[l] = 0;
	return x;
}

/**
 * Shift the state_vector to the left and insert on the first position
 * the xored bits depending on the tap position
 */
char* lsfr(char *polynom, char *initial_state, int output_length) {
	int poly_size = strlen(polynom);
	int state_size = strlen(initial_state);
	polynom = transform(polynom);
	polynom = transform(initial_state);
	if (state_size < poly_size) {
		return 0;
	}
	int i = 0;
	char *output = (char *) malloc(output_length * sizeof(char));
	char *state = initial_state;
	for (i = 0; i < output_length; i++) {
		output[i] = 0;
		int j = 0;
		for (j = 0; j < poly_size; j++) {
			if (polynom[j]) {
				output[i] = (output[i] + initial_state[i]) % 2;
			}
		}
		char *next_state = (char *) malloc(poly_size * sizeof(char));
		for (j = 0; i < state_size - 1; j++) {
			next_state[i] = state[j + 1];
		}
		next_state[state_size - 1] = output[j];
	}

	return output;
}
