#include "mcc_hash_set.h"
#include "mcc_err.h"
#include <stdlib.h>

struct mcc_hash_set {
	struct mcc_hash_map *data;
};

static const struct mcc_object_interface none_obj_intf = {
	.size = 0,
	.dtor = NULL,
	.cmp = NULL,
	.hash = NULL,
};

struct mcc_hash_set *mcc_hash_set_new(const struct mcc_object_interface *T)
{
	struct mcc_hash_set *self = calloc(1, sizeof(struct mcc_hash_set));

	if (!self)
		return NULL;

	self->data = mcc_hash_map_new(T, &none_obj_intf);
	if (!self->data) {
		free(self);
		return NULL;
	}

	return self;
}

void mcc_hash_set_delete(struct mcc_hash_set *self)
{
	if (!self)
		return;

	mcc_hash_map_delete(self->data);
	free(self);
}

int mcc_hash_set_reserve(struct mcc_hash_set *self, size_t additional)
{
	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_hash_map_reserve(self->data, additional);
}

int mcc_hash_set_insert(struct mcc_hash_set *self, const void *value)
{
	int none = 0;

	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_hash_map_insert(self->data, value, &none);
}

void mcc_hash_set_remove(struct mcc_hash_set *self, const void *value)
{
	if (!self)
		return;

	mcc_hash_map_remove(self->data, value);
}

void mcc_hash_set_clear(struct mcc_hash_set *self)
{
	if (!self)
		return;

	mcc_hash_map_clear(self->data);
}

int mcc_hash_set_get(struct mcc_hash_set *self, const void *value, void *result)
{
	int none = 0;
	struct mcc_kv_pair pair = {result, &none};

	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_hash_map_get_key_value(self->data, value, &pair);
}

size_t mcc_hash_set_capacity(struct mcc_hash_set *self)
{
	return !self ? 0 : mcc_hash_map_capacity(self->data);
}

size_t mcc_hash_set_len(struct mcc_hash_set *self)
{
	return !self ? 0 : mcc_hash_map_len(self->data);
}

bool mcc_hash_set_is_empty(struct mcc_hash_set *self)
{
	return !self ? true : mcc_hash_map_is_empty(self->data);
}

static const struct mcc_iterator_interface mcc_hash_set_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_hash_set_iter_next,
};

int mcc_hash_set_iter_init(struct mcc_hash_set *self,
			   struct mcc_hash_set_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	mcc_hash_map_iter_init(self->data, &iter->base);
	iter->base.base.iter_intf = &mcc_hash_set_iter_intf;
	iter->container = self;
	return OK;
}

bool mcc_hash_set_iter_next(struct mcc_hash_set_iter *self, void *result)
{
	int none = 0;
	struct mcc_kv_pair pair = {result, &none};

	return mcc_hash_map_iter_next(&self->base, &pair);
}
