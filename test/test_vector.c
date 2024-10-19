#include "fruit.h"
#include "mcc_vector.h"
#include <assert.h>

static bool equals(struct mcc_vector *v, int *a)
{
	int *ref;

	for (size_t i = 0, len = mcc_vector_len(v); i < len; i++) {
		assert(!mcc_vector_get(v, i, (void **)&ref));
		if (*ref != a[i])
			return false;
	}
	return true;
}

static void test_push_and_pop()
{
	struct mcc_vector *v = mcc_vector_new(mcc_int());
	assert(v != NULL);
	assert(!mcc_vector_push(v, &(int){0}));
	assert(!mcc_vector_push(v, &(int){1}));
	assert(!mcc_vector_push(v, &(int){2}));
	assert(!mcc_vector_push(v, &(int){3}));
	assert(!mcc_vector_push(v, &(int){4}));
	assert(!mcc_vector_push(v, &(int){5}));
	assert(!mcc_vector_push(v, &(int){6}));
	assert(!mcc_vector_push(v, &(int){7}));
	assert(equals(v, (int[]){0, 1, 2, 3, 4, 5, 6, 7}));
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	assert(equals(v, (int[]){0, 1, 2, 3, 4, 5}));
	mcc_vector_push(v, &(int){8});
	mcc_vector_push(v, &(int){9});
	assert(equals(v, (int[]){0, 1, 2, 3, 4, 5, 8, 9}));
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	mcc_vector_pop(v);
	assert(equals(v, (int[]){0}));
	mcc_vector_drop(v);
}

static void test_insert_and_remove()
{
	struct mcc_vector *v = mcc_vector_new(mcc_int());
	assert(v != NULL);
	assert(!mcc_vector_insert(v, 0, &(int){0}));
	assert(!mcc_vector_insert(v, mcc_vector_len(v) - 1, &(int){1}));
	assert(!mcc_vector_insert(v, 1, &(int){2}));
	assert(!mcc_vector_insert(v, 1, &(int){3}));
	assert(!mcc_vector_insert(v, 2, &(int){4}));
	assert(!mcc_vector_insert(v, 3, &(int){5}));
	assert(!mcc_vector_insert(v, 3, &(int){6}));
	assert(!mcc_vector_insert(v, 5, &(int){7}));
	assert(equals(v, (int[]){1, 3, 4, 6, 5, 7, 2, 0}));
	mcc_vector_remove(v, 5);
	assert(equals(v, (int[]){1, 3, 4, 6, 5, 2, 0}));
	mcc_vector_remove(v, 2);
	assert(equals(v, (int[]){1, 3, 6, 5, 2, 0}));
	mcc_vector_remove(v, 5);
	assert(equals(v, (int[]){1, 3, 6, 5, 2}));
	assert(!mcc_vector_insert(v, 3, &(int){8}));
	assert(equals(v, (int[]){1, 3, 6, 8, 5, 2}));
	assert(!mcc_vector_insert(v, mcc_vector_len(v), &(int){9}));
	assert(equals(v, (int[]){1, 3, 6, 8, 5, 2, 9}));
	mcc_vector_drop(v);
}

static void test_drop_call()
{
	struct mcc_vector *v = mcc_vector_new(&fruit_);
	assert(v != NULL);
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Orange")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Watermelon")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Apple")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Pear")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Pineapple")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Banana")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Grape")));
	assert(!mcc_vector_push(v, fruit_new(&(struct fruit){}, "Strawberry")));
	putchar('\t');
	mcc_vector_pop(v);
	putchar('\t');
	mcc_vector_pop(v);
	putchar('\t');
	mcc_vector_remove(v, 3);
	assert(!mcc_vector_sort(v));
	assert(!mcc_vector_reverse(v));
	mcc_vector_drop(v);
}

static void test_iterator()
{
	struct mcc_vector_iter *iter;
	int *elem;
	size_t i;
	int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	struct mcc_vector *v = mcc_vector_new(mcc_int());
	assert(v != NULL);
	assert(!mcc_vector_push(v, &(int){0}));
	assert(!mcc_vector_push(v, &(int){1}));
	assert(!mcc_vector_push(v, &(int){2}));
	assert(!mcc_vector_push(v, &(int){3}));
	assert(!mcc_vector_push(v, &(int){4}));
	assert(!mcc_vector_push(v, &(int){5}));
	assert(!mcc_vector_push(v, &(int){6}));
	assert(!mcc_vector_push(v, &(int){7}));
	assert(!mcc_vector_push(v, &(int){8}));
	assert(!mcc_vector_push(v, &(int){9}));
	assert(!mcc_vector_push(v, &(int){10}));
	assert(!mcc_vector_push(v, &(int){11}));
	assert(!mcc_vector_push(v, &(int){12}));
	assert(!mcc_vector_push(v, &(int){13}));
	assert(!mcc_vector_push(v, &(int){14}));
	assert(!mcc_vector_push(v, &(int){15}));
	iter = mcc_vector_iter_new(v);
	assert(iter != NULL);
	for (i = 0; mcc_vector_iter_next(iter, (void **)&elem); i++)
		assert(a[i] == *elem);
	mcc_vector_drop(v);
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
