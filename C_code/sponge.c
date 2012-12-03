#include "sponge.h"
#include "lsfr.h"
#include <stdlib.h>

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

char* sponge(polynom message, int digest_size) {
	char *output = (char *)malloc(digest_size * sizeof(char));
	polynom padded = padding(message);
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
		output[j] = state.p[STATE_LENGTH - j - 1];
	}
	int output_block = 1;

	for (i = 0; i < digest_size/BITRATE_LENGTH - 1; i++) {
		state = BunnyTn(state, key);
		/* OUTPUT I-TH CHUNK */
		for (j = 0; j < BITRATE_LENGTH; j++) {
			output[output_block * BITRATE_LENGTH + j] = state.p[STATE_LENGTH - j - 1];
		}
		output_block++;
	}
	return output;
}

polynom sponge_poly(polynom message, int digest_size) {
	int dif = digest_size % BITRATE_LENGTH;
	int extended = digest_size;
	if ( dif != 0 ) {
		extended = extended + BITRATE_LENGTH - dif;
	}
//	printf(" %d\n", extended);
	char *out = sponge(message, extended);
//	printf("=====================\n");
//	int j;
//	for (j = 0; j < extended; j++) {
//		printf("%d", out[j]);
//	}
//	printf("\n");
//	printf("%s %d %d \n", transform_to_str(out, extended), extended, digest_size);
	polynom res;
	res.size = digest_size;
	res.p = (char *) malloc(res.size * sizeof(char));
	int i = 0;
	for (i = 0; i < res.size; i++) {
		res.p[i] = out[i];
	}
	return res;
}

char* SPONGEBUNNY(char *message) {
	polynom m = initialize(message);
	char* out =  sponge(m, DIGEST_SIZE);
	return transform_to_str(out, DIGEST_SIZE);
}
