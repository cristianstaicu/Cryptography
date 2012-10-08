typedef struct {
	char *p;
	char size;
} polynom;

void print(char *p_name, polynom p);
polynom add(polynom a, polynom b);
polynom mult(polynom a, polynom b, polynom p);
int get_int_equivalent(polynom p);
polynom get_poly_equivalent(int p_int);




