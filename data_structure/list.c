#include "list.h"

#define ret_null(funcname, pointer) do {					\
		if (pointer == NULL) {								\
			fprintf(stderr, funcname "Not enough memory");	\
			return NULL;									\
		}													\
	} while(0)

list_t* list_init()
{
	list_t* alist = (list_t*)malloc(sizeof(list_t));
	ret_null("list_init", alist);	
	alist->head = alist->tail = NULL;
	alist->length = 0;
	return alist;
}

node_t* node_init(void *data)
{
	node_t *node = (node_t*)malloc(sizeof(node_t));
	ret_null("node_init", node);
	node->data = data;
	node->next = node->prev = NULL;
	return node;
}

void list_insert(list_t *list, node_t *node)
{
	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
	} else {
		list->head->prev = node;
		node->next = list->head;
		list->head = node;
	}
	list->length++;
}

node_t* list_remove_head(list_t *list)
{
	node_t* f = list->head;
	f->next->prev = NULL;
	list->head = f->next;
	list->length--;
	return f;
}

node_t* list_remove_tail(list_t *list)
{
	node_t *f = list->tail;
	f->prev->next = NULL;
	list->tail = f->prev;
	list->length--;	
	return f;
}

list_iterator_t* get_iterator(const list_t *list, int direction)
{
	list_iterator_t *iter = (list_iterator_t*)malloc(sizeof(list_iterator_t));
	ret_null("get_iterator", iter);

	iter->direction = direction;
	if (direction == LIST_FORWARD) {
		iter->next = list->head;	
	} else {
		iter->next = list->tail;
	}

	return iter;
}

node_t *iterator_next(list_iterator_t *iter) {
	node_t *current = iter->next;
	if (current != NULL) {
		if (iter->direction == LIST_FORWARD)
			iter->next = current->next;
		else
			iter->next = current->prev;
	}
	return current;
}

void free_iterator(list_iterator_t *iter)
{
	free(iter);
}

void list_traverse(const list_t* list, int direction, void (*func)(node_t*))
{
	node_t *node;
	list_iterator_t *it = get_iterator(list, direction);
	while ((node = iterator_next(it)) != NULL) {
		func(node);
	}
	free_iterator(it);	
}

void list_free(list_t *list)
{
	node_t *t, *node;
	for(node = list->head, t = node; node != NULL; node = node->next) {
		free(t->data);
		free(t);
	}
}


#ifdef TEST
#include <string.h>
#include <assert.h>

struct person
{
	int age;
	char name[32];
};

void dump_data(node_t *n)
{
	struct person *p = (struct person*)n->data;
	printf("age: %d -- name: %s\n", p->age, p->name);	
}

int main(int argc, char **argv)
{
	struct person *p;
	int tmp_age;
	char tmp_name[32];
	int i, n;

	list_t *l = list_init();
	node_t *tmp_node;

	printf("Number of people: ");
	scanf("%d", &n);

	for (i = 0; i < n; ++i) {
		p = (struct person*)malloc(sizeof(struct person));
		assert(p != NULL);
		printf("age: ");
		scanf("%d", &p->age);
		while(getchar()!='\n');
		printf("name: ");
		fgets(p->name, 32, stdin);
		p->name[strlen(p->name) - 1] = '\0';

		tmp_node = node_init(p);
		list_insert(l, tmp_node);
	}
	printf("List length: %d\n", l->length);

	printf("Traverse list forward\n");
	list_traverse(l, LIST_FORWARD, dump_data);
	printf("Traverse list backward\n");
	list_traverse(l, LIST_BACKWARD, dump_data);		

	tmp_node = list_remove_head(l);
	printf("Removed node: \n");
	dump_data(tmp_node);
	free(tmp_node);
	printf("New list. List length = %d\n", l->length);
	list_traverse(l, LIST_FORWARD, dump_data);

	tmp_node = list_remove_tail(l);
	printf("Removed node: \n");	
	dump_data(tmp_node);
	free(tmp_node);
	printf("New list. List length = %d\n", l->length);
	list_traverse(l, LIST_FORWARD, dump_data);

	list_free(l);
	
	return 0;
}

#endif
