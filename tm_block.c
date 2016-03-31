#include "tm_block.h"

#include <stdlib.h>

static size_t block_size;

int tm_block_init(size_t block_data_field_size)
{
	int result = 1;

	block_size = sizeof(tm_block) + block_data_field_size;

	return result;
}
void tm_block_fin()
{
}

inline void tm_block_destroy(tm_block *block)
{
	if(block)
		free(block);
}

void tm_block_destructor(void *obj)
{
	tm_block_destroy((tm_block *)obj);
}

tm_block *tm_block_create()
{
	tm_block *block = NULL;
	block = (tm_block*) calloc(1, block_size);
	if (block)
		tm_refcount_init((tm_refcount*)block, tm_block_destructor);
	return block;
}

inline tm_block *tm_block_transfer_block(tm_block *block)
{
	if (!block)
		return NULL;

	return (tm_block*)tm_refcount_retain((void*)block);
}

inline void tm_block_dispose_block(tm_block *block)
{
	if (block)
		tm_refcount_release((void*)block);
}
