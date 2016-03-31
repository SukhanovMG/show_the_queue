#ifndef TM_REFCOUNT_H
#define TM_REFCOUNT_H

typedef void (*tm_refcount_destructor)(void *);

typedef struct _tm_refcount {
	volatile unsigned int counter;
	tm_refcount_destructor destructor;
} tm_refcount;

void tm_refcount_init(tm_refcount *, tm_refcount_destructor);

void *tm_refcount_retain(void *obj);
void tm_refcount_release(void *obj);

#endif /* TM_REFCOUNT_H */
