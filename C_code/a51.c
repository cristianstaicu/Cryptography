#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dim_v1 19
#define dim_v2 22
#define dim_v3 23
#define key_length 64
#define frame_vector_length 22
#define tap_size_1_3 4
#define tap_size_2 2


char lsfr_1[dim_v1];
char lsfr_2[dim_v2];
char lsfr_3[dim_v3];

char sync_1 = 8;
char sync_2 = 10;
char sync_3 = 10;

char sync_vector[3] = {0,0,0};

char taps_1[4] = {13,16,17,18};
char taps_2[2] = {20,21};
char taps_3[4] = {7,20,21,22};


/*
 *  Step1 - of A5/1 - Initialization of vectors
 */
void step1 (char *vector, int size){
	int i;

	for (i=0; i < size; i++){
		vector[i] = 0;
	}
}
/*
 *  Xor statement between 2 numbers (1 or 0)
 */
int xor (int x, int y){
	return (x+y)%2;
}

int xor3 (int x, int y, int z){
	int res;
	res = (x+y)%2;
	res = (res+z)%2;
	return res;
}

/*
 * Xor statement between taps (starting from the end of the array)
 * how is it possible to see from http://it.wikipedia.org/w/index.php?title=File:A5-1_GSM_cipher.svg&page=1
 */
char tap_xor_iteration (char* lsfr_vector, char* tap_array, int array_size) {
	char result = 0;
	char x;
	char y;
	for ( ; array_size > 0 ; array_size--){
		x = tap_array[array_size-1];
		y = lsfr_vector[(int)x];
		result = xor(result, y);
	}
	return result;
}

void shift_LSFR(char state[], int state_size, char res ) {
	int j;
	for (j = state_size - 1; j > 0 ; j--) {
		state[j] = state[j - 1];
	}
	state[0] = res;
}

void step_2 (char *key){
	int i;
	char res1;
	char res2;
	char res3;
	for (i=0; i < key_length; i++){
		res1 = tap_xor_iteration(lsfr_1, taps_1, tap_size_1_3);	//tap iteration = xor between taps within the vector
		res1 = xor(res1,key[i]);								//xor with the current bit value of the key
		shift_LSFR(lsfr_1, dim_v1, res1);						//shift the vector and put in the result
		res2 = tap_xor_iteration(lsfr_2, taps_2, tap_size_2);
		res2 = xor(res2,key[i]);
		shift_LSFR(lsfr_2, dim_v2, res2);
		res3 = tap_xor_iteration(lsfr_3, taps_3, tap_size_1_3);
		res3 = xor(res3,key[i]);
		shift_LSFR(lsfr_3, dim_v3, res3);
	}
}


/*
 * Step3 is very similar to the step2, the only thing that change is that we don't have the key,
 * but a frame_vector that has a different length
 */
void step_3 (char *frame_vector){
	int i;
	char res1, res2, res3;
	for (i=0; i < frame_vector_length; i++){
		res1 = tap_xor_iteration(lsfr_1, taps_1, tap_size_1_3);	//tap iteration = xor between taps within the vector
		res1= xor(res1,frame_vector[i]);					//xor with the current bit value of the frame_vector
		shift_LSFR(lsfr_1, dim_v1, res1);					//shift the vector and put in the result
		res2 = tap_xor_iteration(lsfr_2, taps_2, tap_size_2);
		res2 = xor(res2,frame_vector[i]);
		shift_LSFR(lsfr_2, dim_v2, res2);
		res3 = tap_xor_iteration(lsfr_3, taps_3, tap_size_1_3);
		res3 = xor(res3,frame_vector[i]);
		shift_LSFR(lsfr_3, dim_v3, res3);
	}

}


/*
 * get the majority of bits -> if the majority it's 1 then i'll return 1, 0 otherwise
 */
char get_majority_bit_a51(char b1, char b2, char b3) {
	char sum = b1 + b2 + b3;
	if (sum > 1)
		return 1;
	return 0;
}

/*
 * step 4 of the process. 100 iterations of check_sync_bits + xor operation.
 */
void step_4 (char* lsfr_vec1, char* lsfr_vec2, char* lsfr_vec3){
	int i;
	int maj_bit = 0;
	char res1;
	char res2;
	char res3;
	for (i=0; i<100; i++){
		maj_bit = get_majority_bit_a51(lsfr_vec1[sync_1], lsfr_vec2[sync_2], lsfr_vec3[sync_3]);
		if(lsfr_vec1[sync_1]==maj_bit){
			res1 = tap_xor_iteration(lsfr_1, taps_1, tap_size_1_3);
			shift_LSFR(lsfr_1, dim_v1, res1);
		}
		if(lsfr_vec2[sync_2]==maj_bit){
			res2 = tap_xor_iteration(lsfr_2, taps_2, tap_size_2);
			shift_LSFR(lsfr_2, dim_v2, res2);
		}
		if(lsfr_vec3[sync_3]==maj_bit){
			res3 = tap_xor_iteration(lsfr_3, taps_3, tap_size_1_3);
			shift_LSFR(lsfr_3, dim_v3, res3);
		}
	}
}

/*
 * Output -> xor of positions 18-22-23 of vectors 1-2-3
 * afterwards it's equal to the step 4
 */
void step_5 (char* lsfr_vec1, char* lsfr_vec2, char* lsfr_vec3, int output_length, char* output){
	int i;
	int maj_bit = 0;
	char res1, res2, res3;
	for (i=0; i < output_length; i++){
		output[i] = xor3(lsfr_vec1[dim_v1-1], lsfr_vec2[dim_v2-1], lsfr_vec3[dim_v3-1]);
		maj_bit = get_majority_bit_a51(lsfr_vec1[sync_1], lsfr_vec2[sync_2], lsfr_vec3[sync_3]);
		if(lsfr_vec1[sync_1]==maj_bit){
			res1 = tap_xor_iteration(lsfr_1, taps_1, tap_size_1_3);
			shift_LSFR(lsfr_1, dim_v1, res1);
		}
		if(lsfr_vec2[sync_2]==maj_bit){
			res2 = tap_xor_iteration(lsfr_2, taps_2, tap_size_2);
			shift_LSFR(lsfr_2, dim_v2, res2);
		}
		if(lsfr_vec3[sync_3]==maj_bit){
			res3 = tap_xor_iteration(lsfr_3, taps_3, tap_size_1_3);
			shift_LSFR(lsfr_3, dim_v3, res3);
		}
	}
}

/*
 * a51 protocol, which use previous functions to work
 */
char* a51 (char* key, int n){
	int i = 0;
	char* register_1 = lsfr_1;
	char* register_2 = lsfr_2;
	char* register_3 = lsfr_3;
	char frame_vector[] = {0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	char *output = (char *)malloc((n + 1) * sizeof(char));

	step1(register_1, dim_v1);
	step1(register_2, dim_v2);
	step1(register_3, dim_v3);

	step_2(key);
//	for (i = 0; i < dim_v1; i++) {
//		printf("%d", register_1[i]);
//	}
//	printf("\n");

	step_3(frame_vector);
	step_4(register_1, register_2, register_3);
	step_5(register_1, register_2, register_3, n, output);

	for (i = 0; i < n; i++) {
		output[i] += '0';
	}
	output[n] = 0;
	return output;
}

char * A51ENC(char *key, char* message) {
	char *keystream = ALL5(key, strlen(message));
	return xor_str(keystream, message);
}

char * A51DEC(char *key, char* ciphertext) {
	char *keystream = a51(key, strlen(ciphertext));
	return xor_str(keystream, ciphertext);
}
