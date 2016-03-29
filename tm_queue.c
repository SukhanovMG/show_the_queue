#include "tm_queue.h"
#include "tm_queue_simple.h"
#include "tm_queue_lockless.h"
#include <stdlib.h>

typedef void *          (*queue_backend_create)    ();
typedef void            (*queue_backend_destroy)   (void *);
typedef int             (*queue_backend_push_back) (void *, void *);
typedef void *          (*queue_backend_pop_front) (void *);
typedef int             (*queue_backend_is_empty)  (void *);

#define SIMPLE_QUEUE_BACKEND(backend) \
do { \
	backend.ctx = NULL; \
	backend.create_func = (queue_backend_create) tm_queue_create_simple; \
	backend.destroy_func = (queue_backend_destroy) tm_queue_destroy_simple; \
	backend.push_back_func = (queue_backend_push_back) tm_queue_push_back_simple; \
	backend.pop_front_func = (queue_backend_pop_front) tm_queue_pop_front_simple; \
	backend.is_empty_func = (queue_backend_is_empty) tm_queue_is_empty_simple; \
} while (0)

#define LOCKLESS_QUEUE_BACKEND(backend) \
do { \
	backend.ctx = NULL; \
	backend.create_func = (queue_backend_create) tm_queue_create_lockless; \
	backend.destroy_func = (queue_backend_destroy) tm_queue_destroy_lockless; \
	backend.push_back_func = (queue_backend_push_back) tm_queue_push_back_lockless; \
	backend.pop_front_func = (queue_backend_pop_front) tm_queue_pop_front_lockless; \
	backend.is_empty_func = (queue_backend_is_empty) tm_queue_is_empty_lockless; \
} while (0)

typedef struct _queue_ctx {
	struct {
		void *ctx;
		queue_backend_create create_func;
		queue_backend_destroy destroy_func;
		queue_backend_push_back push_back_func;
		queue_backend_pop_front pop_front_func;
		queue_backend_is_empty is_empty_func;
	} backend;

	tm_queue_type queue_type;
} queue_ctx;

tm_queue_ctx *tm_queue_create(tm_queue_type type)
{
	queue_ctx *q = calloc(1, sizeof(queue_ctx));
	if (q) {
		q->queue_type = type;
		switch(type) {
			case kTmQueueSimple:
				SIMPLE_QUEUE_BACKEND(q->backend);
				break;
			case kTmQueueLockless:
				LOCKLESS_QUEUE_BACKEND(q->backend);
				break;
			default:
				SIMPLE_QUEUE_BACKEND(q->backend);
				break;
		}

		q->backend.ctx = q->backend.create_func();
		if (!q->backend.ctx) {
			free(q);
			q = NULL;
		}
	}

	return (tm_queue_ctx *) q;
}

void tm_queue_destroy(tm_queue_ctx *_q)
{
	queue_ctx *q = (queue_ctx *) _q;
	if (q) {
		if (q->backend.destroy_func)
			q->backend.destroy_func(q->backend.ctx);
		free(q);
	}
}

int tm_queue_push_back(tm_queue_ctx *_q, void **push_array, int count)
{
	queue_ctx *q = (queue_ctx *) _q;
	int result = 0;

	if (q && push_array && count > 0) {
		for (int i = 0; i < count; i++ ) {
			result = q->backend.push_back_func(q->backend.ctx, &push_array[i]);
			if (result != 1) {
				break;
			}
		}
	}
	return result;
}

int tm_queue_pop_front(tm_queue_ctx *_q, void **pop_array, int count)
{
	queue_ctx *q = (queue_ctx *) _q;
	int result = 0;

	if (q && pop_array && count > 0) {
		for (int i = 0; i < count; i++) {
			pop_array[i] = q->backend.pop_front_func(q->backend.ctx);
			if (pop_array[i] == NULL)
				break;
		}
		result = pop_array[0] != NULL;
	}
	return result;
}

int tm_queue_is_empty(tm_queue_ctx *_q)
{
	queue_ctx *q = (queue_ctx *) _q;
	return q && q->backend.is_empty_func(q->backend.ctx);
}
