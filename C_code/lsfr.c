#include <stdio.h>
#include <stdlib.h>

char* lsfr(char *p, char *s, int n) {
	int poly_size = strlen(p);
	int state_size = strlen(s);
	if (state_size < poly_size) {
		return 0;
	}
	int i = 0;
	char *output = (char *)malloc(n * sizeof(char));
	char *state = s;
	for (i = 0; i < n; i++) {
		output[i] = 0;
		int j = 0;
		for (j = 0; j < poly_size; j++) {
			if (p[j]) {
				output[i] = (output[i] + s[i]) % 2;
			}
		}
		char *next_state = (char *)malloc(poly_size * sizeof(char));
		for (j = 0; i < state_size - 1; j++) {
			next_state[i] = state[j + 1];
		}
		next_state[state_size - 1] = output[j];
 	}
	return output;
}
