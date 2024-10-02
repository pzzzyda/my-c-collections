#include "base/mcc_array.h"
#include "mcc_utils.h"
#include <stdlib.h>

mcc_array_t *mcc_array_new(const struct mcc_object_interface *T, size_t len)
{
	mcc_array_t *self;
	size_t n;

	if (!T)
		return NULL;

	n = sizeof(mcc_array_t) + len * T->size;
	self = calloc(1, n);
	if (!self)
		return NULL;

	self->T = T;
	self->len = len;
	return self;
}

void mcc_array_delete(mcc_array_t *self)
{
	free(self);
}

mcc_array_t *mcc_array_reserve(mcc_array_t *self, size_t additional)
{
	mcc_array_t *new_arr;
	size_t n, new_len;
	const size_t needs = self->len + additional;

	if (needs <= self->len)
		return self;

	new_len = !self->len ? 8 : self->len << 1;
	while (new_len < needs)
		new_len <<= 1;

	n = sizeof(mcc_array_t) + new_len * self->T->size;
	new_arr = realloc(self, n);
	if (!new_arr)
		return NULL;

	n = (new_len - new_arr->len) * new_arr->T->size;
	memset(mcc_array_at(new_arr, new_arr->len), 0, n);

	new_arr->len = new_len;
	return new_arr;
}

mcc_array_t *mcc_array_resize(mcc_array_t *self, size_t new_len)
{
	mcc_array_t *new_arr;
	size_t n;

	n = sizeof(mcc_array_t) + new_len * self->T->size;
	new_arr = realloc(self, n);
	if (!new_arr)
		return NULL;

	new_arr->len = new_len;
	return new_arr;
}

void *mcc_array_at(mcc_array_t *self, size_t index)
{
	return (uint8_t *)self + sizeof(mcc_array_t) + index * self->T->size;
}

void mcc_array_set(mcc_array_t *self, size_t index, const void *value)
{
	memcpy(mcc_array_at(self, index), value, self->T->size);
}

void mcc_array_get(mcc_array_t *self, size_t index, void *value)
{
	memcpy(value, mcc_array_at(self, index), self->T->size);
}

void *mcc_array_ptr(mcc_array_t *self)
{
	return (uint8_t *)self + sizeof(mcc_array_t);
}

void mcc_array_move(mcc_array_t *self, size_t to, size_t from, size_t n)
{
	size_t total_size = n * self->T->size;
	void *pa = mcc_array_at(self, to);
	void *pb = mcc_array_at(self, from);

	memmove(pa, pb, total_size);
}

void mcc_array_swap(mcc_array_t *self, size_t a, size_t b)
{
	void *pa = mcc_array_at(self, a);
	void *pb = mcc_array_at(self, b);

	mcc_memswap(pa, pb, self->T->size);
}
