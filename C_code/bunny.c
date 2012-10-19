#include "field.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

polynom primitive_p;

int POLY_SIZE = 6;

int SBOX_1[] = {0, 1, 45, 54, 59, 18, 27, 30, 48, 10, 9, 49, 32, 62, 15, 14, 24, 51, 5, 58, 41, 56, 53, 35, 16, 50, 31, 6, 42, 38, 7, 26, 12, 63, 52, 23, 47, 61, 29, 43, 57, 20, 28, 39, 55, 2, 60, 36, 8, 11, 25, 17, 34, 22, 3, 44, 21, 40, 19, 4, 46, 37, 13, 33};
int SBOX_2[] = {0, 1, 32, 51, 49, 3, 63, 31, 36, 4, 59, 9, 62, 45, 15, 14, 7, 5, 54, 38, 8, 57, 23, 52, 30, 61, 16, 33, 58, 42, 26, 24, 13, 43, 22, 34, 41, 60, 28, 27, 55, 48, 19, 6, 56, 12, 50, 20, 47, 10, 37, 18, 53, 35, 17, 21, 40, 44, 29, 11, 25, 46, 2, 39};
int SBOX_3[] = {0, 1, 38, 54, 37, 18, 43, 13, 20, 50, 25, 46, 42, 58, 15, 14, 32, 51, 5, 7, 47, 10, 34, 22, 12, 56, 2, 39, 24, 26, 62, 45, 28, 27, 35, 53, 8, 57, 31, 63, 4, 36, 16, 33, 11, 29, 55, 48, 41, 60, 21, 17, 23, 52, 3, 49, 9, 59, 30, 61, 44, 40, 19, 6};
int SBOX_4[] = {4, 5, 41, 50, 63, 22, 31, 26, 52, 14, 13, 53, 36, 58, 11, 10, 28, 55, 1, 62, 45, 60, 49, 39, 20, 54, 27, 2, 46, 34, 3, 30, 8, 59, 48, 19, 43, 57, 25, 47, 61, 16, 24, 35, 51, 6, 56, 32, 12, 15, 29, 21, 38, 18, 7, 40, 17, 44, 23, 0, 42, 33, 9, 37};

polynom INV_Sbox(polynom x, int* S_BOX) {
	int a = get_int_equivalent(x);
	int i = 0;
	int b;
	for (i = 0; i < 64; i++) {
		if (S_BOX[i] == a) {
			b = i;
			break;
		}
	}
	return get_poly_equivalent(b, POLY_SIZE);
}

polynom INV_Sbox1(polynom x) {
	return INV_Sbox(x, SBOX_1);
}

polynom INV_Sbox2(polynom x) {
	return INV_Sbox(x, SBOX_2);
}

polynom INV_Sbox3(polynom x) {
	return INV_Sbox(x, SBOX_3);
}

polynom INV_Sbox4(polynom x) {
	return INV_Sbox(x, SBOX_4);
}

polynom Sbox1(polynom x) {
	int a = get_int_equivalent(x);
	int b = SBOX_1[a];
	return get_poly_equivalent(b, POLY_SIZE);
}

polynom Sbox2(polynom x) {
	int a = get_int_equivalent(x);
	int b = SBOX_2[a];
	return get_poly_equivalent(b, POLY_SIZE);
}

polynom Sbox3(polynom x) {
	int a = get_int_equivalent(x);
	int b = SBOX_3[a];
	return get_poly_equivalent(b, POLY_SIZE);
}

polynom Sbox4(polynom x) {
	int a = get_int_equivalent(x);
	int b = SBOX_4[a];
	return get_poly_equivalent(b, POLY_SIZE);
}

/* The argument should be an array of polynoms of length 4. Each being in E! */
polynom* MixingLayer(polynom *x) {
	polynom ml11 = initialize("100011");
	polynom ml12 = initialize("111011");
	polynom ml13 = initialize("111000");
	polynom ml14 = initialize("111101");
	polynom ml21 = initialize("001101");
	polynom ml22 = initialize("111100");
	polynom ml23 = initialize("010110");
	polynom ml24 = initialize("011000");
	polynom ml31 = initialize("000011");
	polynom ml32 = initialize("100000");
	polynom ml33 = initialize("010111");
	polynom ml34 = initialize("110111");
	polynom ml41 = initialize("101100");
	polynom ml42 = initialize("100110");
	polynom ml43 = initialize("111000");
	polynom ml44 = initialize("010011");

	polynom mixing_layer[4][4]={
		{ml11, ml12, ml13, ml14},
		{ml21, ml22, ml23, ml24},
		{ml31, ml32, ml33, ml34},
		{ml41, ml42, ml43, ml44}
	};
	int i = 0, j = 0;
	polynom *r = (polynom *)malloc(4 * sizeof(polynom));
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			polynom aux = mult(x[j], mixing_layer[j][i], primitive_p);
			if (j == 0) {
				r[i] = aux;
			} else {
				r[i] = add(r[i], aux);
 			}
		}
	}

	return r;
}

