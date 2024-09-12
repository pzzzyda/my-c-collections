#include "mcc_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct fruit {
	char *name;
	int color;
};

MCC_DESTRUCTOR(fruit, {
	if (self->name) {
		printf("~destruct: %s\n", self->name);
		free(self->name);
	}
})

int cmp_int(const void *a, const void *b)
{
	return *(const int *)a - *(const int *)b;
}

bool is_ordered(struct mcc_list *l)
{
	struct mcc_list_iter iter;
	int a, b;

	mcc_list_iter_init(l, &iter);
	iter.next(&iter, &a);
	while (iter.next(&iter, &b)) {
		if (b < a)
			return false;
		a = b;
	}
	return true;
}

int main()
{
	struct mcc_list *l1;
	struct mcc_list *l2;
	struct mcc_list *l3;
	struct mcc_list *l4;
	struct mcc_list_iter iter;

	l1 = mcc_list_new(MCC_BASICS(int));

	mcc_list_push_back(l1, &(int){0});
	mcc_list_push_back(l1, &(int){1});
	mcc_list_push_back(l1, &(int){2});
	mcc_list_push_back(l1, &(int){3});
	mcc_list_push_front(l1, &(int){0});
	mcc_list_push_front(l1, &(int){1});
	mcc_list_push_front(l1, &(int){2});
	mcc_list_push_front(l1, &(int){3});

	mcc_list_iter_init(l1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_insert(l1, 3, &(int){30});
	mcc_list_iter_init(l1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_remove(l1, 5);
	mcc_list_iter_init(l1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_list_insert(l1, 2, &(int){20});
	mcc_list_iter_init(l1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	l2 = mcc_list_new(MCC_OBJECT(fruit));

	mcc_list_push_back(l2, &(struct fruit){strdup("A"), 0});
	mcc_list_push_back(l2, &(struct fruit){strdup("B"), 0});
	mcc_list_push_back(l2, &(struct fruit){strdup("C"), 0});
	mcc_list_push_front(l2, &(struct fruit){strdup("D"), 0});
	mcc_list_push_front(l2, &(struct fruit){strdup("E"), 0});
	mcc_list_push_front(l2, &(struct fruit){strdup("F"), 0});
	mcc_list_insert(l2, 2, &(struct fruit){strdup("G"), 0});
	mcc_list_remove(l2, 4);

	mcc_list_iter_init(l2, &iter);
	for (struct fruit f; iter.next(&iter, &f);)
		printf("name: %s, color: %d\n", f.name, f.color);

	l3 = mcc_list_new(MCC_BASICS(int));

	srand(time(NULL));
	for (size_t i = 0; i < 51431; i++)
		mcc_list_push_back(l3, &(int){rand()});
	for (size_t i = 0; i < 78413; i++)
		mcc_list_push_front(l3, &(int){rand()});

	mcc_list_sort(l3, cmp_int);

	if (is_ordered(l3))
		puts("OK");
	else
		puts("ERR");

	l4 = mcc_list_new(MCC_LIST);

	for (size_t i = 0; i < 5; i++) {
		struct mcc_list *tmp;
		tmp = mcc_list_new(MCC_BASICS(int));
		mcc_list_push_back(l4, &tmp);
	}

	for (size_t i = 0; i < 5; i++) {
		struct mcc_list *tmp;
		tmp = mcc_list_new(MCC_BASICS(int));
		mcc_list_push_back(l4, &tmp);
	}

	mcc_list_iter_init(l4, &iter);
	for (struct mcc_list *tmp; iter.next(&iter, &tmp);) {
		mcc_list_push_back(tmp, &(int){0});
		mcc_list_push_back(tmp, &(int){1});
		mcc_list_push_back(tmp, &(int){2});
		mcc_list_push_front(tmp, &(int){0});
		mcc_list_push_front(tmp, &(int){1});
		mcc_list_push_front(tmp, &(int){2});
	}

	mcc_list_iter_init(l4, &iter);
	for (struct mcc_list *tmp; iter.next(&iter, &tmp);) {
		struct mcc_list_iter i;
		mcc_list_iter_init(tmp, &i);
		for (int t; i.next(&i, &t);)
			printf("%d ", t);
		putchar('\n');
	}

	mcc_list_delete(l1);
	mcc_list_delete(l2);
	mcc_list_delete(l3);
	mcc_list_delete(l4);
	return 0;
}
