#include "mcc_hash_map.h"
#include "mcc_hash_set.h"

#define HASH_MAP(PTR) ((struct mcc_hash_map *)(PTR))
#define HASH_SET(PTR) ((struct mcc_hash_set *)(PTR))
#define HASH_MAP_ITER(PTR) ((struct mcc_hash_map_iter *)(PTR))
#define HASH_SET_ITER(PTR) ((struct mcc_hash_set_iter *)(PTR))

static const struct mcc_object_interface none = {
	.size = 0,
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)0,
	.hash = (mcc_hash_fn)0,
};

struct mcc_hash_set *mcc_hash_set_new(const struct mcc_object_interface *T)
{
	return HASH_SET(mcc_hash_map_new(T, &none));
}

void mcc_hash_set_drop(struct mcc_hash_set *self)
{
	mcc_hash_map_drop(HASH_MAP(self));
}

int mcc_hash_set_reserve(struct mcc_hash_set *self, size_t additional)
{
	return mcc_hash_map_reserve(HASH_MAP(self), additional);
}

int mcc_hash_set_insert(struct mcc_hash_set *self, const void *value)
{
	return mcc_hash_map_insert(HASH_MAP(self), value, &none);
}

void mcc_hash_set_remove(struct mcc_hash_set *self, const void *value)
{
	mcc_hash_map_remove(HASH_MAP(self), value);
}

void mcc_hash_set_clear(struct mcc_hash_set *self)
{
	mcc_hash_map_clear(HASH_MAP(self));
}

int mcc_hash_set_get(struct mcc_hash_set *self, const void *value,
		     const void **ref)
{
	struct mcc_pair *pair;
	int err;

	err = mcc_hash_map_get_key_value(HASH_MAP(self), value, &pair);
	if (!err)
		*ref = pair->key;
	return err;
}

size_t mcc_hash_set_capacity(struct mcc_hash_set *self)
{
	return mcc_hash_map_capacity(HASH_MAP(self));
}

size_t mcc_hash_set_len(struct mcc_hash_set *self)
{
	return mcc_hash_map_len(HASH_MAP(self));
}

bool mcc_hash_set_is_empty(struct mcc_hash_set *self)
{
	return mcc_hash_map_is_empty(HASH_MAP(self));
}

struct mcc_hash_set_iter *mcc_hash_set_iter_new(struct mcc_hash_set *set)
{
	return HASH_SET_ITER(mcc_hash_map_iter_new(HASH_MAP(set)));
}

void mcc_hash_set_iter_drop(struct mcc_hash_set_iter *self)
{
	mcc_hash_map_iter_drop(HASH_MAP_ITER(self));
}

bool mcc_hash_set_iter_next(struct mcc_hash_set_iter *self, const void **ref)
{
	struct mcc_pair *pair;

	if (mcc_hash_map_iter_next(HASH_MAP_ITER(self), &pair)) {
		*ref = pair->key;
		return true;
	} else {
		return false;
	}
}
