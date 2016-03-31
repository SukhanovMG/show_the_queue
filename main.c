#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tm_queue.h"
#include "tm_block.h"

#define BLOCK_SIZE 1024
#define BLOCK_RATE 128
#define BLOCK_PERIOD ( 1.0 / BLOCK_RATE )
#define BLOCKS_PER_PERIOD 100

#define TEST_TIME 10

double get_time()
{
	double result = 0.0;
	struct timespec ts;
	int res = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (res != 0) {
		fprintf(stderr, "%s\n", "clock_gettime failed");
		result = -1.0;
	} else {
		result = (double) ts.tv_sec + 1.0e-9 * ts.tv_nsec;
	}
	return result;
}

typedef struct _work_thread {
	pthread_t id;
	tm_queue_ctx *q;
	volatile int stop;
} work_thread;

void *thread_func(void *arg)
{
	work_thread *thread = (work_thread*)arg;
	while (__sync_add_and_fetch(&thread->stop, 0) == 0) {
		void *data_array[100] = {NULL};
		tm_queue_pop_front(thread->q, (void**)data_array, 100);
		for (size_t i = 0; i < 100; i++) {
			if (data_array[i] == NULL) {
				break;
			}
			tm_block *block = (tm_block*)data_array[i];
			tm_block_dispose_block(block);
		}
	}
	return NULL;
}

void work_thread_delete(work_thread *thread)
{
	if (thread) {
		while(1) {
			tm_block *block = NULL;
			tm_queue_pop_front(thread->q, (void**)&block, 1);
			if (block)
				tm_block_dispose_block(block);
			else
				break;
		}
		tm_queue_destroy(thread->q);
		free(thread);
	}
}

work_thread *work_thread_new()
{
	int success = 0;
	work_thread *thread = 0;
	thread = calloc(1, sizeof(work_thread));
	if (thread) {
		thread->q = tm_queue_create(kTmQueueLockless);
		if (thread->q) {
			int res = pthread_create(&thread->id, NULL, thread_func, thread);
			success = res == 0 ? 1 : 0;
		}
	}

	if (!success){
		work_thread_delete(thread);
		thread = NULL;
	}

	return thread;
}

int main()
{
	tm_block_init(BLOCK_SIZE);
	work_thread *wt = work_thread_new();
	if (wt) {
		double start_time = 0.0;
		while(42) {
			double current_time = get_time();
			double time_after_work;
			double diff;
			long sleep_time_usec = 0;
			tm_block *block = NULL;
			void *data_array[BLOCKS_PER_PERIOD];

			if (start_time == 0.0)
				start_time = current_time;

			if (current_time - start_time > TEST_TIME)
				break;

			block = tm_block_create();
			for (size_t i = 0; i < BLOCKS_PER_PERIOD; i++)
				data_array[i] = (void*)tm_block_transfer_block(block);
			tm_queue_push_back(wt->q, (void**)data_array, BLOCKS_PER_PERIOD);
			tm_block_dispose_block(block);

			time_after_work = get_time();
			diff = time_after_work - current_time;
			while (diff > BLOCK_PERIOD)
				diff -= BLOCK_PERIOD;

			sleep_time_usec = (BLOCK_PERIOD - diff) * 1.0e6;
			usleep(sleep_time_usec);
		}
		__sync_add_and_fetch(&wt->stop, 1);
		pthread_join(wt->id, NULL);
	}
	work_thread_delete(wt);
	return 0;
}