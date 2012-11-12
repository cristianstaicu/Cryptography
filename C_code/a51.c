#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size_lsfr_1 = 19;
int size_lsfr_2 = 22;
int size_lsfr_3 = 23;

char key_length = 64;
char frame_vector_length = 22;

char lsfr_1[size_lsfr_1] = {};
char lsfr_2[size_lsfr_2] = {};
char lsfr_3[size_lsfr_3] = {};

int sync_1 = 8;
int sync_2 = 10;
int sync_3 = 10;

int taps_1[] = {13,16,17,18};
int taps_2[] = {20,21};
int taps_3[] = {7,20,21,22};


/*
 *  Step1 - of A5/1 - Initialization of vectors
 */
void initialize (char *vector, int size){
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

/*
 * Xor statement between taps (starting from the end of the array)
 * how is it possible to see from http://it.wikipedia.org/w/index.php?title=File:A5-1_GSM_cipher.svg&page=1
 */
void tap_xor_iteration (char* lsfr_vector, char* tap_array, int array_size) {
	int result = 0;
	int x;
	for ( ; array_size >= 0 ; array_size--){
		x = tap_array[array_size];
		result = xor(result, x);
	}
	return result;
}

/*
 * Shift the vector by one, and add in pos 0 the result of the xor
 * TODO fix it, doesn't return anything - fix the function
 *
void shift_lsfr (char* state, int state_size ,int xor_result){
	char next_state[state_size] = {};
	int j;
	for (j = 1; j < state_size; j++) {
		next_state[j] = state[j - 1];
	}
	next_state[0] = xor_result;
}
*/
/*
 * (this process must be done 64 times - number of the bit of the key)
 * Iterate every lsfr vector -> do the xor with the taps of the vector and store in res (an integer)
 * Do xor statement with res and the current bit of the key
 * Add the result to the lsfr vector using the shift statement
 */
void step_2 (char *key){
	int i;
	int res;
	for (i=0; i < key_length; i++){
		res = tap_xor_iteration(lsfr_1, taps_1);	//tap iteration = xor between taps within the vector
		res = xor(res,key[i]);						//xor with the current bit value of the key
		shift_lsfr(lsfr_1, size_lsfr_1, res);		//shift the vector and put in the result
		res = tap_xor_iteration(lsfr_2, taps_2);
		res = xor(res,key[i]);
		shift_lsfr(lsfr_2, size_lsfr_2, res);
		res = tap_xor_iteration(lsfr_3, taps_3);
		res = xor(res,key[i]);
		shift_lsfr(lsfr_3, size_lsfr_3, res);
	}
}


/*
 * Step3 is very similar to the step2, the only thing that change is that we don't have the key,
 * but a frame_vector that has a different length
 */
void step_3 (char *frame_vector){
	int i;
	int res;
	for (i=0; i < frame_vector_length; i++){
		res = tap_xor_iteration(lsfr_1, taps_1);	//tap iteration = xor between taps within the vector
		res = xor(res,frame_vector[i]);				//xor with the current bit value of the frame_vector
		shift_lsfr(lsfr_1, size_lsfr_1, res);		//shift the vector and put in the result
		res = tap_xor_iteration(lsfr_2, taps_2);
		res = xor(res,frame_vector[i]);
		shift_lsfr(lsfr_2, size_lsfr_2, res);
		res = tap_xor_iteration(lsfr_3, taps_3);
		res = xor(res,frame_vector[i]);
		shift_lsfr(lsfr_3, size_lsfr_3, res);
	}

}



