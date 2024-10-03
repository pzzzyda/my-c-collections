#include "mcc_rb_set.h"
#include <stdlib.h>

struct mcc_rb_set {
	struct mcc_rb_map *data;
};

static const struct mcc_object_interface none_obj_intf = {
	.size = 0,
	.dtor = NULL,
	.cmp = NULL,
	.hash = NULL,
};

struct mcc_rb_set *mcc_rb_set_new(const struct mcc_object_interface *T)
{
	struct mcc_rb_set *self = calloc(1, sizeof(struct mcc_rb_set));

	if (!self)
		return NULL;

	self->data = mcc_rb_map_new(T, &none_obj_intf);
	if (!self->data) {
		free(self);
		return NULL;
	}

	return self;
}

void mcc_rb_set_delete(struct mcc_rb_set *self)
{
	if (!self)
		return;

	mcc_rb_map_delete(self->data);
	free(self);
}

mcc_err_t mcc_rb_set_insert(struct mcc_rb_set *self, const void *value)
{
	int none = 0;

	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_rb_map_insert(self->data, value, &none);
}

void mcc_rb_set_remove(struct mcc_rb_set *self, const void *value)
{
	if (!self)
		return;

	mcc_rb_map_remove(self->data, value);
}

void mcc_rb_set_clear(struct mcc_rb_set *self)
{
	if (!self)
		return;

	mcc_rb_map_clear(self->data);
}

mcc_err_t mcc_rb_set_get(struct mcc_rb_set *self, const void *value,
			 void *result)
{
	int none = 0;
	struct mcc_kv_pair pair = {result, &none};

	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_rb_map_get_key_value(self->data, value, &pair);
}

size_t mcc_rb_set_len(struct mcc_rb_set *self)
{
	return !self ? 0 : mcc_rb_map_len(self->data);
}

bool mcc_rb_set_is_empty(struct mcc_rb_set *self)
{
	return !self ? 0 : mcc_rb_map_is_empty(self->data);
}

static const struct mcc_iterator_interface mcc_rb_set_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_rb_set_iter_next,
};

mcc_err_t mcc_rb_set_iter_init(struct mcc_rb_set *self,
			       struct mcc_rb_set_iter *iter)
{
	if (!self)
		return INVALID_ARGUMENTS;

	mcc_rb_map_iter_init(self->data, &iter->base);
	iter->base.base.iter_intf = &mcc_rb_set_iter_intf;
	iter->container = self;
	return OK;
}

bool mcc_rb_set_iter_next(struct mcc_rb_set_iter *self, void *result)
{
	int none = 0;
	struct mcc_kv_pair pair = {result, &none};

	return mcc_rb_map_iter_next(&self->base, &pair);
}
