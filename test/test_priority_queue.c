#include "mcc_priority_queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	struct mcc_priority_queue_iter *iter;
	int *mut_ref;
	const int *ref;
	struct mcc_priority_queue *q;

	assert((q = mcc_priority_queue_new(mcc_int())) != NULL);
	srand(time(NULL));
	for (size_t i = 0; i < 30; i++)
		assert(!mcc_priority_queue_push(q, &(int){rand() % 100}));

	iter = mcc_priority_queue_iter_new(q);
	assert(iter != NULL);
	while (mcc_priority_queue_iter_next(iter, (const void **)&ref))
		printf("%d ", *ref);
	putchar('\n');

	while (!mcc_priority_queue_is_empty(q)) {
		assert(!mcc_priority_queue_front(q, (void **)&mut_ref));
		printf("%d ", *mut_ref);
		mcc_priority_queue_pop(q);
	}
	putchar('\n');

	mcc_priority_queue_drop(q);
	return 0;
}
