#include "fruit.h"
#include "mcc_hash_set.h"
#include <assert.h>
#include <stdio.h>

#define set_insert mcc_hash_set_insert
#define set_remove mcc_hash_set_remove
#define set_get mcc_hash_set_get
#define set_clear mcc_hash_set_clear

int main(void)
{
	struct fruit tmp;
	const struct fruit *ref;
	struct mcc_hash_set *set = mcc_hash_set_new(&fruit_);
	assert(set != NULL);
	assert(!set_insert(set, fruit_new(&tmp, "Orange")));
	assert(!set_insert(set, fruit_new(&tmp, "Watermelon")));
	assert(!set_insert(set, fruit_new(&tmp, "Apple")));
	assert(!set_insert(set, fruit_new(&tmp, "Pear")));
	assert(!set_insert(set, fruit_new(&tmp, "Pineapple")));
	assert(!set_insert(set, fruit_new(&tmp, "Banana")));
	assert(!set_insert(set, fruit_new(&tmp, "Grape")));
	assert(!set_insert(set, fruit_new(&tmp, "Strawberry")));
	assert(!set_get(set, &(struct fruit){"Apple"}, (const void **)&ref));
	assert(!fruit_cmp(ref, &(struct fruit){"Apple", 1, 0.5}));
	assert(!set_get(set, &(struct fruit){"Grape"}, (const void **)&ref));
	assert(!fruit_cmp(ref, &(struct fruit){"Grape", 1, 0.5}));
	set_remove(set, &(struct fruit){"Pineapple"});
	assert(set_get(set, &(struct fruit){"Pineapple"}, (const void **)&ref));
	mcc_hash_set_drop(set);
	puts("testing done");
	return 0;
}
