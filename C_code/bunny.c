#include "field.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

polynom primitive_p;

polynom Sbox1(polynom x) {
	int SBOX_1[] = {0, 1, 45, 54, 59, 18, 27, 30, 48, 10, 9, 49, 32, 62, 15, 14, 24, 51, 5, 58, 41, 56, 53, 35, 16, 50, 31, 6, 42, 38, 7, 26, 12, 63, 52, 23, 47, 61, 29, 43, 57, 20, 28, 39, 55, 2, 60, 36, 8, 11, 25, 17, 34, 22, 3, 44, 21, 40, 19, 4, 46, 37, 13, 33};
	int a = get_int_equivalent(x);
	int b = SBOX_1[a];
	return get_poly_equivalent(b);
}

polynom Sbox2(polynom x) {
	int SBOX_2[] = {0, 1, 32, 51, 49, 3, 63, 31, 36, 4, 59, 9, 62, 45, 15, 14, 7, 5, 54, 38, 8, 57, 23, 52, 30, 61, 16, 33, 58, 42, 26, 24, 13, 43, 22, 34, 41, 60, 28, 27, 55, 48, 19, 6, 56, 12, 50, 20, 47, 10, 37, 18, 53, 35, 17, 21, 40, 44, 29, 11, 25, 46, 2, 39};
	int a = get_int_equivalent(x);
	int b = SBOX_2[a];
	return get_poly_equivalent(b);
}

polynom Sbox3(polynom x) {
	int SBOX_3[] = {0, 1, 38, 54, 37, 18, 43, 13, 20, 50, 25, 46, 42, 58, 15, 14, 32, 51, 5, 7, 47, 10, 34, 22, 12, 56, 2, 39, 24, 26, 62, 45, 28, 27, 35, 53, 8, 57, 31, 63, 4, 36, 16, 33, 11, 29, 55, 48, 41, 60, 21, 17, 23, 52, 3, 49, 9, 59, 30, 61, 44, 40, 19, 6};
	int a = get_int_equivalent(x);
	int b = SBOX_3[a];
	return get_poly_equivalent(b);
}

polynom Sbox4(polynom x) {
	int SBOX_4[] = {4, 5, 41, 50, 63, 22, 31, 26, 52, 14, 13, 53, 36, 58, 11, 10, 28, 55, 1, 62, 45, 60, 49, 39, 20, 54, 27, 2, 46, 34, 3, 30, 8, 59, 48, 19, 43, 57, 25, 47, 61, 16, 24, 35, 51, 6, 56, 32, 12, 15, 29, 21, 38, 18, 7, 40, 17, 44, 23, 0, 42, 33, 9, 37};
	int a = get_int_equivalent(x);
	int b = SBOX_4[a];
	return get_poly_equivalent(b);
}

/* The argument should be an array of polynoms of length 4. Each being in E! */
polynom* MixingLayer(polynom *x) {
	char mixing_layer[4][4][6]={
		{{1, 0, 0, 0, 1, 1}, {1, 1, 1, 0, 1, 1}, {1, 1, 1, 0, 0, 0}, {1, 1, 1, 1, 0, 1}},
		{{0, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0}, {0, 1, 0, 1, 1, 0}, {0, 1, 1, 0, 0, 0}},
		{{0, 0, 0, 0, 1, 1}, {1, 0, 0, 0, 0, 0}, {0, 1, 0, 1, 1, 1}, {1, 1, 0, 1, 1, 1}},
		{{1, 0, 1, 1, 0, 0}, {1, 0, 0, 1, 1, 0}, {1, 1, 1, 0, 0, 0}, {0, 1, 0, 0, 1, 1}}
	};

	int i = 0, j = 0;
	polynom *r = (polynom *)malloc(4 * sizeof(polynom));
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			polynom aux = mult_a(x[4-i-1], mixing_layer[i][j], 6, primitive_p);
//			print("A", aux);
			if (j == 0) {
				r[i] = aux;
			} else {
				r[i] = add(r[i], aux);
 			}
		}
	}

