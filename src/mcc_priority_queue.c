#include "mcc_priority_queue.h"
#include "base/mcc_array.h"
#include "mcc_err.h"
#include <stdlib.h>

struct mcc_priority_queue {
	mcc_array_t *buf;
	size_t len;
};

static inline bool has_destructor(struct mcc_priority_queue *self)
{
	return self->buf->T->dtor != NULL;
}

static inline void call_destructor(struct mcc_priority_queue *self,
				   size_t index)
{
	self->buf->T->dtor(mcc_array_at(self->buf, index));
}

static inline int do_compare(struct mcc_priority_queue *self, size_t a,
			     size_t b)
{
	void *pa = mcc_array_at(self->buf, a);
	void *pb = mcc_array_at(self->buf, b);

	return self->buf->T->cmp(pa, pb);
}

static inline void sift_down(struct mcc_priority_queue *self, size_t i)
{
	size_t left, right, large;

	while (i * 2 + 1 < self->len) {
		large = i;
		left = i * 2 + 1;
		right = i * 2 + 2;

		if (do_compare(self, left, large) > 0)
			large = left;

		if (right < self->len) {
			if (do_compare(self, right, large) > 0)
				large = right;
		}

		if (large != i) {
			mcc_array_swap(self->buf, large, i);
			i = large;
		} else {
			break;
		}
	}
}

struct mcc_priority_queue *
mcc_priority_queue_new(const struct mcc_object_interface *T)
{
	struct mcc_priority_queue *self;

	self = calloc(1, sizeof(struct mcc_priority_queue));
	if (!self)
		return NULL;

	self->buf = mcc_array_new(T, 0);
	if (!self->buf) {
		free(self);
		return NULL;
	}

	return self;
}

void mcc_priority_queue_delete(struct mcc_priority_queue *self)
{
	if (!self)
		return;

	mcc_priority_queue_clear(self);
	mcc_array_delete(self->buf);
	free(self);
}

int mcc_priority_queue_reserve(struct mcc_priority_queue *self,
			       size_t additional)
{
	mcc_array_t *new_buf;

	if (!self)
		return INVALID_ARGUMENTS;

	new_buf = mcc_array_reserve(self->buf, additional);
	if (!new_buf)
		return INVALID_ARGUMENTS;

	self->buf = new_buf;
	return OK;
}

int mcc_priority_queue_push(struct mcc_priority_queue *self, const void *value)
{
	size_t i;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->buf->len) {
		if (mcc_priority_queue_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	mcc_array_set(self->buf, self->len, value);
	self->len++;

	if (self->len > 1) {
		i = self->len << 1;

		while (true) {
			sift_down(self, i);
			if (i-- == 0)
				break;
		}
	}

	return OK;
}

void mcc_priority_queue_pop(struct mcc_priority_queue *self)
{
	if (!self || !self->len)
		return;

	mcc_array_swap(self->buf, 0, self->len - 1);
	if (has_destructor(self))
		call_destructor(self, self->len - 1);
	self->len--;

	if (self->len > 1)
		sift_down(self, 0);
}

void mcc_priority_queue_clear(struct mcc_priority_queue *self)
{
	if (!self || !self->len)
		return;

	if (has_destructor(self)) {
		while (self->len > 0)
			call_destructor(self, --self->len);
	} else {
		self->len = 0;
	}
}

int mcc_priority_queue_front(struct mcc_priority_queue *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	mcc_array_get(self->buf, 0, value);
	return OK;
}

size_t mcc_priority_queue_len(struct mcc_priority_queue *self)
{
	return !self ? 0 : self->len;
}

size_t mcc_priority_queue_capacity(struct mcc_priority_queue *self)
{
	return !self ? 0 : self->buf->len;
}

bool mcc_priority_queue_is_empty(struct mcc_priority_queue *self)
{
	return !self ? true : self->len == 0;
}
