#include "mcc_priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	int elem;
	struct mcc_priority_queue *pq = mcc_priority_queue_new(INT);

	srand(time(NULL));
	for (size_t i = 0; i < 20; i++)
		mcc_priority_queue_push(pq, &(int){rand() % 100});
	printf("len: %lu\n", mcc_priority_queue_len(pq));
	printf("capacity: %lu\n", mcc_priority_queue_capacity(pq));
	mcc_priority_queue_front(pq, &elem);
	printf("front: %d\n", elem);

	while (!mcc_priority_queue_is_empty(pq)) {
		mcc_priority_queue_front(pq, &elem);
		printf("%d ", elem);
		mcc_priority_queue_pop(pq);
	}
	putchar('\n');

	mcc_priority_queue_delete(pq);
	return 0;
}
