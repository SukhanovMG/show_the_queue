#include "tm_refcount.h"

void tm_refcount_init(tm_refcount *refcount_ctx, tm_refcount_destructor destructor)
{
	refcount_ctx->counter = 1;
	refcount_ctx->destructor = destructor;
}

void *tm_refcount_retain(void *obj)
{
	tm_refcount *refcount_ctx = (tm_refcount*)obj;
	if (refcount_ctx)
		__sync_add_and_fetch(&refcount_ctx->counter, 1);
	return refcount_ctx;
}

void tm_refcount_release(void *obj)
{
	tm_refcount *refcount_ctx = (tm_refcount*)obj;
	if (refcount_ctx)
	{
		if (__sync_sub_and_fetch(&refcount_ctx->counter, 1) == 0) {
			if (refcount_ctx->destructor)
				refcount_ctx->destructor(obj);
		}
	}
}
