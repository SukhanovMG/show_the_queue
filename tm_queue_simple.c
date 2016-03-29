#include "tm_queue_simple.h"
#include "utlist.h"
#include <stdlib.h>

typedef struct _queue_elem_simple {
	struct _queue_elem_simple *next;
	struct _queue_elem_simple *prev;
	void *data;
} queue_elem_simple;

typedef struct _queue_simple {
	queue_elem_simple *head;
} queue_simple;

void tm_queue_destroy_simple(void *_q)
{
	queue_simple *q = (queue_simple *)_q;
	if (q) {
		while (q->head) {
			tm_queue_pop_front_simple(q);
		}
		free(q);
	}
}

void *tm_queue_create_simple()
{
	queue_simple *q = calloc(1, sizeof(queue_simple));
	if (!q){
		return q;
	}
	return (void *) q;
}

int tm_queue_push_back_simple(void *_q, void *data)
{
	queue_simple *q = (queue_simple *)_q;
	queue_elem_simple *elem = NULL;

	if (!q || !data)
		return 0;

	elem = calloc(1, sizeof(queue_elem_simple));
	if (!elem)
		return 0;

	elem->data = data;

	DL_APPEND(q->head, elem);

	return 1;
}

void *tm_queue_pop_front_simple(void *_q)
{
	queue_simple *q = (queue_simple *)_q;
	queue_elem_simple *elem = NULL;
	void *data = NULL;
	if (q && q->head) {
		elem = q->head;
		DL_DELETE(q->head, elem);
		data = elem->data;
		free(elem);
	}

	return data;
}

int tm_queue_is_empty_simple(void *_q)
{
	queue_simple *q = (queue_simple *)_q;
	return q && !q->head;
}

