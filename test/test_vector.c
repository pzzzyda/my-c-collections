#include "mcc_vector.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	.name = "struct fruit",
	.size = sizeof(struct fruit),
	.dtor = &fruit_dtor,
	.cmp = NULL,
	.hash = NULL,
};

static void print_int_vector(struct mcc_vector *d, const char *fmt, ...)
{
	struct mcc_vector_iter iter;

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_vector_iter_init(d, &iter);
	for (int i; mcc_vector_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');
}

static int test_store_int(void)
{
	struct mcc_vector *v;

	v = mcc_vector_new(&mcc_i32_i);

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

	mcc_vector_insert(v, 2, &(int){9});
	print_int_vector(v, "Insert 9 at 2:\n");

	mcc_vector_insert(v, 5, &(int){9});
	print_int_vector(v, "Insert 9 at 5:\n");

	mcc_vector_reverse(v);
	print_int_vector(v, "Reverse\n");

	mcc_vector_delete(v);

	return 0;
}

static int test_store_fruit(void)
{
	struct mcc_vector *v;
	struct mcc_vector_iter iter;

	v = mcc_vector_new(&fruit_i);

	mcc_vector_push(v, &(struct fruit){strdup("A"), 0});
	mcc_vector_push(v, &(struct fruit){strdup("B"), 1});
	mcc_vector_push(v, &(struct fruit){strdup("C"), 2});
	mcc_vector_push(v, &(struct fruit){strdup("D"), 3});
	mcc_vector_push(v, &(struct fruit){strdup("E"), 4});
	mcc_vector_push(v, &(struct fruit){strdup("F"), 5});

	mcc_vector_iter_init(v, &iter);
	for (struct fruit f; mcc_vector_iter_next(&iter, &f);)
		printf("name: %s, color: %d\n", f.name, f.color);

	mcc_vector_delete(v);

	return 0;
}

static int test_store_int_vector(void)
{
	struct mcc_vector *v, *tmp;
	struct mcc_vector_iter iter;

	v = mcc_vector_new(&mcc_vector_i);

	for (size_t i = 0; i < 3; i++) {
		tmp = mcc_vector_new(&mcc_i32_i);
		mcc_vector_push(v, &tmp);
	}

	mcc_vector_iter_init(v, &iter);
	for (; mcc_vector_iter_next(&iter, &tmp);) {
		mcc_vector_push(tmp, &(int){0});
		mcc_vector_push(tmp, &(int){1});
		mcc_vector_push(tmp, &(int){2});
		mcc_vector_push(tmp, &(int){3});
	}

	mcc_vector_iter_init(v, &iter);
	for (; mcc_vector_iter_next(&iter, &tmp);) {
		size_t len = mcc_vector_len(tmp);
		for (size_t i = 0; i < len; i++) {
			int t;
			mcc_vector_get(tmp, i, &t);
			printf("%d ", t);
		}
		putchar('\n');
	}

	mcc_vector_delete(v);

	return 0;
}

int main(int argc, char **argv)
{
	test_store_int();
	test_store_fruit();
	test_store_int_vector();
	return 0;
}
