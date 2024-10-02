#include "mcc_vector.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_int_vector(struct mcc_vector *vec, const char *fmt, ...)
{
	struct mcc_vector_iter iter;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_vector_iter_init(vec, &iter);
	printf("[ ");
	for (int i; mcc_iter_next(&iter, &i); i++)
		printf("%d ", i);
	printf("]\n");
}

int test_push_and_pop()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_vector *v = mcc_vector_new(INT);

	mcc_vector_push(v, &(int){0});
	mcc_vector_push(v, &(int){1});
	mcc_vector_push(v, &(int){2});
	mcc_vector_push(v, &(int){3});
	mcc_vector_push(v, &(int){4});
	mcc_vector_push(v, &(int){5});
	mcc_vector_push(v, &(int){6});
	mcc_vector_push(v, &(int){7});
	print_int_vector(v, "Raw data:\n");

	mcc_vector_pop(v);
	print_int_vector(v, "Pop:\n");

	mcc_vector_pop(v);
	print_int_vector(v, "Pop:\n");

	mcc_vector_push(v, &(int){8});
	print_int_vector(v, "Push 8:\n");

	mcc_vector_pop(v);
	print_int_vector(v, "Pop:\n");

	mcc_vector_push(v, &(int){9});
	print_int_vector(v, "Push 9:\n");

	mcc_vector_delete(v);
	puts("testing done");
	return 0;
}

int test_insert_and_remove()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_vector *v = mcc_vector_new(INT);

	for (int i = 0; i < 10; i++)
		mcc_vector_push(v, &i);
	print_int_vector(v, "Raw data:\n");

	mcc_vector_insert(v, 9, &(int){0});
	print_int_vector(v, "Insert 0 at 9:\n");

	mcc_vector_insert(v, 0, &(int){0});
	print_int_vector(v, "Insert 0 at 0:\n");

	mcc_vector_insert(v, mcc_vector_len(v), &(int){0});
	print_int_vector(v, "Insert 0 at the end:\n");

	mcc_vector_remove(v, 8);
	print_int_vector(v, "Remove element at 8:\n");

	mcc_vector_insert(v, 2, &(int){0});
	print_int_vector(v, "Insert 0 at 2:\n");

	mcc_vector_remove(v, 6);
	print_int_vector(v, "Remove element at 6:\n");

	mcc_vector_insert(v, 5, &(int){0});
	print_int_vector(v, "Insert 0 at 5:\n");

	mcc_vector_delete(v);
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

void print_fruit_vector(struct mcc_vector *vec, const char *fmt, ...)
{
	struct mcc_vector_iter iter;
	struct fruit elem;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_vector_iter_init(vec, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("{ .name = %s, .color = %d }\n", elem.name, elem.color);
}

int test_automatic_destructor_call()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_vector *v = mcc_vector_new(&fruit_obj_intf);

	mcc_vector_push(v, &(struct fruit){strdup("Orange"), 0});
	mcc_vector_push(v, &(struct fruit){strdup("Watermelon"), 1});
	mcc_vector_push(v, &(struct fruit){strdup("Apple"), 2});
	mcc_vector_push(v, &(struct fruit){strdup("Pear"), 3});
	mcc_vector_push(v, &(struct fruit){strdup("Pineapple"), 4});
	mcc_vector_push(v, &(struct fruit){strdup("Banana"), 5});
	mcc_vector_push(v, &(struct fruit){strdup("Grape"), 6});
	mcc_vector_push(v, &(struct fruit){strdup("Strawberry"), 7});
	print_fruit_vector(v, "Before sorting (by name):\n");

	mcc_vector_sort(v);
	print_fruit_vector(v, "After sorting (by name):\n");

	mcc_vector_delete(v);
	puts("testing done");
	return 0;
}

int test_storage_container()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_vector_iter iter;
	struct mcc_vector *elem;
	size_t i;
	struct mcc_vector *v = mcc_vector_new(MCC_VECTOR);

	for (i = 0; i < 10; i++) {
		elem = mcc_vector_new(INT);
		mcc_vector_push(v, &elem);
	}

	srand(time(NULL));
	mcc_vector_iter_init(v, &iter);
	for (; mcc_iter_next(&iter, &elem);) {
		mcc_vector_push(elem, &(int){rand() % 100});
		mcc_vector_push(elem, &(int){rand() % 100});
		mcc_vector_push(elem, &(int){rand() % 100});
		mcc_vector_push(elem, &(int){rand() % 100});
		mcc_vector_push(elem, &(int){rand() % 100});
	}

	mcc_vector_iter_init(v, &iter);
	for (i = 0; mcc_iter_next(&iter, &elem); i++) {
		mcc_vector_sort(elem);
		print_int_vector(elem, "The %lu element: ", i);
	}

	mcc_vector_delete(v);
	puts("testing done");
	return 0;
}

int test_iterator()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_vector_iter iter;
	int elem;
	struct mcc_vector *v = mcc_vector_new(INT);

	for (int i = 0; i < 20; i++)
		mcc_vector_push(v, &(int){i * 2});

	mcc_vector_iter_init(v, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	mcc_vector_delete(v);
	puts("testing done");
	return 0;
}

int main(void)
{
	test_push_and_pop();
	test_insert_and_remove();
	test_automatic_destructor_call();
	test_storage_container();
	test_iterator();
	return 0;
}
