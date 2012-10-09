#include <stdio.h>
#include <stdlib.h>
#include "field.h"

polynom primitive_p;

void copy(char * source, char *dest, char start_pos, char end_pos) {
	int i = 0;
	for (i = start_pos; i < end_pos; i++) {
		dest[i] = source[i];
	}
}

polynom add(polynom a, polynom b) {
	char min_order = 0, max_order = 0;
	polynom res;

	if (a.size > b.size) {
		min_order = b.size;
		max_order = a.size;
		res.p = (char *)malloc(max_order * sizeof(char));
		copy(a.p, res.p, min_order, max_order);
	} else {
		min_order = a.size;
		max_order = b.size;
		res.p = (char *)malloc(max_order * sizeof(char));
		if (min_order != max_order) {
			copy(b.p, res.p, min_order, max_order);
		}
	}
	char i = 0;
	for (i = 0; i < min_order; i++) {
		res.p[i] = (a.p[i] + b.p[i]) % 2;
	}
	res.size = max_order;
	return res;
}

polynom shift_left(polynom a, char pos) {
	if (pos == 0)
		return a;
	polynom res;
	res.size = a.size + pos;
	res.p = (char *)malloc(res.size * sizeof(char));
	int i;
	for (i = 0; i < a.size; i++) {
		res.p[i+pos] = a.p[i];
	}
	return res;
}

polynom rotate(polynom p) {
	polynom res;
	res.size = p.size;
	res.p = (char *)malloc(res.size * sizeof(char));
	int i;
	for (i = 0; i < res.size - 1; i++) {
		res.p[i] = p.p[i+1];
	}
	res.p[res.size - 1] = p.p[0];
	return res;
}

int get_grade(polynom p) {
	int i;
	for (i = p.size; i >= 0; i--) {
		if (p.p[i]) {
			return i + 1;
		}
	}
	return -1;
}

polynom mult_a(polynom a, char b[], int bsize, polynom p) {
	int result_size = p.size - 1;
	polynom res;
	int max_len = a.size + bsize - 1;
	res.p = (char *)malloc((max_len) * sizeof(char));
	int i,j;
	for (i = 0; i < max_len; i++) {
		res.p[i] = 0;
	}
	for (i = 0; i < bsize; i++) {
		if (b[i] != 0) {
			for (j = 0; j < a.size; j++) {
				if (a.p[j] != 0) {
					res.p[i+j] = (res.p[i+j] + 1) % 2;
				}
			}
		}
	}
	res.size = 0;
	for (i = max_len-1; i >= 0; i--) {
		if (res.p[i]) {
			res.size = i + 1;
			break;
		}
	}
	int g;
	while ((g = get_grade(res)) >= p.size) {
		int dif = g - p.size;
		polynom biased = shift_left(p, dif);
		res = add(res, biased);
	}

	res.size = result_size;
	return res;
}

polynom mult(polynom a, polynom b, polynom p) {
	return mult_a(a, b.p, b.size, p);
}

void print_binary(char *p_name, polynom p) {
	printf("%s=", p_name);
	int i = 0;
	for (i = p.size - 1; i>=0; i--) {
		printf("%d", p.p[i]);
	}
	printf("\n");
}

/**
 * Print the polynom given as argument. The first argument p_name
 * is the name of the polynom (strictly for aestetics). If the polynom
 * has 0 for all its elements void set symbol will be printed!
 */
void print(char *p_name, polynom p) {
	printf("%s[X]=",p_name);
	if (p.size == 0) {
		printf("Ø");
		return;
	}
	int i;
	int first = 1;
	for (i=p.size-1; i >= 0; i--) {
		if (p.p[i] == 1) {
			if (!first)
				printf("+");
			if (i > 0)
				printf("X^%d", i);
			else
				printf("1");
			first = 0;
		}
	}
	if (first) {
		printf("Ø");
	}
	printf("\n");
}

int get_int_equivalent(polynom p) {
	int res = 0;
	int i, pow = 1;
	for (i = 0; i < p.size; i++) {
		res += p.p[i] * pow;
		pow <<= 1;
	}
	return res;
}

int relatively_prime(int a, int b) {
	while (a  != 0 && b != 0) {
		if (a>b)
			a = a - b;
		else
			b = b - a;
	}
	if ((a == 0 && b == 1) || (a == 1 && b == 0))
		return 1;
	return 0;
}

