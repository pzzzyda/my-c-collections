#include "mcc_vector.h"
#include "base/mcc_array.h"
#include <stdlib.h>

struct mcc_vector {
	mcc_array_t *buf;
	size_t len;
};

static void mcc_vector_dtor(void *self)
{
	mcc_vector_delete(*(struct mcc_vector **)self);
}

static int mcc_vector_cmp(const void *self, const void *other)
{
	struct mcc_vector *const *p1 = self;
	struct mcc_vector *const *p2 = other;

	return SIZE_T->cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_vector_hash(const void *self)
{
	struct mcc_vector *const *p = self;

	return SIZE_T->hash(&(**p).len);
}

const struct mcc_object_interface __mcc_vector_obj_intf = {
	.size = sizeof(struct mcc_vector *),
	.dtor = &mcc_vector_dtor,
	.cmp = &mcc_vector_cmp,
	.hash = &mcc_vector_hash,
};

static inline bool has_destructor(struct mcc_vector *self)
{
	return self->buf->T->dtor != NULL;
}

static inline void call_destructor(struct mcc_vector *self, size_t index)
{
	self->buf->T->dtor(mcc_array_at(self->buf, index));
}

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *T)
{
	struct mcc_vector *self;

	self = calloc(1, sizeof(struct mcc_vector));
	if (!self)
		return NULL;

	self->buf = mcc_array_new(T, 0);
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

mcc_err_t mcc_vector_reserve(struct mcc_vector *self, size_t additional)
{
	mcc_array_t *new_buf;

	if (!self)
		return INVALID_ARGUMENTS;

	new_buf = mcc_array_reserve(self->buf, additional);
	if (!new_buf)
		return CANNOT_ALLOCATE_MEMORY;

	self->buf = new_buf;
	return OK;
}

mcc_err_t mcc_vector_shrink_to_fit(struct mcc_vector *self)
{
	mcc_array_t *new_buf;

	if (!self)
		return INVALID_ARGUMENTS;

	new_buf = mcc_array_resize(self->buf, self->len);
	if (!new_buf)
		return CANNOT_ALLOCATE_MEMORY;

	self->buf = new_buf;
	return OK;
}

mcc_err_t mcc_vector_push(struct mcc_vector *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->buf->len) {
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

	if (has_destructor(self))
		call_destructor(self, self->len - 1);
	self->len--;
}

mcc_err_t mcc_vector_insert(struct mcc_vector *self, size_t index,
			    const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (index == self->len)
		return mcc_vector_push(self, value);

	if (self->len >= self->buf->len) {
		if (mcc_vector_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	mcc_array_move(self->buf, index + 1, index, self->len - index);
	mcc_array_set(self->buf, index, value);
	self->len++;
	return OK;
}

void mcc_vector_remove(struct mcc_vector *self, size_t index)
{
	if (!self || index >= self->len)
		return;

	if (index == self->len - 1) {
		mcc_vector_pop(self);
		return;
	}

	if (has_destructor(self))
		call_destructor(self, index);
	mcc_array_move(self->buf, index, index + 1, self->len - index - 1);
	self->len--;
}

void mcc_vector_clear(struct mcc_vector *self)
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

mcc_err_t mcc_vector_get(struct mcc_vector *self, size_t index, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	mcc_array_get(self->buf, index, value);
	return OK;
}

void *mcc_vector_get_ptr(struct mcc_vector *self, size_t index)
{
	if (!self || index >= self->len)
		return NULL;
	else
		return mcc_array_at(self->buf, index);
}

mcc_err_t mcc_vector_front(struct mcc_vector *self, void *value)
{
	return mcc_vector_get(self, 0, value);
}

void *mcc_vector_front_ptr(struct mcc_vector *self)
{
	return mcc_vector_get_ptr(self, 0);
}

mcc_err_t mcc_vector_back(struct mcc_vector *self, void *value)
{
	return mcc_vector_get(self, self->len - 1, value);
}

void *mcc_vector_back_ptr(struct mcc_vector *self)
{
	return mcc_vector_get_ptr(self, self->len - 1);
}

size_t mcc_vector_capacity(struct mcc_vector *self)
{
	return !self ? 0 : self->buf->len;
}

size_t mcc_vector_len(struct mcc_vector *self)
{
	return !self ? 0 : self->len;
}

bool mcc_vector_is_empty(struct mcc_vector *self)
{
	return !self ? true : self->len == 0;
}

mcc_err_t mcc_vector_swap(struct mcc_vector *self, size_t a, size_t b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b >= self->len)
		return OUT_OF_RANGE;

	if (a == b)
		return OK;

	mcc_array_swap(self->buf, a, b);
	return OK;
}

mcc_err_t mcc_vector_reverse(struct mcc_vector *self)
{
	size_t i, j;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (i = 0, j = self->len - 1; i < j; i++, j--)
		mcc_array_swap(self->buf, i, j);
	return OK;
}

mcc_err_t mcc_vector_sort(struct mcc_vector *self)
{
	void *elems;
	size_t elem_nums, elem_size;
	mcc_compare_fn cmp;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	elems = mcc_array_ptr(self->buf);
	elem_nums = self->len;
	elem_size = self->buf->T->size;
	cmp = self->buf->T->cmp;

	/* Use the sorting function of the standard library. */
	qsort(elems, elem_nums, elem_size, cmp);

	return OK;
}

void *mcc_vector_binary_search(struct mcc_vector *self, const void *key)
{
	void *elems;
	size_t elem_nums, elem_size;
	mcc_compare_fn cmp;

	if (!self || !key || !self->len)
		return NULL;

	elems = mcc_array_ptr(self->buf);
	elem_nums = self->len;
	elem_size = self->buf->T->size;
	cmp = self->buf->T->cmp;

	/* Use the binary search function of the standard library. */
	return bsearch(key, elems, elem_nums, elem_size, cmp);
}

mcc_err_t mcc_vector_iter_init(struct mcc_vector *self,
			       struct mcc_vector_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_vector_iter_next;
	iter->index = 0;
	iter->container = self;
	return OK;
}

bool mcc_vector_iter_next(struct mcc_vector_iter *iter, void *result)
{
	if (!iter || iter->index >= iter->container->len)
		return false;

	if (result)
		mcc_vector_get(iter->container, iter->index, result);
	iter->index++;
	return true;
}
