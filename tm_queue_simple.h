#ifndef TM_QUEUE_SIMPLE_H
#define TM_QUEUE_SIMPLE_H

void tm_queue_destroy_simple(void *_q);
void *tm_queue_create_simple();
int tm_queue_push_back_simple(void *_q, void *data);
void *tm_queue_pop_front_simple(void *_q);
int tm_queue_is_empty_simple(void *_q);

#endif /*TM_QUEUE_SIMPLE_H*/