#include "mcc_heap.h"
#include "mcc_err.h"
#include "mcc_vector.h"
#include <stdlib.h>

struct mcc_heap {
	struct mcc_vector *data;
	mcc_compare_f cmp;
};

static void mcc_heap_dtor(void *self)
{
	mcc_heap_delete(*(struct mcc_heap **)self);
}

const struct mcc_object_interface mcc_heap_object_interface = {
	.size = sizeof(struct mcc_heap *),
	.dtor = mcc_heap_dtor,
};

static void sift_down(struct mcc_heap *self, size_t i)
{
	size_t large, left, right;
	void *tmp1, *tmp2;

	const size_t len = mcc_vector_len(self->data);

	while (i * 2 + 1 < len) {
		large = i;
		left = i * 2 + 1;
		right = i * 2 + 2;

		tmp1 = mcc_vector_get_ptr(self->data, left);
		tmp2 = mcc_vector_get_ptr(self->data, large);
		if (self->cmp(tmp1, tmp2) > 0)
			large = left;

		if (right < len) {
			tmp1 = mcc_vector_get_ptr(self->data, right);
			tmp2 = mcc_vector_get_ptr(self->data, large);
			if (self->cmp(tmp1, tmp2) > 0)
				large = right;
		}

		if (large != i) {
			mcc_vector_swap(self->data, large, i);
			i = large;
		} else {
			break;
		}
	}
}

struct mcc_heap *mcc_heap_new(const struct mcc_object_interface *element,
			      mcc_compare_f cmp)
{
	struct mcc_heap *self;

	if (!cmp)
		return NULL;

	self = malloc(sizeof(struct mcc_heap));
	if (!self)
		return NULL;

	self->data = mcc_vector_new(element);
	if (!self->data) {
		free(self);
		return NULL;
	}

	(self)->cmp = cmp;
	return self;
}

void mcc_heap_delete(struct mcc_heap *self)
{
	if (!self)
		return;

	mcc_vector_delete(self->data);
	free(self);
}

int mcc_heap_reserve(struct mcc_heap *self, size_t additional)
{
	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_vector_reserve(self->data, additional);
}

int mcc_heap_shrink_to_fit(struct mcc_heap *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_vector_shrink_to_fit(self->data);
}

int mcc_heap_push(struct mcc_heap *self, void *value)
{
	int e;
	size_t len;

	if (!self)
		return INVALID_ARGUMENTS;

	e = mcc_vector_push(self->data, value);
	if (e != OK)
		return e;

	len = mcc_vector_len(self->data);
	if (len > 1) {
		for (size_t i = len / 2;;) {
			sift_down(self, i);
			if (i-- == 0)
				break;
		}
	}
	return OK;
}

void mcc_heap_pop(struct mcc_heap *self)
{
	size_t len;

	if (!self)
		return;

	len = mcc_vector_len(self->data);
	if (len) {
		mcc_vector_swap(self->data, 0, len - 1);
		mcc_vector_pop(self->data);
		sift_down(self, 0);
	}
}

void mcc_heap_clear(struct mcc_heap *self)
{
	if (!self)
		return;

	return mcc_vector_clear(self->data);
}

int mcc_heap_peek(struct mcc_heap *self, void *value)
{
	if (!self)
		return INVALID_ARGUMENTS;

	return mcc_vector_front(self->data, value);
}

void *mcc_heap_peek_ptr(struct mcc_heap *self)
{
	return !self ? NULL : mcc_vector_front_ptr(self->data);
}

size_t mcc_heap_capacity(struct mcc_heap *self)
{
	return !self ? 0 : mcc_vector_capacity(self->data);
}

size_t mcc_heap_len(struct mcc_heap *self)
{
	return !self ? 0 : mcc_vector_len(self->data);
}

bool mcc_heap_is_empty(struct mcc_heap *self)
{
	return !self ? true : mcc_vector_is_empty(self->data);
}

int mcc_heap_iter_init(struct mcc_heap *self, struct mcc_heap_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iter_next_f)&mcc_heap_iter_next;
	iter->idx = 0;
	iter->container = self;
	return OK;
}

bool mcc_heap_iter_next(struct mcc_heap_iter *iter, void *result)
{
	if (!iter || iter->idx >= mcc_vector_len(iter->container->data))
		return false;

	if (result)
		mcc_vector_get(iter->container->data, iter->idx, result);
	iter->idx++;
	return true;
}
