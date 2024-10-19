#include "mcc_hash_map.h"
#include <assert.h>
#include <stdio.h>

#define map_insert mcc_hash_map_insert
#define map_get mcc_hash_map_get
#define map_remove mcc_hash_map_remove

static void print(struct mcc_hash_map *map)
{
	struct mcc_hash_map_iter *iter;
	struct mcc_pair *ref;
	const mcc_str_t *k;
	int *v;

	iter = mcc_hash_map_iter_new(map);
	assert(iter != NULL);
	while (mcc_hash_map_iter_next(iter, &ref)) {
		k = ref->key;
		v = ref->value;
		printf("(%s, %d)\n", *k, *v);
	}

	mcc_hash_map_iter_drop(iter);
}

int main(void)
{
	struct mcc_hash_map *map = mcc_hash_map_new(mcc_str(), mcc_int());
	assert(map != NULL);
	assert(!map_insert(map, &(mcc_str_t){"Apple"}, &(int){0}));
	assert(!map_insert(map, &(mcc_str_t){"Banana"}, &(int){1}));
	assert(!map_insert(map, &(mcc_str_t){"Orange"}, &(int){2}));
	assert(!map_insert(map, &(mcc_str_t){"Raspberry"}, &(int){3}));
	assert(!map_insert(map, &(mcc_str_t){"Pear"}, &(int){4}));
	assert(!map_insert(map, &(mcc_str_t){"Watermelon"}, &(int){5}));
	assert(!map_insert(map, &(mcc_str_t){"Coconut"}, &(int){6}));
	assert(!map_insert(map, &(mcc_str_t){"Pineapple"}, &(int){7}));
	assert(!map_insert(map, &(mcc_str_t){"Strawberry"}, &(int){8}));
	assert(!map_insert(map, &(mcc_str_t){"Grape"}, &(int){9}));
	print(map);
	puts("-------------------------------");
	map_remove(map, &(mcc_str_t){"Orange"});
	map_remove(map, &(mcc_str_t){"Strawberry"});
	print(map);
	mcc_hash_map_drop(map);
	puts("testing done");
	return 0;
}
