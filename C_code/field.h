typedef struct {
	char *p;
	char size;
} polynom;

polynom initialize(char *);
void print(char *p_name, polynom p);
void print_binary(char *p_name, polynom p);
polynom add(polynom a, polynom b);
polynom mult(polynom a, polynom b, polynom p);
polynom mult_a(polynom a, char b[], int n, polynom p);
int get_int_equivalent(polynom p);
polynom get_poly_equivalent(int p_int);
polynom rotate(polynom);



