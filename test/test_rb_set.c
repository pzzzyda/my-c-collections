#include "mcc_rb_set.h"
#include <stdio.h>

int main(void)
{
	struct mcc_rb_set_iter iter;
	int elem;
	struct mcc_rb_set *set = mcc_rb_set_new(INT);

	mcc_rb_set_insert(set, &(int){9});
	mcc_rb_set_insert(set, &(int){3});
	mcc_rb_set_insert(set, &(int){7});
	mcc_rb_set_insert(set, &(int){12});
	mcc_rb_set_insert(set, &(int){8});
	mcc_rb_set_insert(set, &(int){2});
	mcc_rb_set_insert(set, &(int){10});
	mcc_rb_set_insert(set, &(int){4});
	mcc_rb_set_insert(set, &(int){0});
	mcc_rb_set_insert(set, &(int){11});
	mcc_rb_set_insert(set, &(int){5});
	mcc_rb_set_insert(set, &(int){6});
	mcc_rb_set_insert(set, &(int){1});

	puts("Raw data");
	mcc_rb_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	puts("Removed 5");
	mcc_rb_set_remove(set, &(int){5});
	mcc_rb_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	puts("Removed 10");
	mcc_rb_set_remove(set, &(int){10});
	mcc_rb_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	puts("Removed 3");
	mcc_rb_set_remove(set, &(int){3});
	mcc_rb_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	mcc_rb_set_delete(set);
	return 0;
}
