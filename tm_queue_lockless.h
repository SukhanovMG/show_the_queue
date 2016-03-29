#ifndef TM_QUEUE_LOCKLESS_H
#define TM_QUEUE_LOCKLESS_H

void tm_queue_destroy_lockless(void *_q);
void *tm_queue_create_lockless();
int tm_queue_push_back_lockless(void *_q, void *data);
void *tm_queue_pop_front_lockless(void *_q);
int tm_queue_is_empty_lockless(void *_q);

#endif /*TM_QUEUE_LOCKLESS_H*/