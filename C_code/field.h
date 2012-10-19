typedef struct {
	char *p;
	char size;
} polynom;

char equals(polynom p, char *tst_string);
polynom initialize(char *init_string);
void print(char *p_name, polynom p);
void print_binary(char *p_name, polynom p);
polynom add(polynom a, polynom b);
polynom mult(polynom a, polynom b, polynom p);
int get_int_equivalent(polynom p);
polynom get_poly_equivalent(int p_int, int size);
polynom rotate(polynom);



