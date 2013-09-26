/*
 * author: Bui Hong Ha
 * created: 2013/09/23 05:09:06
 */

#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIST_FORWARD 0
#define LIST_BACKWARD 1

#define list_empty(list) ((list)->length == 0)

typedef struct node_tag {
	void *data;
	struct node_tag *next, *prev;
} node_t;


typedef struct list_iterator_tag {
	node_t *next;
	int direction;
} list_iterator_t;


typedef struct list_tag {
	node_t *head;
	node_t *tail;
	int length;
} list_t;


list_t* list_init();
node_t* node_init();
void list_insert(list_t *list, node_t *node);
node_t* list_remove_head(list_t *list);
node_t* list_remove_tail(list_t *list);
list_iterator_t *get_iterator(const list_t *list, int direction);
node_t* iterator_next(list_iterator_t *iter);
void free_iterator(list_iterator_t *iter);
void list_traverse(const list_t* l, int direction, void (*func)(node_t*));
void list_free(list_t *l);

#endif /* __LIST_H__ */

