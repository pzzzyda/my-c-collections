#include "mcc_rb_map.h"
#include "mcc_vector.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int test_str_key_int_value(void)
{
	printf("%s\n", __FUNCTION__);
	struct mcc_rb_map_iter iter;
	const char *key;
	int value;
	int *ptr;
	struct mcc_kv_pair pair = {&key, &value};
	struct mcc_rb_map *map = mcc_rb_map_new(STR, INT);

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

	mcc_rb_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair))
		printf("(%s, %d)\n", key, value);

	mcc_rb_map_remove(map, &(mcc_str_t){"C"});
	mcc_rb_map_remove(map, &(mcc_str_t){"b"});
	mcc_rb_map_remove(map, &(mcc_str_t){"Z"});

	ptr = mcc_rb_map_get_ptr(map, &(mcc_str_t){"w"});
	*ptr = 1000;

	puts("Removed 'C', 'Z', 'b' and changed the value of 'w' to 1000");

	mcc_rb_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair))
		printf("(%s, %d)\n", key, value);

	mcc_rb_map_delete(map);
	puts("testing done");
	return 0;
}

void print_int_vector(struct mcc_vector *vec, const char *fmt, ...)
{
	struct mcc_vector_iter iter;
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	mcc_vector_iter_init(vec, &iter);
	printf("[ ");
	for (int i; mcc_iter_next(&iter, &i); i++)
		printf("%d ", i);
	printf("]\n");
}

int test_str_key_vector_value(void)
{
	printf("%s\n", __FUNCTION__);
	struct mcc_rb_map_iter iter;
	const char *key;
	struct mcc_vector *value;
	struct mcc_kv_pair pair = {&key, &value};
	struct mcc_rb_map *map = mcc_rb_map_new(STR, MCC_VECTOR);

	value = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"a021be"}, &value);
	value = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"92b341"}, &value);
	value = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"891b7a"}, &value);
	value = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"5ab362"}, &value);
	value = mcc_vector_new(INT);
	mcc_rb_map_insert(map, &(mcc_str_t){"1bf2e0"}, &value);

	mcc_rb_map_iter_init(map, &iter);
	srand(time(NULL));
	while (mcc_iter_next(&iter, &pair)) {
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
	}

	mcc_rb_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair)) {
		mcc_vector_sort(value);
		print_int_vector(value, "%s: ", key);
	}

	mcc_rb_map_delete(map);
	puts("testing done");
	return 0;
}

int main(void)
{
	test_str_key_int_value();
	test_str_key_vector_value();
	return 0;
}
