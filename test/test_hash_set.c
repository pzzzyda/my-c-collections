#include "mcc_hash_set.h"
#include <stdio.h>

int main(void)
{
	struct mcc_hash_set_iter iter;
	int elem;
	struct mcc_hash_set *set = mcc_hash_set_new(INT);

	mcc_hash_set_insert(set, &(int){34});
	mcc_hash_set_insert(set, &(int){62});
	mcc_hash_set_insert(set, &(int){19});
	mcc_hash_set_insert(set, &(int){22});
	mcc_hash_set_insert(set, &(int){30});
	mcc_hash_set_insert(set, &(int){75});
	mcc_hash_set_insert(set, &(int){93});
	mcc_hash_set_insert(set, &(int){57});
	mcc_hash_set_insert(set, &(int){25});

	puts("Raw data");
	mcc_hash_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	puts("Removed 30");
	mcc_hash_set_remove(set, &(int){30});
	mcc_hash_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	puts("Removed 93");
	mcc_hash_set_remove(set, &(int){93});
	mcc_hash_set_iter_init(set, &iter);
	while (mcc_iter_next(&iter, &elem))
		printf("%d ", elem);
	putchar('\n');

	mcc_hash_set_delete(set);
	return 0;
}
