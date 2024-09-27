#include "base/mcc_array.h"
#include "mcc_utils.h"
#include <stdlib.h>

struct mcc_array *mcc_array_new(const struct mcc_object_interface *T,
				mcc_usize capacity)
{
	struct mcc_array *self;
	mcc_usize n;

	if (!T)
		return NULL;

	n = sizeof(struct mcc_array) + capacity * T->size;
	self = calloc(1, n);
	if (!self)
		return NULL;

	self->T = T;
	self->cap = capacity;
	return self;
}

void mcc_array_delete(struct mcc_array *self)
{
	free(self);
}

struct mcc_array *mcc_array_reserve(struct mcc_array *self, mcc_usize extras)
{
	struct mcc_array *new_arr;
	mcc_usize needs, new_capacity, n;

	needs = self->cap + extras;
	if (needs <= self->cap)
		return self;

	new_capacity = !self->cap ? 8 : self->cap << 1;
	while (new_capacity < needs)
		new_capacity <<= 1;

	n = sizeof(struct mcc_array) + new_capacity * self->T->size;
	new_arr = realloc(self, n);
	if (!new_arr)
		return NULL;

	n = (new_capacity - new_arr->cap) * new_arr->T->size;
	memset(mcc_array_at(new_arr, new_arr->cap), 0, n);

	new_arr->cap = new_capacity;
	return new_arr;
}

struct mcc_array *mcc_array_shrink(struct mcc_array *self, mcc_usize capacity)
{
	struct mcc_array *new_arr;
	mcc_usize n;

	n = sizeof(struct mcc_array) + capacity * self->T->size;
	new_arr = realloc(self, n);
	if (!new_arr)
		return NULL;

	new_arr->cap = capacity;
	return new_arr;
}

void *mcc_array_at(struct mcc_array *self, mcc_usize index)
{
	return (mcc_u8 *)self + sizeof(struct mcc_array) +
	       index * self->T->size;
}

void mcc_array_set(struct mcc_array *self, mcc_usize index, const void *value)
{
	memcpy(mcc_array_at(self, index), value, self->T->size);
}

void mcc_array_get(struct mcc_array *self, mcc_usize index, void *value)
{
	memcpy(value, mcc_array_at(self, index), self->T->size);
}

void *mcc_array_ptr(struct mcc_array *self)
{
	return (mcc_u8 *)self + sizeof(struct mcc_array);
}

void mcc_array_move(struct mcc_array *self, mcc_usize to, mcc_usize from,
		    mcc_usize n)
{
	mcc_usize total_size = n * self->T->size;
	void *p1 = mcc_array_at(self, to);
	void *p2 = mcc_array_at(self, from);

	memmove(p1, p2, total_size);
}

void mcc_array_swap(struct mcc_array *self, mcc_usize a, mcc_usize b)
{
	void *p1 = mcc_array_at(self, a);
	void *p2 = mcc_array_at(self, b);

	mcc_memswap(p1, p2, self->T->size);
}

mcc_i32 mcc_array_cmp(struct mcc_array *self, mcc_usize a, mcc_usize b)
{
	void *p1 = mcc_array_at(self, a);
	void *p2 = mcc_array_at(self, b);

	return self->T->cmp(p1, p2);
}
