#include "mcc_heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmp_int(const void *a, const void *b)
{
	return *(const int *)a - *(const int *)b;
}

int main()
{
	struct mcc_heap *h1;
	struct mcc_heap_iter iter;

	h1 = mcc_heap_new(MCC_BASICS(int), cmp_int);

	srand(time(NULL));
	for (size_t i = 0; i < 30; i++)
		mcc_heap_push(h1, &(int){rand() % 100});

	mcc_heap_iter_init(h1, &iter);
	for (int i; iter.next(&iter, &i);)
		printf("%d ", i);
	putchar('\n');

	while (!mcc_heap_is_empty(h1)) {
		int t;
		mcc_heap_peek(h1, &t);
		printf("%d ", t);
		mcc_heap_pop(h1);
	}
	putchar('\n');

	mcc_heap_delete(h1);
	return 0;
}
