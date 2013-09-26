/*
 * How to compile program
 * 1. Write extern in a c program
 * 2. Write assembly routine
 * 3. Compile 2 files
 *    - For c program:
 *         $ gcc -m32 -c bridge.c
 *    - For asm routine:
 *         $ yasm -a x86 -f macho -o file.o file.asm
 * 4. Link 2 object files:
 *    $ gcc -o output file.o bridge.o
 */

#include <stdio.h>

extern void binrepr(int a);
extern void hexrepr(int a);
extern unsigned snoob(unsigned a);
extern int ha_abs(int a);
extern int ha_sign(int a);

unsigned snoob_c(unsigned x)
{
	unsigned smallest, ripple, ones;

	smallest = x & -x;
	ripple = x + smallest;
	ones = x ^ ripple;
	ones = (ones >> 2) / smallest;
	return ripple | ones;
}

int main()
{
	int a, b;
	/*	int i;
	for (i = 0; i < 100000000; ++i) {
		a = 752;
		b = snoob_c(a);
		}*/

	scanf("%d", &a);
	printf("abs(a) = %d\n", ha_abs(a));
	printf("sign(a) = %d\n", ha_sign(a));
	return 0;
}
