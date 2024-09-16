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

	map = mcc_hash_map_new(&mcc_str_i, &mcc_i32_i);

	mcc_hash_map_insert(map, "89ba21", &(int){0});
	mcc_hash_map_insert(map, "b412ca", &(int){1});
	mcc_hash_map_insert(map, "b9ac44", &(int){2});
	mcc_hash_map_insert(map, "43aa43", &(int){3});
	mcc_hash_map_insert(map, "13ba1d", &(int){4});
	mcc_hash_map_insert(map, "7531ba", &(int){5});
	mcc_hash_map_insert(map, "21ce90", &(int){6});
	mcc_hash_map_insert(map, "5421ba", &(int){7});
	mcc_hash_map_insert(map, "af231e", &(int){8});
	mcc_hash_map_insert(map, "d1b53a", &(int){9});

	mcc_hash_map_remove(map, "b9ac44");

	mcc_hash_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_hash_map_iter_next(&iter, &pair);)
		printf("(%s, %d)\n", (char *)pair.key, *(int *)pair.value);
	putchar('\n');

	mcc_hash_map_delete(map);

	return 0;
}

static int test_str_key_vector_value(void)
{
	struct mcc_hash_map *map;
	struct mcc_vector *tmp;
	struct mcc_hash_map_iter iter;

	map = mcc_hash_map_new(&mcc_str_i, &mcc_vector_i);

	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_hash_map_insert(map, "a021be", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_hash_map_insert(map, "92b341", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_hash_map_insert(map, "891b7a", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_hash_map_insert(map, "5ab362", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_hash_map_insert(map, "1bf2e0", &tmp);

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
		char *k = pair.key;
		tmp = *(struct mcc_vector **)pair.value;

		printf("%s: ", k);

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
