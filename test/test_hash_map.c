#include "mcc_hash_map.h"
#include "mcc_vector.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int test_str_key_int_value(void)
{
	printf("%s\n", __FUNCTION__);
	struct mcc_hash_map_iter iter;
	const char *key;
	int value;
	int *ptr;
	struct mcc_kv_pair pair = {&key, &value};
	struct mcc_hash_map *map = mcc_hash_map_new(STR, INT);

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

	mcc_hash_map_get_key_value(map, &(mcc_str_t){"Watermelon"}, &pair);
	printf("%s = %d\n", key, value);

	mcc_hash_map_get_key_value(map, &(mcc_str_t){"Grape"}, &pair);
	printf("%s = %d\n", key, value);

	mcc_hash_map_get_key_value(map, &(mcc_str_t){"Pineapple"}, &pair);
	printf("%s = %d\n", key, value);

	puts("Raw data");
	mcc_hash_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair))
		printf("(%s, %d)\n", key, value);

	puts("Change the value of 'Watermelon' to 1000");
	ptr = mcc_hash_map_get_ptr(map, &(mcc_str_t){"Watermelon"});
	*ptr = 1000;

	puts("Removed Pineapple");
	mcc_hash_map_remove(map, &(mcc_str_t){"Pineapple"});

	puts("Removed Pear");
	mcc_hash_map_remove(map, &(mcc_str_t){"Pear"});

	puts("Update the value of Apple to 100");
	mcc_hash_map_insert(map, &(mcc_str_t){"Apple"}, &(int){100});

	mcc_hash_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair))
		printf("(%s, %d)\n", key, value);

	mcc_hash_map_delete(map);
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
	struct mcc_hash_map_iter iter;
	const char *key;
	struct mcc_vector *value;
	struct mcc_kv_pair pair = {&key, &value};
	struct mcc_hash_map *map = mcc_hash_map_new(STR, MCC_VECTOR);

	value = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"a021be"}, &value);
	value = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"92b341"}, &value);
	value = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"891b7a"}, &value);
	value = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"5ab362"}, &value);
	value = mcc_vector_new(INT);
	mcc_hash_map_insert(map, &(mcc_str_t){"1bf2e0"}, &value);

	mcc_hash_map_iter_init(map, &iter);
	srand(time(NULL));
	while (mcc_iter_next(&iter, &pair)) {
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
		mcc_vector_push(value, &(int){rand() % 100});
	}

	mcc_hash_map_iter_init(map, &iter);
	while (mcc_iter_next(&iter, &pair)) {
		mcc_vector_sort(value);
		print_int_vector(value, "%s: ", key);
	}

	mcc_hash_map_delete(map);
	puts("testing done");
	return 0;
}

int main()
{
	test_str_key_int_value();
	test_str_key_vector_value();
	return 0;
}
