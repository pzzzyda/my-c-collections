#include "mcc_deque.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_int_deque(struct mcc_deque *deq, const char *fmt, ...)
{
	struct mcc_deque_iter iter;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_deque_iter_init(deq, &iter);
	printf("[ ");
	for (int i; mcc_iter_next(&iter, &i); i++)
		printf("%d ", i);
	printf("]\n");
}

int test_push_and_pop()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque *d = mcc_deque_new(INT);

	mcc_deque_push_back(d, &(int){0});
	mcc_deque_push_back(d, &(int){1});
	mcc_deque_push_back(d, &(int){2});
	mcc_deque_push_back(d, &(int){3});
	mcc_deque_push_front(d, &(int){0});
	mcc_deque_push_front(d, &(int){1});
	mcc_deque_push_front(d, &(int){2});
	mcc_deque_push_front(d, &(int){3});
	print_int_deque(d, "Raw data:\n");

	mcc_deque_push_back(d, &(int){10});
	print_int_deque(d, "Push 10 in back:\n");

	mcc_deque_pop_front(d);
	print_int_deque(d, "Pop front:\n");

	mcc_deque_push_back(d, &(int){10});
	print_int_deque(d, "Push 10 in back:\n");

	mcc_deque_pop_front(d);
	print_int_deque(d, "Pop front:\n");

	mcc_deque_push_front(d, &(int){10});
	print_int_deque(d, "Push 10 in front:\n");

	mcc_deque_pop_back(d);
	print_int_deque(d, "Pop back:\n");

	mcc_deque_delete(d);
	puts("testing done");
	return 0;
}

int test_insert_and_remove()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque *d = mcc_deque_new(INT);

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
	puts("testing done");
	return 0;
}

struct fruit {
	char *name;
	int color;
};

void fruit_dtor(void *self)
{
	struct fruit *fruit = self;

	if (fruit->name) {
		printf("~destruct: %s\n", fruit->name);
		free(fruit->name);
	}
}

int fruit_cmp(const void *self, const void *other)
{
	const struct fruit *a = self;
	const struct fruit *b = other;

	return strcmp(a->name, b->name);
}

const struct mcc_object_interface fruit_obj_intf = {
	.size = sizeof(struct fruit),
	.dtor = &fruit_dtor,
	.cmp = &fruit_cmp,
	.hash = NULL,
};

void print_fruit_deque(struct mcc_deque *deq, const char *fmt, ...)
{
	struct mcc_deque_iter iter;
	struct fruit elem;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_deque_iter_init(deq, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("{ .name = %s, .color = %d }\n", elem.name, elem.color);
}

int test_automatic_destructor_call()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque *d = mcc_deque_new(&fruit_obj_intf);

	mcc_deque_push_back(d, &(struct fruit){strdup("Orange"), 0});
	mcc_deque_push_back(d, &(struct fruit){strdup("Watermelon"), 1});
	mcc_deque_push_back(d, &(struct fruit){strdup("Apple"), 2});
	mcc_deque_push_back(d, &(struct fruit){strdup("Pear"), 3});
	mcc_deque_push_front(d, &(struct fruit){strdup("Pineapple"), 4});
	mcc_deque_push_front(d, &(struct fruit){strdup("Banana"), 5});
	mcc_deque_push_front(d, &(struct fruit){strdup("Grape"), 6});
	mcc_deque_push_front(d, &(struct fruit){strdup("Strawberry"), 7});
	print_fruit_deque(d, "Before sorting (by name):\n");

	mcc_deque_sort(d);
	print_fruit_deque(d, "After sorting (by name):\n");

	mcc_deque_delete(d);
	puts("testing done");
	return 0;
}

int test_storage_container()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque_iter iter;
	struct mcc_deque *elem;
	size_t i;
	struct mcc_deque *d = mcc_deque_new(MCC_DEQUE);

	for (i = 0; i < 10; i++) {
		elem = mcc_deque_new(INT);
		mcc_deque_push_back(d, &elem);
	}

	mcc_deque_iter_init(d, &iter);
	srand(time(NULL));
	while (mcc_iter_next(&iter, &elem)) {
		mcc_deque_push_back(elem, &(int){rand() % 100});
		mcc_deque_push_back(elem, &(int){rand() % 100});
		mcc_deque_push_back(elem, &(int){rand() % 100});
		mcc_deque_push_back(elem, &(int){rand() % 100});
		mcc_deque_push_back(elem, &(int){rand() % 100});
		mcc_deque_push_front(elem, &(int){rand() % 100});
		mcc_deque_push_front(elem, &(int){rand() % 100});
		mcc_deque_push_front(elem, &(int){rand() % 100});
	}

	mcc_deque_iter_init(d, &iter);
	for (i = 0; mcc_iter_next(&iter, &elem); i++) {
		mcc_deque_sort(elem);
		print_int_deque(elem, "The %lu element: ", i);
	}

	mcc_deque_delete(d);
	puts("testing done");
	return 0;
}

bool is_ordered(struct mcc_deque *deq)
{
	struct mcc_deque_iter iter;
	int a, b;

	mcc_deque_iter_init(deq, &iter);
	mcc_iter_next(&iter, &a);
	while (mcc_iter_next(&iter, &b)) {
		if (b < a)
			return false;
		a = b;
	}
	return true;
}

int test_sorting()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque *d = mcc_deque_new(INT);

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
	puts("testing done");
	return 0;
}

int test_binary_search()
{
	printf("%s\n", __FUNCTION__);
	int n = 20;
	int i = 0;
	int *v;
	struct mcc_deque *d = mcc_deque_new(INT);

	for (; i < n / 2; i++)
		mcc_deque_push_back(d, &i);
	for (; i < n; i++)
		mcc_deque_push_front(d, &i);

	mcc_deque_sort(d);

	for (int j = 0; j < n; j++) {
		if (!(v = mcc_deque_binary_search(d, &j)))
			printf("No %d\n", j);
		else
			printf("%d\n", *v);
	}

	mcc_deque_delete(d);
	puts("testing done");
	return 0;
}

int test_iterator()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_deque_iter iter;
	int elem;
	struct mcc_deque *d = mcc_deque_new(INT);

	for (int i = 0; i < 10; i++)
		mcc_deque_push_front(d, &(int){i * 2});
	for (int i = 0; i < 10; i++)
		mcc_deque_push_back(d, &(int){i * 2});

	mcc_deque_iter_init(d, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	mcc_deque_delete(d);
	puts("testing done");
	return 0;
}

int main(void)
{
	test_push_and_pop();
	test_insert_and_remove();
	test_automatic_destructor_call();
	test_storage_container();
	test_sorting();
	test_binary_search();
	test_iterator();
	return 0;
}
