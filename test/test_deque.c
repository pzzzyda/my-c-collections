#include "fruit.h"
#include "mcc_deque.h"
#include <assert.h>

static bool equals(struct mcc_deque *d, int *a)
{
	int *ref;

	for (size_t i = 0, len = mcc_deque_len(d); i < len; i++) {
		assert(!mcc_deque_get(d, i, (void **)&ref));
		if (*ref != a[i])
			return false;
	}
	return true;
}

static void test_push_and_pop()
{
	struct mcc_deque *d = mcc_deque_new(mcc_int());
	assert(d != NULL);
	assert(!mcc_deque_push_back(d, &(int){0}));
	assert(!mcc_deque_push_back(d, &(int){1}));
	assert(!mcc_deque_push_back(d, &(int){2}));
	assert(!mcc_deque_push_back(d, &(int){3}));
	assert(!mcc_deque_push_back(d, &(int){4}));
	assert(!mcc_deque_push_front(d, &(int){5}));
	assert(!mcc_deque_push_front(d, &(int){6}));
	assert(!mcc_deque_push_front(d, &(int){7}));
	assert(equals(d, (int[]){7, 6, 5, 0, 1, 2, 3, 4}));
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	assert(equals(d, (int[]){7, 6, 5, 0, 1, 2}));
	mcc_deque_push_back(d, &(int){8});
	mcc_deque_push_front(d, &(int){9});
	assert(equals(d, (int[]){9, 7, 6, 5, 0, 1, 2, 8}));
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	mcc_deque_pop_back(d);
	assert(equals(d, (int[]){9}));
	mcc_deque_drop(d);
}

static void test_insert_and_remove()
{
	struct mcc_deque *d = mcc_deque_new(mcc_int());
	assert(d != NULL);
	assert(!mcc_deque_insert(d, 0, &(int){0}));
	assert(!mcc_deque_insert(d, 0, &(int){1}));
	assert(!mcc_deque_insert(d, 1, &(int){2}));
	assert(!mcc_deque_insert(d, 1, &(int){3}));
	assert(!mcc_deque_insert(d, 2, &(int){4}));
	assert(!mcc_deque_insert(d, 3, &(int){5}));
	assert(!mcc_deque_insert(d, 3, &(int){6}));
	assert(!mcc_deque_insert(d, 5, &(int){7}));
	assert(equals(d, (int[]){1, 3, 4, 6, 5, 7, 2, 0}));
	mcc_deque_remove(d, 5);
	assert(equals(d, (int[]){1, 3, 4, 6, 5, 2, 0}));
	mcc_deque_remove(d, 2);
	assert(equals(d, (int[]){1, 3, 6, 5, 2, 0}));
	mcc_deque_remove(d, 5);
	assert(equals(d, (int[]){1, 3, 6, 5, 2}));
	assert(!mcc_deque_insert(d, 3, &(int){8}));
	assert(equals(d, (int[]){1, 3, 6, 8, 5, 2}));
	assert(!mcc_deque_insert(d, mcc_deque_len(d), &(int){9}));
	assert(equals(d, (int[]){1, 3, 6, 8, 5, 2, 9}));
	assert(!mcc_deque_insert(d, mcc_deque_len(d), &(int){10}));
	assert(!mcc_deque_insert(d, mcc_deque_len(d), &(int){11}));
	assert(!mcc_deque_insert(d, mcc_deque_len(d), &(int){12}));
	assert(equals(d, (int[]){1, 3, 6, 8, 5, 2, 9, 10, 11, 12}));
	mcc_deque_drop(d);
	d = mcc_deque_new(mcc_int());
	assert(d != NULL);
	assert(!mcc_deque_push_back(d, &(int){0}));
	assert(!mcc_deque_push_back(d, &(int){1}));
	assert(!mcc_deque_push_back(d, &(int){2}));
	assert(!mcc_deque_push_back(d, &(int){3}));
	assert(!mcc_deque_push_front(d, &(int){4}));
	assert(!mcc_deque_push_front(d, &(int){5}));
	assert(!mcc_deque_push_front(d, &(int){6}));
	assert(!mcc_deque_push_front(d, &(int){7}));
	assert(!mcc_deque_insert(d, 3, &(int){8}));
	assert(equals(d, (int[]){7, 6, 5, 8, 4, 0, 1, 2, 3}));
	mcc_deque_drop(d);
}

static void test_drop_call()
{
	struct fruit tmp;
	struct mcc_deque *d = mcc_deque_new(&fruit_);
	assert(d != NULL);
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Orange")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Watermelon")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Apple")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Pear")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Pineapple")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Banana")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Grape")));
	assert(!mcc_deque_push_back(d, fruit_new(&tmp, "Strawberry")));
	putchar('\t');
	mcc_deque_pop_back(d);
	putchar('\t');
	mcc_deque_pop_back(d);
	putchar('\t');
	mcc_deque_remove(d, 3);
	assert(!mcc_deque_sort(d));
	assert(!mcc_deque_reverse(d));
	mcc_deque_drop(d);
}

static void test_iterator()
{
	struct mcc_deque_iter *iter;
	int *elem;
	size_t i;
	int a[] = {15, 14, 13, 12, 11, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	struct mcc_deque *d = mcc_deque_new(mcc_int());
	assert(d != NULL);
	assert(!mcc_deque_push_back(d, &(int){0}));
	assert(!mcc_deque_push_back(d, &(int){1}));
	assert(!mcc_deque_push_back(d, &(int){2}));
	assert(!mcc_deque_push_back(d, &(int){3}));
	assert(!mcc_deque_push_back(d, &(int){4}));
	assert(!mcc_deque_push_back(d, &(int){5}));
	assert(!mcc_deque_push_back(d, &(int){6}));
	assert(!mcc_deque_push_back(d, &(int){7}));
	assert(!mcc_deque_push_back(d, &(int){8}));
	assert(!mcc_deque_push_back(d, &(int){9}));
	assert(!mcc_deque_push_front(d, &(int){10}));
	assert(!mcc_deque_push_front(d, &(int){11}));
	assert(!mcc_deque_push_front(d, &(int){12}));
	assert(!mcc_deque_push_front(d, &(int){13}));
	assert(!mcc_deque_push_front(d, &(int){14}));
	assert(!mcc_deque_push_front(d, &(int){15}));
	iter = mcc_deque_iter_new(d);
	assert(iter != NULL);
	for (i = 0; mcc_deque_iter_next(iter, (void **)&elem); i++)
		assert(a[i] == *elem);
	mcc_deque_drop(d);
}

int main(void)
{
	test_push_and_pop();
	test_insert_and_remove();
	test_drop_call();
	test_iterator();
	puts("testing done");
	return 0;
}
