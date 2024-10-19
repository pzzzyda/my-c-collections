#include "mcc_map.h"
#include <assert.h>
#include <stdio.h>

#define map_insert mcc_map_insert
#define map_remove mcc_map_remove
#define map_get mcc_map_get

static void print(struct mcc_map *map)
{
	struct mcc_map_iter *iter;
	struct mcc_pair *pair;
	const mcc_str_t *k;
	int *v;

	iter = mcc_map_iter_new(map);
	assert(iter != NULL);
	while (mcc_map_iter_next(iter, &pair)) {
		k = pair->key;
		v = pair->value;
		printf("(%s, %d)\n", *k, *v);
	}

	mcc_map_iter_drop(iter);
}

int main(void)
{
	struct mcc_map *map = mcc_map_new(mcc_str(), mcc_int());
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
	map_remove(map, &(mcc_str_t){"Orange"});
	map_remove(map, &(mcc_str_t){"Grape"});
	map_remove(map, &(mcc_str_t){"Watermelon"});
	puts("---------------------------");
	print(map);
	map_remove(map, &(mcc_str_t){"Apple"});
	map_remove(map, &(mcc_str_t){"Banana"});
	map_remove(map, &(mcc_str_t){"Raspberry"});
	map_remove(map, &(mcc_str_t){"Pear"});
	map_remove(map, &(mcc_str_t){"Coconut"});
	map_remove(map, &(mcc_str_t){"Pineapple"});
	map_remove(map, &(mcc_str_t){"Strawberry"});
	puts("---------------------------");
	print(map);
	mcc_map_drop(map);
	puts("testing done");
	return 0;
}
