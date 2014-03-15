#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __err_if_null(funcname, pointer) do {					\
		if (pointer == NULL) {									\
			fprintf(stderr, funcname "Not enough memory");		\
			return NULL;										\
		}														\
	} while(0)

#define SPLIT 256
#define MATCH 257

typedef struct State {
	int c;
	struct State *out;
	struct State *out1;
	int lastlist;
} State;

typedef struct Ptrlist {
	State *s;
	struct Ptrlist *next;
} Ptrlist;

typedef struct Frag {
	State *start;
	Ptrlist *out;    /* A list points to states */
} Frag;

State *matchstate;

inline Frag frag(State *start, Ptrlist *out)
{
	Frag s;
	s.start = start;
	s.out = out;
	return s;
}

Ptrlist* list1(State **outp)
{
	Ptrlist *l;
	l = (Ptrlist*)malloc(sizeof(Ptrlist));
	__err_if_null("list1", l);

	l->s = *outp;
	l->next = NULL;

	return l;
}

Ptrlist* append(Ptrlist *l1, Ptrlist *l2)
{
	l1->next = l2;
	return l1;
}

void patch(Ptrlist *l, State *s)
{
	while (l != NULL) {
		l->s = s;
		l = l->next;
	}
}

State *state(int c, State *out, State *out1)
{
	State *s;
	s = (State*)malloc(sizeof(State));
	__err_if_null("state", s);
	
	s->c = c;
	s->out = out;
	s->out1 = out1;
	s->lastlist = 0;
	return s;
}


#define push(s) *stackp++ = s
#define pop() *--stackp

State* post2nfa(char *postfix)
{
	char *p;
	Frag stack[1000], *stackp, e1, e2, e;
	State *s;

	stackp = stack;
	for(p = postfix; *p; p++) {
		switch(*p) {
		default:
			s = state(*p, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;
		case '.':
			e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			push(frag(e1.start, e2.out));
			break;
		case '|':
			e2 = pop();
			e1 = pop();
			s = state(SPLIT, e1.start, e2.start);
			push(frag(s, append(e1.out, e2.out)));
			break;
		case '?':
			e = pop();
			s = state(SPLIT, e.start, NULL);
			push(frag(s, append(e.out, list1(&s->out1))));
			break;
		case '*':
			e = pop();
			s = state(SPLIT, e.start, NULL);
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
			break;
		}
	}
	e = pop();
	patch(e.out, matchstate);
	return e.start;
}


void dump_state(State *s)
{
	if (s != NULL) {
		puts(s->c);
		dump_state(out);
		dump_state(out1);
	}
}

int main(int argc, char **argv)
{
	char regex_str[128];
	fgets(regex_str, 128, stdin);
	regex_str[strlen(regex_str)-1] = '\0';
	
	State *s = post2nfa(regex_str);
	return 0;
}
