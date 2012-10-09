#include <stdio.h>
#include "field.h"
#include "mintest.h"

int tests_run = 0;

/* Wikipedia example */
static char * test_addition() {
	polynom a = initialize("01010011");
	polynom b = initialize("11001010");
	polynom c = add(a, b);
	mu_assert("Addition is not working in finite field arithmetic!", equals(c, "10011001"));
	return 0;
}

/* Rindjael wiki example: 53 x CA*/
static char * test_mult() {
	polynom a = initialize("11001010");
	polynom b = initialize("01010011");
	polynom p = initialize("100011011");
	polynom c = mult(a, b, p);
	mu_assert("Multiplication is not working in finite field arithmetic!", equals(c, "00000001"));
	return 0;
}

static char * all_tests() {
	mu_run_test(test_addition);
	mu_run_test(test_mult);
	return 0;
}

int main(int argc, char ** argv) {

	char *result = all_tests();
	if (result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
