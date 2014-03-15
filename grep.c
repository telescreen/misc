/* grep algorithm devised by Rob Pike
 * Copied here for reference
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* PROTOTYPE */
int matchhere(char *re, char *text);
int matchstar(int c, char *re, char *text);


int matchstar(int c, char *re, char *text)
{
	char *t;

	for (t = text; *t != '\0' && (*t == c || c == '.'); t++)
		;
	do {   /* * matches zero or more */
		if (matchhere(re, t))
			return 1;
	} while (t-- > text);
	return 0;
}

int matchhere(char *re, char *text)
{
	if (re[0] == '\0')
		return 1;
	if (re[0] == '*')
		return matchstar(re[0], re+2, text);
	if (re[0] == '$' && re[1] == '\0')
		return *text == '\0';
	if (*text!='\0' && (re[0]=='.' || re[0]==*text))
		return matchhere(re+1, text+1);
	return 0;
}


int match(char *re, char *text)
{
	if (re[0] == '^')
		return matchhere(re+1, text);
	do {
		if (matchhere(re, text))
			return 1;
	} while(*text++ != 0);
	return 0;
}


/* grep: search for re in file */
int grep(char *re, FILE *f, char *name)
{
	int n, nmatch;
	char buf[BUFSIZ];

	nmatch = 0;
	while (fgets(buf, sizeof buf, f) != NULL) {
		n = strlen(buf);
		if (n > 0 && buf[n-1] == '\n')
			buf[n-1] = '\0';
		if (match(re, buf)) {
			nmatch++;
			if (name != NULL)
				printf("%s:", name);
			printf("%s\n", buf);
		}
	}
	return nmatch;
}

/* grep main: search for re in files */
int main(int argc, char *argv[])
{
	int i, nmatch;
	FILE *f;

	if (argc < 2) {
		fprintf(stderr, "usage: grep pattern [file ...]\n");
		exit(2);
	}
	nmatch = 0;
	if (argc < 3) {
		if (grep(argv[1], stdin, NULL))
			nmatch++;
	} else {
		for (i = 2; i < argc; i++) {
			f = fopen(argv[i], "r");
			if (f == NULL) {
				fprintf(stderr, "grep: can't open %s\n", argv[i]);
				continue;
			}
			if (grep(argv[1], f, argc>3 ? argv[i] : NULL))
				nmatch++;
			fclose(f);
		}
	}
	return nmatch == 0;
}
