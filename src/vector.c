#include "mcc_err.h"
#include "mcc_vector.h"
#include "memswap.h"
#include "sift_down.h"
#include <stdlib.h>

struct mcc_vector_iter {
	struct mcc_vector_iter *next;
	struct mcc_vector *vector;
	size_t curr;
	bool in_use;
};

struct mcc_vector {
	const struct mcc_object_interface *T;
	struct mcc_vector_iter *iters;
	uint8_t *ptr;
	size_t len;
	size_t capacity;
};

static inline void *get(struct mcc_vector *self, size_t index)
{
	return self->ptr + index * self->T->size;
}

static inline void move(struct mcc_vector *self, size_t to, size_t from,
			size_t n)
{
	memcpy(get(self, to), get(self, from), n * self->T->size);
}

static int reallocate_buffer(struct mcc_vector *self, size_t capacity)
{
	uint8_t *new_ptr;

	if (!capacity) {
		if (self->capacity) {
			free(self->ptr);
			self->ptr = NULL;
			self->capacity = 0;
		}
		return OK;
	}

	new_ptr = realloc(self->ptr, capacity * self->T->size);

	if (new_ptr) {
		self->ptr = new_ptr;
		self->capacity = capacity;
		return OK;
	} else {
		return CANNOT_ALLOCATE_MEMORY;
	}
}

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *T)
{
	struct mcc_vector *self;

	if (!T)
		return NULL;

	self = calloc(1, sizeof(struct mcc_vector));
	if (!self)
		return NULL;

	self->T = T;
	return self;
}

void mcc_vector_drop(struct mcc_vector *self)
{
	struct mcc_vector_iter *next;

	if (!self)
		return;

	while (self->iters) {
		next = self->iters->next;
		free(self->iters);
		self->iters = next;
	}
	mcc_vector_clear(self);
	free(self->ptr);
	free(self);
}

int mcc_vector_reserve(struct mcc_vector *self, size_t additional)
{
	size_t min_capacity, new_capacity;

	if (!self)
		return INVALID_ARGUMENTS;

	min_capacity = self->len + additional;
	if (min_capacity <= self->capacity)
		return OK;

	new_capacity = !self->capacity ? 8 : self->capacity << 1;
	while (new_capacity < min_capacity)
		new_capacity <<= 1;

	return reallocate_buffer(self, new_capacity);
}

int mcc_vector_grow_to(struct mcc_vector *self, size_t capacity)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (capacity > self->capacity)
		return reallocate_buffer(self, capacity);
	else
		return OK;
}

int mcc_vector_shrink_to(struct mcc_vector *self, size_t capacity)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (capacity < self->capacity)
		return reallocate_buffer(self, capacity);
	else
		return OK;
}

int mcc_vector_shrink_to_fit(struct mcc_vector *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->capacity > self->len)
		return mcc_vector_shrink_to(self, self->len);
	else
		return OK;
}

int mcc_vector_push(struct mcc_vector *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->capacity) {
		if (mcc_vector_reserve(self, 1))
			return CANNOT_ALLOCATE_MEMORY;
	}

	memcpy(get(self, self->len++), value, self->T->size);
	return OK;
}

void mcc_vector_pop(struct mcc_vector *self)
{
	if (!self || !self->len)
		return;

	if (self->T->drop)
		self->T->drop(get(self, self->len - 1));
	self->len--;
}

int mcc_vector_insert(struct mcc_vector *self, size_t index, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (index == self->len) {
		return mcc_vector_push(self, value);
	} else {
		if (self->len >= self->capacity) {
			if (mcc_vector_reserve(self, 1))
				return CANNOT_ALLOCATE_MEMORY;
		}

		move(self, index + 1, index, self->len - index);
		memcpy(get(self, index), value, self->T->size);
		self->len++;
		return OK;
	}
}

void mcc_vector_remove(struct mcc_vector *self, size_t index)
{
	if (!self || index >= self->len)
		return;

	if (index == self->len - 1) {
		mcc_vector_pop(self);
	} else {
		if (self->T->drop)
			self->T->drop(get(self, index));

		move(self, index, index + 1, self->len - index - 1);
		self->len--;
	}
}

void mcc_vector_clear(struct mcc_vector *self)
{
	if (!self || !self->len)
		return;

	if (self->T->drop) {
		while (self->len > 0)
			self->T->drop(get(self, --self->len));
	} else {
		self->len = 0;
	}
}

int mcc_vector_set(struct mcc_vector *self, size_t index, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return OUT_OF_RANGE;

	if (self->T->drop)
		self->T->drop(get(self, index));
	memcpy(get(self, index), value, self->T->size);
	return OK;
}

int mcc_vector_get(struct mcc_vector *self, size_t index, void **ref)
{
	if (!self || !ref)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	*ref = get(self, index);
	return OK;
}

int mcc_vector_front(struct mcc_vector *self, void **ref)
{
	return mcc_vector_get(self, 0, ref);
}

int mcc_vector_back(struct mcc_vector *self, void **ref)
{
	return mcc_vector_get(self, self->len - 1, ref);
}

size_t mcc_vector_capacity(struct mcc_vector *self)
{
	return !self ? 0 : self->capacity;
}

size_t mcc_vector_len(struct mcc_vector *self)
{
	return !self ? 0 : self->len;
}

bool mcc_vector_is_empty(struct mcc_vector *self)
{
	return !self ? true : self->len == 0;
}

int mcc_vector_swap(struct mcc_vector *self, size_t a, size_t b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b >= self->len)
		return OUT_OF_RANGE;

	if (a != b)
		memswap(get(self, a), get(self, b), self->T->size);
	return OK;
}

int mcc_vector_reverse(struct mcc_vector *self)
{
	size_t i, j;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (i = 0, j = self->len - 1; i < j; i++, j--)
		memswap(get(self, i), get(self, j), self->T->size);
	return OK;
}

int mcc_vector_sort(struct mcc_vector *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	/* Use the sorting function of the standard library. */
	qsort(self->ptr, self->len, self->T->size, self->T->cmp);
	return OK;
}

void *mcc_vector_binary_search(struct mcc_vector *self, const void *key)
{
	if (!self || !key || !self->len)
		return NULL;

	/* Use the binary search function of the standard library. */
	return bsearch(key, self->ptr, self->len, self->T->size, self->T->cmp);
}

struct mcc_vector_iter *mcc_vector_iter_new(struct mcc_vector *vector)
{
	struct mcc_vector_iter *self;

	if (!vector)
		return NULL;

	self = vector->iters;
	while (self) {
		if (!self->in_use)
			goto reset_iterator;
		self = self->next;
	}

	self = calloc(1, sizeof(struct mcc_vector_iter));
	if (!self)
		return NULL;

	self->next = vector->iters;
	vector->iters = self;
	self->vector = vector;
reset_iterator:
	self->curr = 0;
	self->in_use = true;
	return self;
}

void mcc_vector_iter_drop(struct mcc_vector_iter *self)
{
	if (self)
		self->in_use = false;
}

bool mcc_vector_iter_next(struct mcc_vector_iter *self, void **ref)
{
	if (!self || !ref || self->curr >= self->vector->len)
		return false;

	*ref = get(self->vector, self->curr++);
	return true;
}

static inline int do_compare(struct mcc_vector *self, size_t a, size_t b)
{
	return self->T->cmp(get(self, a), get(self, b));
}

void sift_down(struct mcc_vector *self, size_t i)
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
			mcc_vector_swap(self, large, i);
			i = large;
		} else {
			break;
		}
	}
}
