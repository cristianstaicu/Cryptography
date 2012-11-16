#include "bunny.c"
#include "lsfr.h"

static const int BITRATE_LENGTH = 20;
static const int STATE_LENGTH = 24;
static const int DIGEST_SIZE = 160;

polynom padding(polynom m) {
	char diff = BITRATE_LENGTH - (m.size % BITRATE_LENGTH);
	if (diff != BITRATE_LENGTH) {
		return shift_left(m, diff);
	}
	return m;
}


void print_array(char *reg, int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("%d", reg[i]);
	}
	printf("\n");
}

char* SPONGEBUNNY(char *message) {
	char *output = (char *)malloc(DIGEST_SIZE * sizeof(char));
	polynom m = initialize(message);
	polynom padded = padding(m);
	int no_pieces = padded.size / BITRATE_LENGTH;
	polynom *pieces = split(padded, no_pieces);
	polynom key = initialize("111111111111111111111111");
	polynom state = initialize("000000000000000000000000");
	int i;
	/* Absorbing part */
	for (i = 0; i < no_pieces; i++) {
		/* XOR bitrate section */
		pieces[i] = shift_left(pieces[i], STATE_LENGTH - BITRATE_LENGTH);
		state = add(state, pieces[i]);
		/* apply bunny */
		state = BunnyTn(state, key);
	}
	/* SQUEEZING PART */
	int j = 0;
	/* OUTPUT THE FIRST CHUNK */
	for (j = 0; j < BITRATE_LENGTH; j++) {
		output[j] = state.p[STATE_LENGTH- j - 1];
	}
	int output_block = 1;

	for (i = 0; i < DIGEST_SIZE/BITRATE_LENGTH - 1; i++) {
		state = BunnyTn(state, key);
		/* OUTPUT I-TH CHUNK */
		for (j = 0; j < BITRATE_LENGTH; j++) {
			output[output_block * BITRATE_LENGTH + j] = state.p[STATE_LENGTH - j - 1];
		}
		output_block++;
	}

	return transform_to_str(output, DIGEST_SIZE);
}
