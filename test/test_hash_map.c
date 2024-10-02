#include "mcc_hash_map.h"
#include "mcc_utils.h"
#include "mcc_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int test_str_key_int_value(void)
{
	struct mcc_hash_map *map;
	struct mcc_hash_map_iter iter;

	map = mcc_hash_map_new(STR, INT);

	mcc_hash_map_insert(map, &(mcc_str_t){"Apple"}, &(int){0});
	mcc_hash_map_insert(map, &(mcc_str_t){"Banana"}, &(int){1});
	mcc_hash_map_insert(map, &(mcc_str_t){"Orange"}, &(int){2});
	mcc_hash_map_insert(map, &(mcc_str_t){"Raspberry"}, &(int){3});
	mcc_hash_map_insert(map, &(mcc_str_t){"Pear"}, &(int){4});
	mcc_hash_map_insert(map, &(mcc_str_t){"Watermelon"}, &(int){5});
	mcc_hash_map_insert(map, &(mcc_str_t){"Coconut"}, &(int){6});
	mcc_hash_map_insert(map, &(mcc_str_t){"Pineapple"}, &(int){7});
	mcc_hash_map_insert(map, &(mcc_str_t){"Strawberry"}, &(int){8});
	mcc_hash_map_insert(map, &(mcc_str_t){"Grape"}, &(int){9});

	puts("Raw data");
	mcc_hash_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_hash_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		int *v = pair.value;
		printf("(%s, %d)\n", *k, *v);
	}
	putchar('\n');

	puts("Removed Pineapple");
	mcc_hash_map_remove(map, &(mcc_str_t){"Pineapple"});

	puts("Removed Pear");
	mcc_hash_map_remove(map, &(mcc_str_t){"Pear"});

	puts("Update the value of Apple to 100");
	mcc_hash_map_insert(map, &(mcc_str_t){"Apple"}, &(int){100});

	mcc_hash_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_hash_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		int *v = pair.value;
		printf("(%s, %d)\n", *k, *v);
	}
	putchar('\n');

	mcc_hash_map_delete(map);

	return 0;
}

static int test_str_key_vector_value(void)
{
	struct mcc_hash_map *map;
	struct mcc_vector *tmp;
	struct mcc_hash_map_iter iter;

	map = mcc_hash_map_new(STR, MCC_VECTOR);

	tmp = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"a021be"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"92b341"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"891b7a"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"5ab362"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"1bf2e0"}, &tmp);

	mcc_hash_map_iter_init(map, &iter);
	srand(time(NULL));
	for (struct mcc_kv_pair pair; mcc_hash_map_iter_next(&iter, &pair);) {
		tmp = *(struct mcc_vector **)pair.value;
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
	}

	mcc_hash_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_hash_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		tmp = *(struct mcc_vector **)pair.value;

		printf("%s: ", *k);

		size_t len = mcc_vector_len(tmp);
		for (size_t i = 0; i < len; i++) {
			int t;
			mcc_vector_get(tmp, i, &t);
			printf("%2d ", t);
		}
		putchar('\n');
	}

	mcc_hash_map_delete(map);

	return 0;
}

int main()
{
	test_str_key_int_value();
	test_str_key_vector_value();
	return 0;
}
