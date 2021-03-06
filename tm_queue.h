
#ifndef TM_QUEUE_H
#define TM_QUEUE_H

typedef enum {
	kTmQueueSimple = 0,
	kTmQueueLockless
} tm_queue_type;

typedef void tm_queue_ctx;

tm_queue_ctx *tm_queue_create(tm_queue_type type);
void tm_queue_destroy(tm_queue_ctx *_q);

int tm_queue_push_back(tm_queue_ctx *_q, void **push_array, int count);
int tm_queue_pop_front(tm_queue_ctx *_q, void **pop_array, int count);

int tm_queue_is_empty(tm_queue_ctx *_q);

#endif /* TM_QUEUE_H */
