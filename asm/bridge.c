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

//extern void binrepr(int a);
extern void hexrepr(int a);
extern unsigned snoob(unsigned a);
extern int habs(int a);
extern int hsign(int a);

void binrepr(unsigned a) {
	char r[32] = {0};
	int i;
	for (i = 0; i < 32; ++i) {
		r[31 - i] = ((a >> i) & 1) + '0';
	}
	puts(r);
}

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
	int i;
	/*for (i = 0; i < 100000000; ++i) {
		a = 752;
		b = snoob(a);
	}*/

	scanf("%d", &a);
	binrepr(a);
    b = snoob_c(a);
	binrepr(b);
	return 0;
}
