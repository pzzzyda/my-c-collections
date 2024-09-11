#include "mcc_deque.h"
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

bool is_ordered(struct mcc_deque *d)
{
	struct mcc_deque_iter iter;
	int a, b;

	mcc_deque_iter_init(d, &iter);
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
	struct mcc_deque *d1;
	struct mcc_deque *d2;
	struct mcc_deque *d3;
	struct mcc_deque *d4;
	struct mcc_deque_iter iter;

	d1 = mcc_deque_new(MCC_BASICS(int));

	mcc_deque_push_back(d1, &(int){0});
	mcc_deque_push_back(d1, &(int){1});
	mcc_deque_push_back(d1, &(int){2});
	mcc_deque_push_back(d1, &(int){3});
	mcc_deque_push_front(d1, &(int){0});
	mcc_deque_push_front(d1, &(int){1});
	mcc_deque_push_front(d1, &(int){2});
	mcc_deque_push_front(d1, &(int){3});

	mcc_deque_iter_init(d1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_deque_insert(d1, 3, &(int){30});
	mcc_deque_iter_init(d1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_deque_remove(d1, 5);
	mcc_deque_iter_init(d1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	mcc_deque_insert(d1, 2, &(int){20});
	mcc_deque_iter_init(d1, &iter);
	for (int i; iter.next(&iter, &i); i++)
		printf("%d ", i);
	putchar('\n');

	d2 = mcc_deque_new(MCC_OBJECT(fruit));

	mcc_deque_push_back(d2, &(struct fruit){strdup("A"), 0});
	mcc_deque_push_back(d2, &(struct fruit){strdup("B"), 0});
	mcc_deque_push_back(d2, &(struct fruit){strdup("C"), 0});
	mcc_deque_push_front(d2, &(struct fruit){strdup("D"), 0});
	mcc_deque_push_front(d2, &(struct fruit){strdup("E"), 0});
	mcc_deque_push_front(d2, &(struct fruit){strdup("F"), 0});
	mcc_deque_insert(d2, 2, &(struct fruit){strdup("G"), 0});
	mcc_deque_remove(d2, 4);

	mcc_deque_iter_init(d2, &iter);
	for (struct fruit f; iter.next(&iter, &f);)
		printf("name: %s, color: %d\n", f.name, f.color);

	d3 = mcc_deque_new(MCC_BASICS(int));

	srand(time(NULL));
	for (size_t i = 0; i < 51894; i++)
		mcc_deque_push_back(d3, &(int){rand()});
	for (size_t i = 0; i < 78123; i++)
		mcc_deque_push_front(d3, &(int){rand()});

	mcc_deque_sort(d3, cmp_int);

	if (is_ordered(d3))
		puts("OK");
	else
	 	puts("ERR");

	d4 = mcc_deque_new(MCC_DEQUE);

	for (size_t i = 0; i < 5; i++) {
		struct mcc_deque *t;
		t = mcc_deque_new(MCC_BASICS(int));
		mcc_deque_push_back(d4, &t);
	}

	mcc_deque_iter_init(d4, &iter);
	for (struct mcc_deque *t; iter.next(&iter, &t);) {
		mcc_deque_push_back(t, &(int){0});
		mcc_deque_push_back(t, &(int){1});
		mcc_deque_push_front(t, &(int){2});
		mcc_deque_push_front(t, &(int){3});
	}

	mcc_deque_iter_init(d4, &iter);
	for (struct mcc_deque *tmp; iter.next(&iter, &tmp);) {
		size_t len = mcc_deque_len(tmp);
		for (size_t i = 0;i < len; i++) {
			int t;
			mcc_deque_get(tmp, i, &t);
			printf("%d ", t);
		}
		putchar('\n');
	}

	mcc_deque_delete(d1);
	mcc_deque_delete(d2);
	mcc_deque_delete(d3);
	mcc_deque_delete(d4);
	return 0;
}
