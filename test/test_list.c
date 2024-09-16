#include "mcc_list.h"
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
	.name = "struct fruit",
	.size = sizeof(struct fruit),
	.dtor = &fruit_dtor,
};

static int test_store_int(void)
{
	struct mcc_list *l;
	struct mcc_list_iter iter;

	l = mcc_list_new(&mcc_i32_i);

	mcc_list_push_back(l, &(int){0});
	mcc_list_push_back(l, &(int){1});
	mcc_list_push_back(l, &(int){2});
	mcc_list_push_back(l, &(int){3});
	mcc_list_push_front(l, &(int){0});
	mcc_list_push_front(l, &(int){1});
	mcc_list_push_front(l, &(int){2});
	mcc_list_push_front(l, &(int){3});

	mcc_list_iter_init(l, &iter);
	for (int i; mcc_list_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_insert(l, 3, &(int){30});
	mcc_list_iter_init(l, &iter);
	for (int i; mcc_list_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_remove(l, 5);
	mcc_list_iter_init(l, &iter);
	for (int i; mcc_list_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_insert(l, 2, &(int){20});
	mcc_list_iter_init(l, &iter);
	for (int i; mcc_list_iter_next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_delete(l);

	return 0;
}

static int test_store_fruit(void)
{
	struct mcc_list *l;
	struct mcc_list_iter iter;
	l = mcc_list_new(&fruit_i);

	mcc_list_push_back(l, &(struct fruit){strdup("A"), 0});
	mcc_list_push_back(l, &(struct fruit){strdup("B"), 0});
	mcc_list_push_back(l, &(struct fruit){strdup("C"), 0});
	mcc_list_push_front(l, &(struct fruit){strdup("D"), 0});
	mcc_list_push_front(l, &(struct fruit){strdup("E"), 0});
	mcc_list_push_front(l, &(struct fruit){strdup("F"), 0});
	mcc_list_insert(l, 2, &(struct fruit){strdup("G"), 0});
	mcc_list_remove(l, 4);

	mcc_list_iter_init(l, &iter);
	for (struct fruit f; mcc_list_iter_next(&iter, &f);)
		printf("name: %s, color: %d\n", f.name, f.color);

	mcc_list_delete(l);

	return 0;
}

static bool is_ordered(struct mcc_list *l)
{
	struct mcc_list_iter iter;
	int a, b;

	mcc_list_iter_init(l, &iter);
	mcc_list_iter_next(&iter, &a);
	while (mcc_list_iter_next(&iter, &b)) {
		if (b < a)
			return false;
		a = b;
	}
	return true;
}

static int test_sort_int_list(void)
{
	struct mcc_list *l;
	l = mcc_list_new(&mcc_i32_i);

	srand(time(NULL));
	for (size_t i = 0; i < 51431; i++)
		mcc_list_push_back(l, &(int){rand()});
	for (size_t i = 0; i < 78413; i++)
		mcc_list_push_front(l, &(int){rand()});

	mcc_list_sort(l);

	if (is_ordered(l))
		puts("OK");
	else
		puts("ERR");

	mcc_list_delete(l);

	return 0;
}

int main(int argc, char **argv)
{
	test_store_int();
	test_store_fruit();
	test_sort_int_list();
	return 0;
}
