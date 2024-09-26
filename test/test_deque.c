#include "mcc_deque.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct fruit {
	char *name;
	int color;
};

static void fruit_dtor(void *ptr)
{
	struct fruit *self = ptr;

	if (self->name) {
		printf("~destruct: %s\n", self->name);
		free(self->name);
	}
}

static const struct mcc_object_interface fruit_i = {
	.size = sizeof(struct fruit),
	.dtor = &fruit_dtor,
	.cmp = NULL,
	.hash = NULL,
};

static void print_int_deque(struct mcc_deque *d, const char *fmt, ...)
{
	struct mcc_deque_iter iter;

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_deque_iter_init(d, &iter);
	for (int i; mcc_deque_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');
}

static int test_store_int(void)
{
	struct mcc_deque *d;

	d = mcc_deque_new(&mcc_i32_i);

	mcc_deque_push_back(d, &(int){0});
	mcc_deque_push_back(d, &(int){1});
	mcc_deque_push_back(d, &(int){2});
	mcc_deque_push_back(d, &(int){3});
	mcc_deque_push_front(d, &(int){0});
	mcc_deque_push_front(d, &(int){1});
	mcc_deque_push_front(d, &(int){2});
	mcc_deque_push_front(d, &(int){3});
	print_int_deque(d, "Raw data:\n");

	mcc_deque_insert(d, 3, &(int){30});
	print_int_deque(d, "Insert 30 at 3:\n");

	mcc_deque_remove(d, 5);
	print_int_deque(d, "Remove element at 5:\n");

	mcc_deque_insert(d, 2, &(int){20});
	print_int_deque(d, "Insert 20 at 2:\n");

	mcc_deque_delete(d);

	return 0;
}

static int test_store_fruit(void)
{
	struct mcc_deque *d;

	d = mcc_deque_new(&fruit_i);

	mcc_deque_push_back(d, &(struct fruit){strdup("A"), 0});
	mcc_deque_push_back(d, &(struct fruit){strdup("B"), 0});
	mcc_deque_push_back(d, &(struct fruit){strdup("C"), 0});
	mcc_deque_push_front(d, &(struct fruit){strdup("D"), 0});
	mcc_deque_push_front(d, &(struct fruit){strdup("E"), 0});
	mcc_deque_push_front(d, &(struct fruit){strdup("F"), 0});

	mcc_deque_insert(d, 2, &(struct fruit){strdup("G"), 0});

	mcc_deque_remove(d, 4);

	mcc_deque_delete(d);
	return 0;
}

static bool is_ordered(struct mcc_deque *d)
{
	struct mcc_deque_iter iter;
	int a, b;

	mcc_deque_iter_init(d, &iter);
	mcc_deque_iter_next(&iter, &a);
	while (mcc_deque_iter_next(&iter, &b)) {
		if (b < a)
			return false;
		a = b;
	}
	return true;
}

static int test_sort_int_deque(void)
{
	struct mcc_deque *d;

	d = mcc_deque_new(&mcc_i32_i);

	srand(time(NULL));
	for (size_t i = 0; i < 51894; i++)
		mcc_deque_push_back(d, &(int){rand()});
	for (size_t i = 0; i < 78123; i++)
		mcc_deque_push_front(d, &(int){rand()});

	mcc_deque_sort(d);

	if (is_ordered(d))
		puts("OK");
	else
		puts("ERR");

	mcc_deque_delete(d);

	return 0;
}

static int test_store_int_deque(void)
{
	struct mcc_deque *d, *tmp;
	struct mcc_deque_iter iter;

	d = mcc_deque_new(&mcc_deque_i);

	for (size_t i = 0; i < 5; i++) {
		tmp = mcc_deque_new(&mcc_i32_i);
		mcc_deque_push_back(d, &tmp);
	}

	mcc_deque_iter_init(d, &iter);
	while (mcc_deque_iter_next(&iter, &tmp)) {
		mcc_deque_push_back(tmp, &(int){0});
		mcc_deque_push_back(tmp, &(int){1});
		mcc_deque_push_front(tmp, &(int){2});
		mcc_deque_push_front(tmp, &(int){3});
	}

	mcc_deque_iter_init(d, &iter);
	for (size_t i = 0; mcc_deque_iter_next(&iter, &tmp); i++)
		print_int_deque(tmp, "[%ld]:", i);

	mcc_deque_delete(d);
	return 0;
}

static int test_binary_search()
{
	struct mcc_deque *d;
	mcc_i32 n = 20;
	mcc_i32 i = 0;
	mcc_i32 *v;

	d = mcc_deque_new(&mcc_i32_i);

	for (; i < n / 2; i++) {
		mcc_deque_push_back(d, &i);
	}
	for (; i < n; i++) {
		mcc_deque_push_front(d, &i);
	}

	mcc_deque_sort(d);

	for (mcc_i32 j = 0; j < n; j++) {
		if (!(v = mcc_deque_binary_search(d, &j)))
			printf("No %d\n", j);
		else
			printf("%d\n", *v);
	}

	mcc_deque_delete(d);
	return 0;
}

static int test_insert_and_remove()
{
	struct mcc_deque *d;

	d = mcc_deque_new(&mcc_i32_i);

	mcc_deque_push_back(d, &(int){0});
	mcc_deque_push_back(d, &(int){1});
	mcc_deque_push_back(d, &(int){2});
	mcc_deque_push_back(d, &(int){3});
	mcc_deque_push_front(d, &(int){0});
	mcc_deque_push_front(d, &(int){1});
	mcc_deque_push_front(d, &(int){2});
	mcc_deque_push_front(d, &(int){3});
	print_int_deque(d, "Raw data:\n");

	mcc_deque_insert(d, 7, &(int){100});
	print_int_deque(d, "Insert 100 at 7:\n");

	mcc_deque_insert(d, 0, &(int){100});
	print_int_deque(d, "Insert 100 at 0:\n");

	mcc_deque_remove(d, 5);
	print_int_deque(d, "Remove element at 5:\n");

	mcc_deque_insert(d, 2, &(int){100});
	print_int_deque(d, "Insert 100 at 2:\n");

	mcc_deque_remove(d, 1);
	print_int_deque(d, "Remove element at 1:\n");

	mcc_deque_insert(d, mcc_deque_len(d), &(int){100});
	print_int_deque(d, "Insert 100 at the end:\n");

	mcc_deque_insert(d, 1, &(int){100});
	print_int_deque(d, "Insert 100 at 1:\n");

	mcc_deque_delete(d);

	return 0;
}

int main(int argc, char **argv)
{
	test_store_int();
	test_store_fruit();
	test_sort_int_deque();
	test_store_int_deque();
	test_binary_search();
	test_insert_and_remove();
	return 0;
}
