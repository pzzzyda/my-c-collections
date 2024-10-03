#include "mcc_list.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_int_list(struct mcc_list *list, const char *fmt, ...)
{
	struct mcc_list_iter iter;
	int elem;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_list_iter_init(list, &iter);
	while (mcc_iter_next(&iter, &elem)) {
		printf("%d ", elem);
	}
	putchar('\n');
}

int test_push_and_pop()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_list *l = mcc_list_new(INT);

	mcc_list_push_back(l, &(int){0});
	mcc_list_push_back(l, &(int){1});
	mcc_list_push_back(l, &(int){2});
	mcc_list_push_back(l, &(int){3});
	mcc_list_push_front(l, &(int){0});
	mcc_list_push_front(l, &(int){1});
	mcc_list_push_front(l, &(int){2});
	mcc_list_push_front(l, &(int){3});
	print_int_list(l, "Raw data:\n");

	mcc_list_push_back(l, &(int){10});
	print_int_list(l, "Push 10 in back:\n");

	mcc_list_pop_front(l);
	print_int_list(l, "Pop front:\n");

	mcc_list_push_back(l, &(int){10});
	print_int_list(l, "Push 10 in back:\n");

	mcc_list_pop_front(l);
	print_int_list(l, "Pop front:\n");

	mcc_list_push_front(l, &(int){10});
	print_int_list(l, "Push 10 in front:\n");

	mcc_list_pop_back(l);
	print_int_list(l, "Pop back:\n");

	mcc_list_delete(l);
	puts("testing done");
	return 0;
}

int test_insert_and_remove()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_list *l = mcc_list_new(INT);

	mcc_list_push_back(l, &(int){0});
	mcc_list_push_back(l, &(int){1});
	mcc_list_push_back(l, &(int){2});
	mcc_list_push_back(l, &(int){3});
	mcc_list_push_front(l, &(int){0});
	mcc_list_push_front(l, &(int){1});
	mcc_list_push_front(l, &(int){2});
	mcc_list_push_front(l, &(int){3});
	print_int_list(l, "Raw data:\n");

	mcc_list_insert(l, 7, &(int){100});
	print_int_list(l, "Insert 100 at 7:\n");

	mcc_list_insert(l, 0, &(int){100});
	print_int_list(l, "Insert 100 at 0:\n");

	mcc_list_remove(l, 5);
	print_int_list(l, "Remove element at 5:\n");

	mcc_list_insert(l, 2, &(int){100});
	print_int_list(l, "Insert 100 at 2:\n");

	mcc_list_remove(l, 1);
	print_int_list(l, "Remove element at 1:\n");

	mcc_list_insert(l, mcc_list_len(l), &(int){100});
	print_int_list(l, "Insert 100 at the end:\n");

	mcc_list_insert(l, 1, &(int){100});
	print_int_list(l, "Insert 100 at 1:\n");

	mcc_list_delete(l);
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

void print_fruit_list(struct mcc_list *list, const char *fmt, ...)
{
	struct mcc_list_iter iter;
	struct fruit elem;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_list_iter_init(list, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("{ .name = %s, .color = %d }\n", elem.name, elem.color);
}

int test_automatic_destructor_call()
{
	printf("%s\n", __FUNCTION__);
	struct mcc_list *l = mcc_list_new(&fruit_obj_intf);

	mcc_list_push_back(l, &(struct fruit){strdup("Orange"), 0});
	mcc_list_push_back(l, &(struct fruit){strdup("Watermelon"), 1});
	mcc_list_push_back(l, &(struct fruit){strdup("Apple"), 2});
	mcc_list_push_back(l, &(struct fruit){strdup("Pear"), 3});
	mcc_list_push_front(l, &(struct fruit){strdup("Pineapple"), 4});
	mcc_list_push_front(l, &(struct fruit){strdup("Banana"), 5});
	mcc_list_push_front(l, &(struct fruit){strdup("Grape"), 6});
	mcc_list_push_front(l, &(struct fruit){strdup("Strawberry"), 7});
	print_fruit_list(l, "Before sorting (by name):\n");

	mcc_list_sort(l);
	print_fruit_list(l, "After sorting (by name):\n");

	mcc_list_delete(l);
	puts("testing done");
	return 0;
}

bool is_ordered(struct mcc_list *list)
{
	struct mcc_list_iter iter;
	int a, b;

	mcc_list_iter_init(list, &iter);
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
	struct mcc_list *l = mcc_list_new(INT);

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
	puts("testing done");
	return 0;
}

int main(void)
{
	test_push_and_pop();
	test_insert_and_remove();
	test_automatic_destructor_call();
	test_sorting();
	return 0;
}
