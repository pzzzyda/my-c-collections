#include "mcc_vector.h"
#include "mcc_utils.h"
#include <stdlib.h>

struct mcc_vector {
	mcc_u8 *ptr;
	mcc_usize len;
	mcc_usize cap;
	struct mcc_object_interface elem;
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

static inline void *get_ptr(struct mcc_vector *self, mcc_usize index)
{
	return self->ptr + index * self->elem.size;
}

static inline void move_elems(struct mcc_vector *self, mcc_usize dest,
			      mcc_usize src, mcc_usize n)
{
	void *p1 = get_ptr(self, dest);
	void *p2 = get_ptr(self, src);
	mcc_usize total_size = n * self->elem.size;

	memmove(p1, p2, total_size);
}

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *element)
{
	struct mcc_vector *self;

	if (!element)
		return NULL;

	self = calloc(1, sizeof(struct mcc_vector));
	if (!self)
		return NULL;

	memcpy(&self->elem, element, sizeof(self->elem));
	return self;
}

void mcc_vector_delete(struct mcc_vector *self)
{
	if (!self)
		return;

	mcc_vector_clear(self);
	free(self->ptr);
	free(self);
}

mcc_err mcc_vector_reserve(struct mcc_vector *self, mcc_usize additional)
{
	mcc_usize new_capacity, needs;
	mcc_u8 *tmp;

	if (!self)
		return INVALID_ARGUMENTS;

	needs = self->len + additional;
	if (needs <= self->cap)
		return OK;

	new_capacity = !self->cap ? 4 : self->cap << 1;
	while (new_capacity < needs)
		new_capacity <<= 1;

	tmp = realloc(self->ptr, new_capacity * self->elem.size);
	if (!tmp)
		return CANNOT_ALLOCATE_MEMORY;

	self->ptr = tmp;
	self->cap = new_capacity;
	return OK;
}

mcc_err mcc_vector_shrink_to_fit(struct mcc_vector *self)
{
	mcc_u8 *tmp;

	if (!self)
		return INVALID_ARGUMENTS;

	if (!self->len) {
		free(self->ptr);
		self->ptr = NULL;
		self->cap = 0;
		return OK;
	}

	tmp = realloc(self->ptr, self->len * self->elem.size);
	if (!tmp)
		return CANNOT_ALLOCATE_MEMORY;

	self->ptr = tmp;
	self->cap = self->len;
	return OK;
}

mcc_err mcc_vector_push(struct mcc_vector *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->cap && mcc_vector_reserve(self, 1) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	memcpy(get_ptr(self, self->len), value, self->elem.size);
	self->len++;
	return OK;
}

void mcc_vector_pop(struct mcc_vector *self)
{
	if (!self)
		return;

	if (self->len) {
		if (self->elem.dtor)
			self->elem.dtor(get_ptr(self, self->len - 1));
		self->len--;
	}
}

mcc_err mcc_vector_insert(struct mcc_vector *self, mcc_usize index,
			  const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index && index >= self->len)
		return OUT_OF_RANGE;

	if (self->len >= self->cap && mcc_vector_reserve(self, 1) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	if (!index || index == self->len - 1) {
		return mcc_vector_push(self, value);
	} else {
		move_elems(self, index + 1, index, self->len - index);
		memcpy(get_ptr(self, index), value, self->elem.size);
		self->len++;
		return OK;
	}
}

void mcc_vector_remove(struct mcc_vector *self, mcc_usize index)
{
	if (!self || index >= self->len)
		return;

	if (self->elem.dtor)
		self->elem.dtor(get_ptr(self, index));
	move_elems(self, index, index + 1, self->len - index - 1);
	self->len--;
}

void mcc_vector_clear(struct mcc_vector *self)
{
	if (!self)
		return;

	if (self->elem.dtor) {
		while (self->len)
			self->elem.dtor(get_ptr(self, --self->len));
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

	memcpy(value, get_ptr(self, index), self->elem.size);
	return OK;
}

void *mcc_vector_get_ptr(struct mcc_vector *self, mcc_usize index)
{
	return !self || index >= self->len ? NULL : get_ptr(self, index);
}

mcc_err mcc_vector_front(struct mcc_vector *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, get_ptr(self, 0), self->elem.size);
	return OK;
}

void *mcc_vector_front_ptr(struct mcc_vector *self)
{
	return !self || !self->len ? NULL : get_ptr(self, 0);
}

mcc_err mcc_vector_back(struct mcc_vector *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, get_ptr(self, self->len - 1), self->elem.size);
	return OK;
}

void *mcc_vector_back_ptr(struct mcc_vector *self)
{
	return !self || !self->len ? NULL : get_ptr(self, self->len - 1);
}

mcc_usize mcc_vector_capacity(struct mcc_vector *self)
{
	return !self ? 0 : self->cap;
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

	mcc_memswap(get_ptr(self, a), get_ptr(self, b), self->elem.size);
	return OK;
}

mcc_err mcc_vector_reverse(struct mcc_vector *self)
{
	void *p1, *p2;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (mcc_usize i = 0, j = self->len - 1; i < j; i++, j--) {
		p1 = get_ptr(self, i);
		p2 = get_ptr(self, j);
		mcc_memswap(p1, p2, self->elem.size);
	}
	return OK;
}

mcc_err mcc_vector_sort(struct mcc_vector *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	qsort(self->ptr, self->len, self->elem.size, self->elem.cmp);
	return OK;
}

void *mcc_vector_binary_search(struct mcc_vector *self, const void *key)
{
	if (!self || !key || !self->len)
		return NULL;
	else
		return bsearch(key, self->ptr, self->len, self->elem.size,
			       self->elem.cmp);
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
