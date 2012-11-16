char * transform(char *x);
char * transform_to_str(char *x, int l);
char* lsfr(char *polynom, char *initial_state, int output_length);
char shift_lsfr(char *polynom, int poly_size, char state[], int state_size );
void print_register(char *reg, int len);
char get_majority_bit(char b1, char b2, char b3, char b4, char b5);
