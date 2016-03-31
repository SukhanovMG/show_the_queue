#include "tm_queue_lockless.h"

#include "rqueue.h"

typedef struct {
	rqueue_t *q;
} queue_lockless;

void tm_queue_destroy_lockless(void *_q)
{
	queue_lockless *q = (queue_lockless *)_q;
	if (q) {
		rqueue_destroy(q->q);
		free(q);
	}
}

void *tm_queue_create_lockless()
{
	queue_lockless *q = calloc(1, sizeof(queue_lockless));
	if (q){
		q->q = rqueue_create(1024*1024, RQUEUE_MODE_BLOCKING);
		if (!q->q) {
			free(q);
			q = NULL;
		}
	}

	return (void *) q;
}

int tm_queue_push_back_lockless(void *_q, void *data)
{
	int result = 0;
	queue_lockless *q = (queue_lockless *)_q;
	if (q && data) {
		if (rqueue_write(q->q, data) == 0)
			result = 1;
	}
	return result;
}

void *tm_queue_pop_front_lockless(void *_q)
{
	queue_lockless *q = (queue_lockless *)_q;
	void *data = NULL;
	if (q) {
		data = rqueue_read(q->q);
	}
	return data;
}

int tm_queue_is_empty_lockless(void *_q)
{
	queue_lockless *q = (queue_lockless *)_q;
	return (rqueue_isempty(q->q));
}