polynom* split(polynom p, char pieces) {
	polynom *res = (polynom *)malloc(pieces*sizeof(polynom));
	int i = 0;
	int l = p.size / pieces;
	for (i = 0; i < pieces; i++) {
		int index = l * i;
		int pos = pieces - i - 1;
		res[pos].p = (char *) malloc (l * sizeof(char));
		memcpy(res[pos].p, p.p + index, l);
		res[pos].size = l;
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
		for (j = 0; j < pieces[no - i - 1].size; j++) {
			res.p[count] = pieces[no - i - 1].p[j];
			count++;
		}
	}
	return res;
}

polynom* compute_keys(polynom key) {
	/* key_w[0] is not used for consistency with the slides! 4 pieces
	 * of key_w for each of the 15 key round */

	polynom *res = (polynom *)malloc(16*sizeof(polynom));
	polynom key_w[89];
	polynom *key_sp;
	key_sp = split(key, 4);
	int i = 0;
	polynom k_pol;
	char k_pol_init[6] = {0, 1, 0, 1, 0, 1};
	k_pol.p = k_pol_init;
	k_pol.size = 6;
	for (i = 1; i <= 4; i++) {
		key_w[i] = key_sp[i - 1];
	}
	for (i = 5; i <= 7; i++) {
		if (i == 5)
			key_w[i] = add(Sbox1(key_w[i - 4]), key_w[i-3]);
		else if (i == 6)
			key_w[i] = add(Sbox2(key_w[i - 4]), key_w[i-3]);
		else if (i == 7)
			key_w[i] = add(Sbox3(key_w[i - 4]), key_w[i-3]);
	}
	key_w[8] = add(Sbox4(key_w[4]), key_w[1]);
	/* Compute values for W array */
	for (i = 9; i <= 88; i++) {
		if (i % 4 != 1) {
			key_w[i] = add(key_w[i - 8], key_w[i - 1]);
		} else if (i % 8 == 1) {
			key_w[i] = add(key_w[i - 8], Sbox2(rotate(key_w[i - 1])));
			key_w[i] = add(key_w[i], k_pol);
		} else if (i % 8 == 5) {
			key_w[i] = add(key_w[i - 8], Sbox3(key_w[i - 1]));
		}
	}
	/* Construct round key */
	for (i = 0; i < 16; i++) {
		polynom key_p[4];
		int j, ik = 0;
		ik = i % 5 + 9 + (i/5) * 20;
		for (j = 0; j <= 4; ik += 5, j++) {
			key_p[j] = key_w[ik];
		}
		polynom k = concat(key_p, 4);
		res[i] = k;
	}
	return res;
}

polynom BunnyTn(polynom m, polynom k) {
	polynom *keys = compute_keys(k);
	polynom whitened = add(m, keys[0]);
	int round = 0;
	polynom partial_result = whitened;
	do {
		round++;
		polynom *msg_split = split(partial_result, 4);
		msg_split[0] = Sbox1(msg_split[0]);
		msg_split[1] = Sbox2(msg_split[1]);
		msg_split[2] = Sbox3(msg_split[2]);
		msg_split[3] = Sbox4(msg_split[3]);
		polynom * msg_mixed = MixingLayer(msg_split);
		polynom concated = concat(msg_mixed, 4);
		polynom round_key = keys[round];
		partial_result = add(concated, round_key);
	} while (round < 15);
	return partial_result;
}

//int main(int argc, char ** argv) {
//
//	primitive_p = initialize("1011011");
////	polynom message = initialize("111011100111110111111101");
//	polynom message = initialize("001001110101100000111100");
////	polynom key = initialize("011111010100111001001100");
//	polynom key = initialize("111101001101010011010000");
//
////	print_binary("K", key);
////	print_binary("M", message);
//	polynom res = BunnyTn(message, key);
//	print_binary("C", res);
//
//	return 0;
//}
