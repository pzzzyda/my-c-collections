#include "mcc_map.h"
#include "mcc_set.h"

#define MAP(PTR) ((struct mcc_map *)(PTR))
#define MAP_ITER(PTR) ((struct mcc_map_iter *)(PTR))
#define SET(PTR) ((struct mcc_set *)(PTR))
#define SET_ITER(PTR) ((struct mcc_set_iter *)(PTR))

static const struct mcc_object_interface none = {
	.size = 0,
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)0,
	.hash = (mcc_hash_fn)0,
};

struct mcc_set *mcc_set_new(const struct mcc_object_interface *T)
{
	return SET(mcc_map_new(T, &none));
}

void mcc_set_drop(struct mcc_set *self)
{
	mcc_map_drop(MAP(self));
}

int mcc_set_insert(struct mcc_set *self, const void *value)
{
	return mcc_map_insert(MAP(self), value, &none);
}

void mcc_set_remove(struct mcc_set *self, const void *value)
{
	mcc_map_remove(MAP(self), value);
}

void mcc_set_clear(struct mcc_set *self)
{
	mcc_map_clear(MAP(self));
}

int mcc_set_get(struct mcc_set *self, const void *value, const void **ref)
{
	struct mcc_pair *pair;
	int err;

	err = mcc_map_get_key_value(MAP(self), value, &pair);
	if (!err)
		*ref = pair->key;
	return err;
}

size_t mcc_set_len(struct mcc_set *self)
{
	return mcc_map_len(MAP(self));
}

bool mcc_set_is_empty(struct mcc_set *self)
{
	return mcc_map_is_empty(MAP(self));
}

struct mcc_set_iter *mcc_set_iter_new(struct mcc_set *set)
{
	return SET_ITER(mcc_map_iter_new(MAP(set)));
}

void mcc_set_iter_drop(struct mcc_set_iter *self)
{
	mcc_map_iter_drop(MAP_ITER(self));
}

bool mcc_set_iter_next(struct mcc_set_iter *self, const void **ref)
{
	struct mcc_pair *pair;

	if (mcc_map_iter_next(MAP_ITER(self), &pair)) {
		*ref = pair->key;
		return true;
	} else {
		return false;
	}
}
