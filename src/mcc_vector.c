#include "mcc_vector.h"
#include "base/mcc_array.h"
#include <stdlib.h>

struct mcc_vector {
	struct mcc_array *buf;
	mcc_usize len;
};

static void mcc_vector_dtor(void *self)
{
	mcc_vector_delete(*(struct mcc_vector **)self);
}

static mcc_i32 mcc_vector_cmp(const void *self, const void *other)
{
	struct mcc_vector *const *p1 = self;
	struct mcc_vector *const *p2 = other;

	return mcc_usize_i.cmp(&(**p1).len, &(**p2).len);
}

static mcc_usize mcc_vector_hash(const void *self)
{
	struct mcc_vector *const *p = self;

	return mcc_usize_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_vector_i = {
	.size = sizeof(struct mcc_vector *),
	.dtor = &mcc_vector_dtor,
	.cmp = &mcc_vector_cmp,
	.hash = &mcc_vector_hash,
};

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *element)
{
	struct mcc_vector *self;

	self = calloc(1, sizeof(struct mcc_vector));
	if (!self)
		return NULL;

	self->buf = mcc_array_new(element, 0);
	if (!self->buf) {
		free(self);
		return NULL;
	}

	return self;
}

void mcc_vector_delete(struct mcc_vector *self)
{
	if (!self)
		return;

	mcc_vector_clear(self);
	mcc_array_delete(self->buf);
	free(self);
}

mcc_err mcc_vector_reserve(struct mcc_vector *self, mcc_usize additional)
{
	struct mcc_array *new_buf;

	if (!self)
		return INVALID_ARGUMENTS;

	new_buf = mcc_array_reserve(self->buf, additional);
	if (!new_buf)
		return CANNOT_ALLOCATE_MEMORY;

	self->buf = new_buf;
	return OK;
}

mcc_err mcc_vector_shrink_to_fit(struct mcc_vector *self)
{
	struct mcc_array *new_buf;

	if (!self)
		return INVALID_ARGUMENTS;

	new_buf = mcc_array_shrink(self->buf, self->len);
	if (!new_buf)
		return CANNOT_ALLOCATE_MEMORY;

	self->buf = new_buf;
	return OK;
}

mcc_err mcc_vector_push(struct mcc_vector *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->buf->cap) {
		if (mcc_vector_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	mcc_array_set(self->buf, self->len, value);
	self->len++;
	return OK;
}

void mcc_vector_pop(struct mcc_vector *self)
{
	if (!self || !self->len)
		return;

	if (self->buf->T->dtor)
		self->buf->T->dtor(mcc_vector_back_ptr(self));
	self->len--;
}

mcc_err mcc_vector_insert(struct mcc_vector *self, mcc_usize index,
			  const void *value)
{
	mcc_usize n;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (index == self->len)
		return mcc_vector_push(self, value);

	if (self->len >= self->buf->cap) {
		if (mcc_vector_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	n = self->len - index;
	mcc_array_move(self->buf, index + 1, index, n);
	mcc_array_set(self->buf, index, value);
	self->len++;
	return OK;
}

void mcc_vector_remove(struct mcc_vector *self, mcc_usize index)
{
	mcc_usize n;

	if (!self || index >= self->len)
		return;

	if (index == self->len - 1) {
		mcc_vector_pop(self);
		return;
	}

	if (self->buf->T->dtor)
		self->buf->T->dtor(mcc_array_at(self->buf, index));
	n = self->len - index - 1;
	mcc_array_move(self->buf, index, index + 1, n);
	self->len--;
}

void mcc_vector_clear(struct mcc_vector *self)
{
	void *tmp;

	if (!self || !self->len)
		return;

	if (self->buf->T->dtor) {
		while (self->len) {
			tmp = mcc_array_at(self->buf, --self->len);
			self->buf->T->dtor(tmp);
		}
	} else {
		self->len = 0;
	}
}

mcc_err mcc_vector_get(struct mcc_vector *self, mcc_usize index, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return OUT_OF_RANGE;

	mcc_array_get(self->buf, index, value);
	return OK;
}

void *mcc_vector_get_ptr(struct mcc_vector *self, mcc_usize index)
{
	if (!self || index >= self->len)
		return NULL;
	else
		return mcc_array_at(self->buf, index);
}

mcc_err mcc_vector_front(struct mcc_vector *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	mcc_array_get(self->buf, 0, value);
	return OK;
}

void *mcc_vector_front_ptr(struct mcc_vector *self)
{
	if (!self || !self->len)
		return NULL;
	else
		return mcc_array_at(self->buf, 0);
}

mcc_err mcc_vector_back(struct mcc_vector *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	mcc_array_get(self->buf, self->len - 1, value);
	return OK;
}

void *mcc_vector_back_ptr(struct mcc_vector *self)
{
	if (!self || !self->len)
		return NULL;
	else
		return mcc_array_at(self->buf, self->len - 1);
}

mcc_usize mcc_vector_capacity(struct mcc_vector *self)
{
	return !self ? 0 : self->buf->cap;
}

mcc_usize mcc_vector_len(struct mcc_vector *self)
{
	return !self ? 0 : self->len;
}

mcc_bool mcc_vector_is_empty(struct mcc_vector *self)
{
	return !self ? true : self->len == 0;
}

mcc_err mcc_vector_swap(struct mcc_vector *self, mcc_usize a, mcc_usize b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b > self->len)
		return OUT_OF_RANGE;

	if (a == b)
		return OK;

	mcc_array_swap(self->buf, a, b);
	return OK;
}

mcc_err mcc_vector_reverse(struct mcc_vector *self)
{
	mcc_usize i, j;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (i = 0, j = self->len - 1; i < j; i++, j--)
		mcc_array_swap(self->buf, i, j);
	return OK;
}

mcc_err mcc_vector_sort(struct mcc_vector *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	qsort(mcc_array_ptr(self->buf), self->len, self->buf->T->size,
	      self->buf->T->cmp);
	return OK;
}

void *mcc_vector_binary_search(struct mcc_vector *self, const void *key)
{
	if (!self || !key || !self->len)
		return NULL;
	else
		return bsearch(key, mcc_array_ptr(self->buf), self->len,
			       self->buf->T->size, self->buf->T->cmp);
}

mcc_err mcc_vector_iter_init(struct mcc_vector *self,
			     struct mcc_vector_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_vector_iter_next;
	iter->index = 0;
	iter->container = self;
	return OK;
}

mcc_bool mcc_vector_iter_next(struct mcc_vector_iter *iter, void *result)
{
	if (!iter || iter->index >= iter->container->len)
		return false;

	if (result)
		mcc_vector_get(iter->container, iter->index, result);
	iter->index++;
	return true;
}
