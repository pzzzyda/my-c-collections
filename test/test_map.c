#include "mcc_map.h"
#include "mcc_utils.h"
#include "mcc_vector.h"
#include <stdio.h>
#include <time.h>

static int test_str_key_int_value(void)
{
	struct mcc_map *map;
	struct mcc_map_iter iter;

	map = mcc_map_new(&mcc_str_i, &mcc_i32_i);

	mcc_map_insert(map, "A", &(int){'A'});
	mcc_map_insert(map, "E", &(int){'E'});
	mcc_map_insert(map, "D", &(int){'D'});
	mcc_map_insert(map, "C", &(int){'C'});
	mcc_map_insert(map, "B", &(int){'B'});
	mcc_map_insert(map, "F", &(int){'F'});
	mcc_map_insert(map, "b", &(int){'b'});
	mcc_map_insert(map, "w", &(int){'w'});
	mcc_map_insert(map, "k", &(int){'k'});
	mcc_map_insert(map, "Z", &(int){'Z'});
	mcc_map_insert(map, "n", &(int){'n'});
	mcc_map_insert(map, "O", &(int){'O'});
	mcc_map_insert(map, "s", &(int){'s'});
	mcc_map_insert(map, "v", &(int){'v'});
	mcc_map_insert(map, "K", &(int){'K'});

	mcc_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_map_iter_next(&iter, &pair);)
		printf("(%s, %d)\n", (char *)pair.key, *(int *)pair.value);

	mcc_map_remove(map, "C");
	mcc_map_remove(map, "b");
	mcc_map_remove(map, "Z");

	puts("Removed 'C', 'Z', 'b' ");

	mcc_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_map_iter_next(&iter, &pair);)
		printf("(%s, %d)\n", (char *)pair.key, *(int *)pair.value);

	mcc_map_clear(map);

	puts("Clear the map");

	mcc_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_map_iter_next(&iter, &pair);)
		printf("(%s, %d)\n", (char *)pair.key, *(int *)pair.value);

	mcc_map_delete(map);
	return 0;
}

static int test_str_key_vector_value(void)
{
	struct mcc_map *map;
	struct mcc_vector *tmp;
	struct mcc_map_iter iter;

	map = mcc_map_new(&mcc_str_i, &mcc_vector_i);

	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_map_insert(map, "a021be", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_map_insert(map, "92b341", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_map_insert(map, "891b7a", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_map_insert(map, "5ab362", &tmp);
	tmp = mcc_vector_new(&mcc_i32_i);
	mcc_map_insert(map, "1bf2e0", &tmp);

	mcc_map_iter_init(map, &iter);
	srand(time(NULL));
	for (struct mcc_kv_pair pair; mcc_map_iter_next(&iter, &pair);) {
		tmp = *(struct mcc_vector **)pair.value;
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
	}

	putchar('\n');
	mcc_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_map_iter_next(&iter, &pair);) {
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

	mcc_map_delete(map);

	return 0;
}

int main(int argc, char **argv)
{
	test_str_key_int_value();
	test_str_key_vector_value();
	return 0;
}