polynom get_poly_equiv(int p_int, int n) {
	if (p_int == 0) {
		polynom res;
		res.p = (char *)malloc(n * sizeof(char));
		res.size = n;
		return res;
	} else {
		polynom p = get_poly_equiv(p_int / 2, n + 1);
		p.p[n] = p_int % 2;
		return p;
	}
}

polynom get_poly_equivalent(int p_int) {
	return get_poly_equiv(p_int, 0);
}

int power(int a, int b) {
	int res = 1, i = 0;
	while (i < b) {
		 res *= a;
		 i++;
	}
	return res;
}

polynom get_gen_power(polynom generator, int pow, polynom primitive) {
	polynom res = generator;
	int i = 0;
	for (i = 1; i < pow; i++) {
		res = mult(res, generator, primitive_p);
	}
	return res;
}

void print_poly_binary(polynom p, int ext_size) {
	int i = 0;
	for (i = ext_size-1; i >= 0; i--) {
		if (i < p.size) {
			printf("%d", p.p[i]);
		} else {
			printf("0");
		}
		if (i != 0) {
			printf(", ");
		}
	}
}

//int main(int argc, char ** argv) {
//	char poly_init[9] = { 1, 1, 0, 1, 1, 0, 1};
//	primitive_p.p = poly_init;
//	primitive_p.size = 7;
//
//	polynom p1;
//	char poly_init_2[7] = {1, 1, 0, 0, 1, 0, 1};
//	p1.p = poly_init_2;
//	p1.size = 7;
//	print("PP", primitive_p);
//	print("P1", p1);
//
//	polynom p2;
//	char poly_init_3[8] = {0, 1, 0, 1, 0, 0, 1, 1};
//	p2.p = poly_init_3;
//	p2.size = 8;
//	print("P2", p2);
//
//	polynom m = mult(p1, p2, primitive_p);
//	print("M",m);
//	printf("DG(M)=%d\n", m.size);
//	polynom s = shift_right(p1, 2);
//	print("SR", s);
//	printf("INT_EQ=%d\n", get_int_equivalent(s));
//	s = shift_left(p1, 2);
//	print("SL", s);
//
//	/* Find smallest int relatively prime with the pp */
//	printf("INT_EQ_PP=%d\n", get_int_equivalent(primitive_p));
//	int count = 0;
//	int i = 1;
//	int pp_int = get_int_equivalent(primitive_p);
//	char *generated;
//	generated = (char *)malloc(pp_int * sizeof(char));
//	polynom generator;
//	int order_group = power(2, primitive_p.size - 1) - 1;
//	do {
//		/* Initialize generated*/
//		int j;
//		for (j = 0; j < pp_int; j++) {
//			generated[j] = 0;
//		}
//		count = 0;
////		printf("%d\n", i);
//		for (i++; i < pp_int/2; i++) {
////			printf("%d\n", i);
//			if (relatively_prime(pp_int, i)) {
//				printf("%d\n", i);
//				break;
//			}
//		}
//		generator = get_poly_equiv(i, 0);
////		print("G", generator);
//		int gen_int;
//		gen_int = get_int_equivalent(generator);
////		printf(" %d ", gen_int);
//		polynom generated_poly = generator;
//		do {
//			generated_poly = mult(generated_poly, generator, primitive_p);
//			gen_int = get_int_equivalent(generated_poly);
////			printf(" %d ", gen_int);
//			if (generated[gen_int]) {
//				break;
//			} else {
//				generated[gen_int] = 1;
//			}
//			count++;
//		} while (1);
////		printf("%d \n", count);
//	} while (count != order_group && i < pp_int);
//	print("G", generator);
////	print("P3", p3);
////	polynom p13 = get_poly_equivalent(13, 0);
////	print("P13", p13);
//
//	printf("int SBOX_1[] = {");
//	for (i = 0; i < 64; i++) {
//		polynom p = get_poly_equiv(i, 0);
//		int j;
//		polynom res = p;
//		for (j = 1; j < 62; j++) {
//			res = mult(res, p, primitive_p);
//		}
//		printf("%d", get_int_equivalent(res));
//		if (i != 63) {
//			printf(", ");
//		}
//	}
//	printf ("};\n");
//
//
//	printf("int SBOX_2[] = {");
//		for (i = 0; i < 64; i++) {
//			polynom p = get_poly_equiv(i, 0);
//			int j;
//			polynom res = p;
//			for (j = 1; j < 5; j++) {
//				res = mult(res, p, primitive_p);
//			}
//			printf("%d", get_int_equivalent(res));
//			if (i != 63) {
//				printf(", ");
//			}
//		}
//	printf ("};\n");
//
//	printf("int SBOX_3[] = {");
//		for (i = 0; i < 64; i++) {
//			polynom p = get_poly_equiv(i, 0);
//			int j;
//			polynom res = p;
//			for (j = 1; j < 17; j++) {
//				res = mult(res, p, primitive_p);
//			}
//			printf("%d", get_int_equivalent(res));
//			if (i != 63) {
//				printf(", ");
//			}
//		}
//	printf ("};\n");
//
//	polynom e_square = mult(generator, generator, primitive_p);
//	printf("int SBOX_4[] = {");
//		for (i = 0; i < 64; i++) {
//			polynom p = get_poly_equiv(i, 0);
//			int j;
//			polynom res = p;
//			for (j = 1; j < 62; j++) {
//				res = mult(res, p, primitive_p);
//			}
//			res = add(res, e_square);
//			printf("%d", get_int_equivalent(res));
//			if (i != 63) {
//				printf(", ");
//			}
//		}
//	printf ("};\n");
//	/*
//	int SBOX_1[] = {0, 1, 45, 54, 59, 18, 27, 30, 48, 10, 9, 49, 32, 62, 15, 14, 24, 51, 5, 58, 41, 56, 53, 35, 16, 50, 31, 6, 42, 38, 7, 26, 12, 63, 52, 23, 47, 61, 29, 43, 57, 20, 28, 39, 55, 2, 60, 36, 8, 11, 25, 17, 34, 22, 3, 44, 21, 40, 19, 4, 46, 37, 13, 33};
//	int SBOX_2[] = {0, 1, 32, 51, 49, 3, 63, 31, 36, 4, 59, 9, 62, 45, 15, 14, 7, 5, 54, 38, 8, 57, 23, 52, 30, 61, 16, 33, 58, 42, 26, 24, 13, 43, 22, 34, 41, 60, 28, 27, 55, 48, 19, 6, 56, 12, 50, 20, 47, 10, 37, 18, 53, 35, 17, 21, 40, 44, 29, 11, 25, 46, 2, 39};
//	int SBOX_3[] = {0, 1, 38, 54, 37, 18, 43, 13, 20, 50, 25, 46, 42, 58, 15, 14, 32, 51, 5, 7, 47, 10, 34, 22, 12, 56, 2, 39, 24, 26, 62, 45, 28, 27, 35, 53, 8, 57, 31, 63, 4, 36, 16, 33, 11, 29, 55, 48, 41, 60, 21, 17, 23, 52, 3, 49, 9, 59, 30, 61, 44, 40, 19, 6};
//	int SBOX_4[] = {4, 5, 41, 50, 63, 22, 31, 26, 52, 14, 13, 53, 36, 58, 11, 10, 28, 55, 1, 62, 45, 60, 49, 39, 20, 54, 27, 2, 46, 34, 3, 30, 8, 59, 48, 19, 43, 57, 25, 47, 61, 16, 24, 35, 51, 6, 56, 32, 12, 15, 29, 21, 38, 18, 7, 40, 17, 44, 23, 0, 42, 33, 9, 37};
//	*/
//	printf("char mixing_layer[4][4][6]={ { {");
//	polynom x = get_gen_power(generator, 45, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 61, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 23, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 29, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}}, \n{{");
//	x = get_gen_power(generator, 25, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 44, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 54, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 59, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}}, \n{{");
//	x = get_gen_power(generator, 56, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 5, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 18, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 8, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}}, \n{{");
//	x = get_gen_power(generator, 55, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 17, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 23, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}, {");
//	x = get_gen_power(generator, 16, primitive_p);
//	print_poly_binary(x, primitive_p.size - 1);
//	printf("}} };");
//	/*
//	char mixing_layer[4][4][6]={
//	{{0, 0, 1, 0, 0, 0, 1, 1}, {0, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 1}},
//	{{0, 0, 0, 0, 1, 1, 0, 1}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 1, 0, 1, 1, 0}, {0, 0, 0, 1, 1, 0, 0, 0}},
//	{{0, 0, 0, 0, 0, 0, 1, 1}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 0, 1, 1, 1}, {0, 0, 1, 1, 0, 1, 1, 1}},
//	{{0, 0, 1, 0, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 1, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 1, 1}}
//	};
//	*/
//	return 0;
//}
