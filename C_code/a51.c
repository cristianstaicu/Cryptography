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

char sync_vector[3] = {0,0,0};

int taps_1[4] = {13,16,17,18};
int taps_2[2] = {20,21};
int taps_3[4] = {7,20,21,22};


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
int tap_xor_iteration (char* lsfr_vector, char* tap_array, int array_size) {
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

/*
 * input: lsfr vectors
 * output: modified sync vector
 *
 * We pass the lsfr vectors, we check in their sync_bits, and we write in sync vector
 * which value they have (remember that it's initialized to 0, so we modified it just if the sync bits are equal to 1
 */
void fill_sync_vector (char* lsfr_vector1, char* lsfr_vector2, char* lsfr_vector3, char* sync_vector ){
	int i = 0;
	int zero_counter = 0;
	if (lsfr_vector1[sync_1]==1){
		sync_vector[0] = 1;
	}
	if (lsfr_vector2[sync_2]==1){
		sync_vector[1] = 1;
	}
	if (lsfr_vector2[sync_2]==1){
			sync_vector[2] = 1;
	}
}

/*
 * input: sync vector
 * output: re-initialized sync vector
 *
 * After we applied fill_sync_vector, and we use its results, we should re-initialize the vector before usint
 * fill_sync_vector again
 */
void format_sync_vector (char* sync_vector){
	int i;
	for (i=0; i < 3; i++){
		sync_vector[i]=0;
	}
}

/*
 *
 */



/*
 * step 4 of the process. 100 iterations of check_sync_bits + xor operation.
 */
void step_4 (char* lsfr_vec1, char* lsfr_vec2, char* lsfr_vec3){
	int i;
	for (i=0, i < 100; i++){
	}
}

