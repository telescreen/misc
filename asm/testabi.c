#include <stdio.h>

int test(double *a)
{
	*a = 5.5f;
}

int main()
{
	double a;
	test(&a);
	return 0;
}
