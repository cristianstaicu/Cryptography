#include <stdio.h>
#include <stdlib.h>

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
 * Shift the state_vector to the left and insert on the first position
 * the xored bits depending on the tap position
 */
char* lsfr(char *polynom, char *initial_state, int output_length) {
	int poly_size = strlen(polynom);
	int state_size = strlen(initial_state);
	polynom = transform(polynom);
	initial_state = transform(initial_state);
	if (state_size + 1 < poly_size) {
		return 0;
	}
	int i = 0;
	char *output = (char *) malloc(output_length * sizeof(char));
	char *state = initial_state;
	for (i = 0; i < output_length; i++) {
		output[i] = 0;
		int j = 0;
		for (j = 1; j < poly_size; j++) {
			if (polynom[j]) {
				output[i] = (output[i] + state[j - 1]) % 2;
			}
		}
		char *next_state = (char *) malloc(state_size * sizeof(char));
		for (j = 1; j < state_size; j++) {
			next_state[j] = state[j - 1];
		}
		next_state[0] = output[i];
		state = next_state;
	}

	return transform_to_str(output, output_length);
}
