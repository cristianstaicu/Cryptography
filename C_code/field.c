#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "field.h"

/*
 * Copy from the  source char array to the dest one starting from start_pos to end_pos
 */
void copy(char * source, char *dest, char start_pos, char end_pos) {
	int i = 0;
	for (i = start_pos; i < end_pos; i++) {
		dest[i] = source[i];
	}
}

polynom truncate_poly(polynom a, int size) {
	int i = 0;
	polynom res;
	res.size = size;
	res.p = (char *) malloc(size * sizeof(char));
	for (i = 0; i < size; i++) {
		if (i < a.size)
			res.p[i] = a.p[i];
		else
			res.p[i] = 0;
	}
	return res;
}

/* Add two polynoms possibly having different sizes. */
polynom add(polynom a, polynom b) {

	/* min_order = degree of the shorter polynom
	 * max_order = degree of the longer polynom */
	char min_order = 0, max_order = 0;
	polynom res;

	if (a.size > b.size) {
		min_order = b.size;
		max_order = a.size;
	} else {
		min_order = a.size;
		max_order = b.size;
	}

	res.p = (char *)malloc(max_order * sizeof(char));
	/* copy the last (max_order - min_order) bits with the bits from the longer one */
	if (min_order != max_order) {
		copy(b.p, res.p, min_order, max_order);
	}

	/* Compute the first min_order bits by xor-ing the bits of the two polynoms */
	char i = 0;
	for (i = 0; i < min_order; i++) {
		res.p[i] = (a.p[i] + b.p[i]) % 2;
	}
	res.size = max_order;
	return res;
}

/* Shift the given polynom, "pos" positions to the left. 0s are inserted from the right.
 * The result has a.size + pos bits and not a.size !!! */
polynom shift_left(polynom a, char pos) {
	if (pos <= 0)
		return a;
	polynom res;
	res.size = a.size + pos;
	/* Dynamic allocation */
	res.p = (char *)malloc(res.size * sizeof(char));
	int i;
	for (i = 0; i < pos; i++) {
		res.p[i] = 0;
	}
	for (i = 0; i < a.size; i++) {
		res.p[i+pos] = a.p[i];
	}
	return res;
}

polynom shift_right(polynom a, char pos) {
	if (pos <= 0)
		return a;
	polynom res;
	res.size = a.size + pos;
	/* Dynamic allocation */
	res.p = (char *)malloc(res.size * sizeof(char));
	int i;
	for (i = 0; i < a.size; i++) {
		res.p[i] = a.p[i];
	}
	return res;
}

/* Move the most important bit of the polynom on the first position + shift all the others left*/
polynom rotate(polynom p) {
	polynom res;
	res.size = p.size;
	res.p = (char *)malloc(res.size * sizeof(char));
	int i;
	char aux = p.p[res.size - 1];
	for (i = 1; i <= res.size - 1; i++) {
		res.p[i] = p.p[i-1];
	}
	res.p[0] = aux;
	return res;
}

/* Compute the grade of the polynom (1 + position of the 1st bit set to '1' starting from left to right)*/
int get_grade(polynom p) {
	int i;
	for (i = p.size; i >= 0; i--) {
		if (p.p[i]) {
			return i + 1;
		}
	}
	return -1;
}

/* Multiply the two polynomials (a,b) using the primitive polynomial (p) */
polynom mult(polynom a, polynom b, polynom p) {
	int result_size = p.size - 1;
	polynom res;
	int max_len = a.size + b.size - 1;
	/* Initialize */
	res.p = (char *)malloc((max_len) * sizeof(char));
	int i,j;
	for (i = 0; i < max_len; i++) {
		res.p[i] = 0;
	}
	/* Multiplication similar with the decimal one*/
	for (i = 0; i < b.size; i++) {
		if (b.p[i] != 0) {
			for (j = 0; j < a.size; j++) {
				if (a.p[j] != 0) {
					res.p[i+j] = (res.p[i+j] + 1) % 2;
				}
			}
		}
	}
	/* Find the grade */
	res.size = 0;
	for (i = max_len-1; i >= 0; i--) {
		if (res.p[i]) {
			res.size = i + 1;
			break;
		}
	}
	/* Divide by the primitive polynom */
	int g;
	while ((g = get_grade(res)) >= p.size) {
		int dif = g - p.size;
		polynom biased = shift_left(p, dif);
		res = add(res, biased);
	}

	res.size = result_size;
	return res;
}

