#include "bunny.c"

static const int BITRATE_LENGTH = 20;

polynom padding(polynom m) {
	char diff = BITRATE_LENGTH - (m.size % BITRATE_LENGTH);
	if (diff != BITRATE_LENGTH) {
		return shift_left(m, diff);
	}
	return m;
}

char* SPONGEBUNNY(char *message) {
	polynom m = initialize(message);
	polynom padded = padding(m);
	int no_pieces = padded.size / BITRATE_LENGTH;
	polynom pieces = split(padded, no_pieces);
	return 0;
}
