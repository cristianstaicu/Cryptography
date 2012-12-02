typedef struct {
	char *p;
	int size;
} polynom;

char equals(polynom p, char *tst_string);
polynom initialize(char *init_string);
polynom init_from_long(long x);
void print(char *p_name, polynom p);
void print_binary(char *p_name, polynom p);
polynom add(polynom a, polynom b);
polynom mult(polynom a, polynom b, polynom p);
int get_int_equivalent(polynom p);
polynom get_poly_equivalent(int p_int, int size);
polynom shift_left(polynom a, char pos);
polynom rotate(polynom);
polynom shift_right(polynom a, char pos);
polynom truncate_poly(polynom a, int size);