/* Print the polynom as a binary string. Starting from the most significant bit (p[p.size-1]) */
void print_binary(char *p_name, polynom p) {
	printf("%s=", p_name);
	int i = 0;
	for (i = p.size - 1; i>=0; i--) {
		printf("%d", p.p[i]);
	}
	printf("\n");
}

/* Initialize a polynom structure with the char array given as parameter
 * (dynamically allocate it + set the size + content).*/
/*
 * !!! Pay attention that in polynom the chars are stored in c style (0->n)
 * not polynomial style (n->0) !!!
 */
polynom initialize(char *p) {
	polynom res;
	res.size = strlen(p);
	res.p = (char *) malloc(res.size * sizeof(char));
	int i = 0;
	for (i = 0; i < res.size; i++) {
		res.p[res.size - i - 1] = p[i] - '0';
	}
	return res;
}

polynom init_from_long(long x) {
	int i = 0,j = 0;
	char bits[140];
	while (x > 0) {
		bits[i++] = x % 2;
		x = x / 2;
	}
	polynom res;
	res.size = i;
	res.p = (char *) malloc(res.size * sizeof(char));
	for (j = 0; j < i; j++) {
		res.p[j] = bits[j];
	}
	return res;
}

/* Check if each chars stored in polynom p is equal with the ones stored in the
 * char array given as param. */