//	polynom *r = (polynom *)malloc(4 * sizeof(polynom));
//	r[0] = mult_a(x[3], mixing_layer[0][0], 6, primitive_p);
//	polynom aux = mult_a(x[1], mixing_layer[0][1], 6, primitive_p);
//	r[0] = add(r[2], aux);
//	aux = mult_a(x[2], mixing_layer[0][2], 6, primitive_p);
//	r[0] = add(r[], aux);
//	aux = mult_a(x[3], mixing_layer[0][3], 6, primitive_p);
//	r[0] = add(r[0], aux);
//
//	r[1] = mult_a(x[0], mixing_layer[1][0], 6, primitive_p);
//	aux = mult_a(x[1], mixing_layer[1][1], 6, primitive_p);
//	r[1] = add(r[1], aux);
//	aux = mult_a(x[2], mixing_layer[1][2], 6, primitive_p);
//	r[1] = add(r[1], aux);
//	aux = mult_a(x[3], mixing_layer[1][3], 6, primitive_p);
//	r[1] = add(r[1], aux);
//
//	r[2] = mult_a(x[0], mixing_layer[2][0], 6, primitive_p);
//	aux = mult_a(x[1], mixing_layer[2][1], 6, primitive_p);
//	r[2] = add(r[2], aux);
//	aux = mult_a(x[2], mixing_layer[2][2], 6, primitive_p);
//	r[2] = add(r[2], aux);
//	aux = mult_a(x[3], mixing_layer[2][3], 6, primitive_p);
//	r[2] = add(r[2], aux);
//
//	r[3] = mult_a(x[0], mixing_layer[3][0], 6, primitive_p);
//	aux = mult_a(x[1], mixing_layer[3][1], 6, primitive_p);
//	r[3] = add(r[3], aux);
//	aux = mult_a(x[2], mixing_layer[3][2], 6, primitive_p);
//	r[3] = add(r[3], aux);
//	aux = mult_a(x[3], mixing_layer[3][3], 6, primitive_p);
//	r[3] = add(r[3], aux);

	return r;
}

polynom* split(polynom p, char pieces) {
	polynom *res = (polynom *)malloc(pieces*sizeof(polynom));
	int i = 0;
	int l = p.size / pieces;
	for (i = 0; i < pieces; i++) {
		int index = l*i;
		res[i].p = (char *) malloc (l * sizeof(char));
		memcpy(res[i].p, p.p+index, l);
		res[i].size = l;
	}
	return res;
}

polynom concat(polynom *pieces, char no) {
	int total = 0;
	int i = 0;
	for (i = 0; i < no; i++) {
		total += pieces[i].size;
	}
	polynom res;
	res.p = (char *)malloc(total * sizeof(char));
	res.size = total;
	int count = 0;
	for (i = 0; i < no; i++) {
		int j = 0;
		for (j = 0; j < pieces[i].size; j++) {
			res.p[count] = pieces[i].p[j];
			count++;
		}
	}
	return res;
}

polynom BunnyTn(polynom m, polynom k) {
	polynom whitened = add(m, k);
	print("W", whitened);
	polynom *msg_split = split(whitened, 4);
	msg_split[3] = Sbox1(msg_split[3]);
	msg_split[2] = Sbox2(msg_split[2]);
	msg_split[1] = Sbox3(msg_split[1]);
	msg_split[0] = Sbox4(msg_split[0]);
	polynom * msg_mixed = MixingLayer(msg_split);
	polynom concated = concat(msg_mixed, 4);
	print("C", concated);
}

int main(int argc, char ** argv) {
	char poly_init[7] = { 1, 1, 0, 1, 1, 0, 1};
	primitive_p.p = poly_init;
	primitive_p.size = 7;

	char msg_init[24] = {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1};
	polynom message;
	message.p = msg_init;
	message.size = 24;

	char key_init[24] = {0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0};
	polynom key;
	key.p = key_init;
	key.size = 24;

//	print("K", key);
//	polynom *r = split(key, 4);
//	print("K0", r[0]);
//	print("K1", r[1]);
//	print("K2", r[2]);
//	print("K3", r[3]);
//	polynom c = concat(r, 4);
//	print("KC",c);
	BunnyTn(message, key);

	return 0;
}
