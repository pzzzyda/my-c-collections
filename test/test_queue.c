#include "mcc_queue.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
	struct mcc_queue *s = mcc_queue_new(mcc_int());
	assert(s != NULL);
	for (int i = 0; i < 10; i++)
		assert(!mcc_queue_push(s, &i));
	while (!mcc_queue_is_empty(s)) {
		int *ref;
		assert(!mcc_queue_front(s, (void **)&ref));
		printf("%d ", *ref);
		mcc_queue_pop(s);
	}
	putchar('\n');
	mcc_queue_drop(s);
	return 0;
}