/*
 * !!! Pay attention that in polynom the chars are stored in c style (0->n)
 * not polynomial style (n->0) !!!
*/
char equals(polynom p, char *tst_string) {
	int i;
	for (i = 0; i < p.size; i++) {
		if (p.p[p.size - i - 1] != tst_string[i] - '0')
			return 0;
	}
	return 1;
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

/* It takes a polynomial (a string of bits) and computes the equivalent integer number.
 * The most important element of the polynomial correspond to the bigger base power.
 * E.g. x^4+x^2+x+1=10111=16+4+2+1=23
 * */
int get_int_equivalent(polynom p) {
	int res = 0;
	int i, pow = 1;
	for (i = 0; i < p.size; i++) {
		res += p.p[i] * pow;
		pow <<= 1;
	}
	return res;
}

/* Recursive algorithm to initialize the polynomial step by step*/
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

/* Extend a given polynomial to a specific size. Assumption: size > p.size */
polynom extend(polynom p, int size) {
	polynom res;
	res.p = (char *)malloc(size * sizeof(char));
	res.size = size;
	int i = 0;
	for (i = 0; i < p.size; i++) {
		res.p[i] = p.p[i];
	}
	for (i = p.size; i < size; i++) {
		res.p[i] = 0;
	}

	return res;
}

/* Compute the polynomial equivalent and extend it to a given size.
 * It uses the get_poly_equiv recursive algorithm */
polynom get_poly_equivalent(int p_int, int size) {
	polynom p = get_poly_equiv(p_int, 0);
	return extend(p, size);
}

/* Compute the generator to the ith power using the field's primitive polynomial */
polynom get_gen_power(polynom generator, int pow, polynom primitive) {
	polynom res = generator;
	int i = 0;
	for (i = 1; i < pow; i++) {
		res = mult(res, generator, primitive);
	}
	return res;
}

/* Print the given polynom as a binary string. (The most left bit is the most important).
 * The second argument tells how much the polynom must be extended with 0s to the left.*/
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

/* Test if two numbers are relatively prime. It is used to minimize the search space
 * for the generator. It employs the Euclid's Algorithm for cmmdc. */
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

/* Raise a to the power b*/
int powr(int a, int b) {
	int res = 1, i = 0;
	while (i < b) {
		 res *= a;
		 i++;
	}
	return res;
}


/* DEPRECATED!!! Functions used to generate sboxed + mixing layer*/
void generate_sboxes_ml() {
	polynom primitive_p;
	char poly_init[9] = { 1, 1, 0, 1, 1, 0, 1};
	primitive_p.p = poly_init;
	primitive_p.size = 7;

	polynom p1;
	char poly_init_2[7] = {1, 1, 0, 0, 1, 0, 1};
	p1.p = poly_init_2;
	p1.size = 7;
	print("PP", primitive_p);
	print("P1", p1);

	polynom p2;
	char poly_init_3[8] = {0, 1, 0, 1, 0, 0, 1, 1};
	p2.p = poly_init_3;
	p2.size = 8;
	print("P2", p2);

	polynom m = mult(p1, p2, primitive_p);
	print("M",m);
	printf("DG(M)=%d\n", m.size);
	polynom s;
	s = shift_left(p1, 2);
	print("SL", s);

	/* Find smallest int relatively prime with the pp */
	printf("INT_EQ_PP=%d\n", get_int_equivalent(primitive_p));
	int count = 0;
	int i = 1;
	int pp_int = get_int_equivalent(primitive_p);
	char *generated;
	generated = (char *)malloc(pp_int * sizeof(char));
	polynom generator;
	int order_group = powr(2, primitive_p.size - 1) - 1;
	do {
		/* Initialize generated*/
		int j;
		for (j = 0; j < pp_int; j++) {
			generated[j] = 0;
		}
		count = 0;
		for (i++; i < pp_int/2; i++) {
			if (relatively_prime(pp_int, i)) {
				printf("%d\n", i);
				break;
			}
		}
		generator = get_poly_equiv(i, 0);
		int gen_int;
		gen_int = get_int_equivalent(generator);
		polynom generated_poly = generator;
		do {
			generated_poly = mult(generated_poly, generator, primitive_p);
			gen_int = get_int_equivalent(generated_poly);
			if (generated[gen_int]) {
				break;
			} else {
				generated[gen_int] = 1;
			}
			count++;
		} while (1);
	} while (count != order_group && i < pp_int);
	print("G", generator);

	printf("int SBOX_1[] = {");
	for (i = 1; i < 64; i++) {
		polynom p = get_poly_equiv(i, 0);
		int j;
		polynom res = p;
		for (j = 1; j < 62; j++) {
			res = mult(res, p, primitive_p);
		}
		printf("%d", get_int_equivalent(res));
		if (i != 63) {
			printf(", ");
		}
	}
	printf ("};\n");


	printf("int SBOX_2[] = {");
		for (i = 1; i < 64; i++) {
			polynom p = get_poly_equiv(i, 0);
			int j;
			polynom res = p;
			for (j = 1; j < 5; j++) {
				res = mult(res, p, primitive_p);
			}
			printf("%d", get_int_equivalent(res));
			if (i != 63) {
				printf(", ");
			}
		}
	printf ("};\n");

	printf("int SBOX_3[] = {");
		for (i = 1; i < 64; i++) {
			polynom p = get_poly_equiv(i, 0);
			int j;
			polynom res = p;
			for (j = 1; j < 17; j++) {
				res = mult(res, p, primitive_p);
			}
			printf("%d", get_int_equivalent(res));
			if (i != 63) {
				printf(", ");
			}
		}
	printf ("};\n");

	polynom e_square = mult(generator, generator, primitive_p);
	printf("int SBOX_4[] = {");
		for (i = 1; i < 64; i++) {
			polynom p = get_poly_equiv(i, 0);
			int j;
			polynom res = p;
			for (j = 1; j < 62; j++) {
				res = mult(res, p, primitive_p);
			}
			res = add(res, e_square);
			printf("%d", get_int_equivalent(res));
			if (i != 63) {
				printf(", ");
			}
		}
	printf ("};\n");
	printf("char mixing_layer[4][4][6]={ { {");
	polynom x = get_gen_power(generator, 45, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 61, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 23, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 29, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}}, \n{{");
	x = get_gen_power(generator, 25, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 44, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 54, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 59, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}}, \n{{");
	x = get_gen_power(generator, 56, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 5, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 18, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 8, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}}, \n{{");
	x = get_gen_power(generator, 55, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 17, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 23, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}, {");
	x = get_gen_power(generator, 16, primitive_p);
	print_poly_binary(x, primitive_p.size - 1);
	printf("}} };");
}
