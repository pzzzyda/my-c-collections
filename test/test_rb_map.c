#include "mcc_rb_map.h"
#include "mcc_utils.h"
#include "mcc_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct A {
	int x;
	int y;
};

int foo(struct A *a)
{
	return a->x + a->y;
}

static int test_str_key_int_value(void)
{
	struct mcc_rb_map *map;
	struct mcc_rb_map_iter iter;

	map = mcc_rb_map_new(STR, INT);

	mcc_rb_map_insert(map, &(mcc_str_t){"A"}, &(int){'A'});
	mcc_rb_map_insert(map, &(mcc_str_t){"E"}, &(int){'E'});
	mcc_rb_map_insert(map, &(mcc_str_t){"D"}, &(int){'D'});
	mcc_rb_map_insert(map, &(mcc_str_t){"C"}, &(int){'C'});
	mcc_rb_map_insert(map, &(mcc_str_t){"B"}, &(int){'B'});
	mcc_rb_map_insert(map, &(mcc_str_t){"F"}, &(int){'F'});
	mcc_rb_map_insert(map, &(mcc_str_t){"b"}, &(int){'b'});
	mcc_rb_map_insert(map, &(mcc_str_t){"w"}, &(int){'w'});
	mcc_rb_map_insert(map, &(mcc_str_t){"k"}, &(int){'k'});
	mcc_rb_map_insert(map, &(mcc_str_t){"Z"}, &(int){'Z'});
	mcc_rb_map_insert(map, &(mcc_str_t){"n"}, &(int){'n'});
	mcc_rb_map_insert(map, &(mcc_str_t){"O"}, &(int){'O'});
	mcc_rb_map_insert(map, &(mcc_str_t){"s"}, &(int){'s'});
	mcc_rb_map_insert(map, &(mcc_str_t){"v"}, &(int){'v'});
	mcc_rb_map_insert(map, &(mcc_str_t){"K"}, &(int){'K'});

	printf("len = %ld\n", mcc_rb_map_len(map));

	mcc_rb_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_rb_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		int *v = pair.value;
		printf("(%s, %d)\n", *k, *v);
	}

	mcc_rb_map_remove(map, &(mcc_str_t){"C"});
	mcc_rb_map_remove(map, &(mcc_str_t){"b"});
	mcc_rb_map_remove(map, &(mcc_str_t){"Z"});

	puts("Removed 'C', 'Z', 'b' ");

	mcc_rb_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_rb_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		int *v = pair.value;
		printf("(%s, %d)\n", *k, *v);
	}

	mcc_rb_map_clear(map);

	puts("Clear the map");

	mcc_rb_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_rb_map_iter_next(&iter, &pair);) {
		mcc_str_t *k = pair.key;
		int *v = pair.value;
		printf("(%s, %d)\n", *k, *v);
	}

	mcc_rb_map_delete(map);
	return 0;
}

static int test_str_key_vector_value(void)
{
	struct mcc_rb_map *map;
	struct mcc_vector *tmp;
	struct mcc_rb_map_iter iter;

	map = mcc_rb_map_new(STR, MCC_VECTOR);

	tmp = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"a021be"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"92b341"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"891b7a"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"5ab362"}, &tmp);
	tmp = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"1bf2e0"}, &tmp);

	mcc_rb_map_iter_init(map, &iter);
	srand(time(NULL));
	for (struct mcc_kv_pair pair; mcc_rb_map_iter_next(&iter, &pair);) {
		tmp = *(struct mcc_vector **)pair.value;
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
		mcc_vector_push(tmp, &(int){rand() % 100});
	}

	putchar('\n');
	mcc_rb_map_iter_init(map, &iter);
	for (struct mcc_kv_pair pair; mcc_rb_map_iter_next(&iter, &pair);) {
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

	mcc_rb_map_delete(map);

	return 0;
}

int main(int argc, char **argv)
{
	test_str_key_int_value();
	test_str_key_vector_value();
	return 0;
}
