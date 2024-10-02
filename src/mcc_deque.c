#include "mcc_deque.h"
#include "base/mcc_array.h"
#include <stdlib.h>

struct mcc_deque {
	mcc_array_t *buf;
	size_t head;
	size_t len;
};

static void mcc_deque_dtor(void *self)
{
	mcc_deque_delete(*(struct mcc_deque **)self);
}

static int mcc_deque_cmp(const void *self, const void *other)
{
	struct mcc_deque *const *p1 = self;
	struct mcc_deque *const *p2 = other;

	return SIZE_T->cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_deque_hash(const void *self)
{
	struct mcc_deque *const *p = self;

	return SIZE_T->hash(&(**p).len);
}

const struct mcc_object_interface __mcc_deque_obj_intf = {
	.size = sizeof(struct mcc_deque *),
	.dtor = &mcc_deque_dtor,
	.cmp = &mcc_deque_cmp,
	.hash = &mcc_deque_hash,
};

static inline size_t to_real_index(struct mcc_deque *self, size_t index)
{
	return (self->head + index) % self->buf->len;
}

static inline bool has_destructor(struct mcc_deque *self)
{
	return self->buf->T->dtor != NULL;
}

static inline void call_destructor(struct mcc_deque *self, size_t index)
{
	index = to_real_index(self, index);
	self->buf->T->dtor(mcc_array_at(self->buf, index));
}

static inline mcc_err_t insert(struct mcc_deque *self, size_t index,
			       const void *value)
{
	size_t from, to, n;

	if (to_real_index(self, index) <= to_real_index(self, self->len - 1)) {
		/*
		 * Case 1:
		 *
		 * assume index == 3
		 *               _____
		 *              |     | ->
		 *        0 1 2 3 4 5 6
		 * |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *  0 1 2 3 4 5 6 7 8 9 A B C D E F
		 *        ^     ^     ^
		 *      head  index  tail
		 *
		 * or
		 *
		 * assume index == 5
		 *     _______
		 *    |       | ->
		 *  4 5 6 7 8 9             0 1 2 3
		 * |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *  0 1 2 3 4 5 6 7 8 9 A B C D E F
		 *    ^       ^                 ^
		 *  index    tail              head
		 */
		from = to_real_index(self, index);
		to = from + 1;
		n = self->len - index;
		index = to_real_index(self, index);
	} else {
		/*
		 * Case 2:
		 *
		 * assume index == 4
		 *
		 *                  move forward
		 *                     _____
		 *                 <- |     |
		 *  7 8 9             0 1 2 3 4 5 6
		 * |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *  0 1 2 3 4 5 6 7 8 9 A B C D E F
		 *      ^             ^       ^
		 *     tail         head     index
		 */
		from = to_real_index(self, 0);
		to = from - 1;
		n = index;
		index = to_real_index(self, index - 1);
		self->head--;
	}

	mcc_array_move(self->buf, to, from, n);
	mcc_array_set(self->buf, index, value);
	self->len++;
	return OK;
}

static inline mcc_err_t remove(struct mcc_deque *self, size_t index)
{
	size_t from, to, n;

	if (has_destructor(self))
		call_destructor(self, index);

	/* see function 'insert' */
	if (to_real_index(self, index) <= to_real_index(self, self->len - 1)) {
		from = to_real_index(self, index + 1);
		to = from - 1;
		n = self->len - index - 1;
	} else {
		from = to_real_index(self, 0);
		to = from + 1;
		n = index;
		self->head++;
	}

	mcc_array_move(self->buf, to, from, n);
	self->len--;
	return OK;
}

struct mcc_deque *mcc_deque_new(const struct mcc_object_interface *T)
{
	struct mcc_deque *self;

	self = calloc(1, sizeof(struct mcc_deque));
	if (!self)
		return NULL;

	self->buf = mcc_array_new(T, 0);
	if (!self->buf) {
		free(self);
		return NULL;
	}

	return self;
}

void mcc_deque_delete(struct mcc_deque *self)
{
	if (!self)
		return;

	mcc_deque_clear(self);
	mcc_array_delete(self->buf);
	free(self);
}

mcc_err_t mcc_deque_reserve(struct mcc_deque *self, size_t additional)
{
	mcc_array_t *new_buf;
	size_t diff, old_capacity, from, to, n;

	if (!self)
		return INVALID_ARGUMENTS;

	old_capacity = self->buf->len;
	new_buf = mcc_array_reserve(self->buf, additional);
	if (!new_buf)
		return CANNOT_ALLOCATE_MEMORY;
	self->buf = new_buf;

	if (self->head + self->len > old_capacity) {
		/*
		 *  block1      block2
		 *    ___     ___________
		 *   |   |   |           | ->
		 *   7 8 9   0 1 2 3 4 5 6       ~~~~~~~~~~~~~
		 *  |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *   0 1 2 3 4 5 6 7 8 9 A|      ^            |
		 *       ^   ^            |___________________|
		 *     tail head                expanded
		 *
		 * If the distribution of elements in the deque looks
		 * like this before the capacity is expanded, block2
		 * should be moved after the capacity is expanded.
		 */
		diff = self->buf->len - old_capacity;
		from = self->head;
		to = self->head + diff;
		n = old_capacity - self->head;

		mcc_array_move(self->buf, to, from, n);

		self->head += diff;
	}
	return OK;
}

mcc_err_t mcc_deque_push_front(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->buf->len) {
		if (mcc_deque_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	/*
	 * Decrease self->head. "!!self->len" ensures that when the first
	 * element is added, the self->head remains the same.
	 */
	self->head += self->buf->len;
	self->head -= !!self->buf->len;
	self->head %= self->buf->len;

	mcc_array_set(self->buf, to_real_index(self, 0), value);
	self->len++;
	return OK;
}

mcc_err_t mcc_deque_push_back(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->buf->len) {
		if (mcc_deque_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	mcc_array_set(self->buf, to_real_index(self, self->len), value);
	self->len++;
	return OK;
}

void mcc_deque_pop_front(struct mcc_deque *self)
{
	if (!self || !self->len)
		return;

	if (has_destructor(self))
		call_destructor(self, 0);
	self->len--;

	/*
	 * Increase self->head. "!!self->len" ensures that when the last element
	 * is removed, the self->head remains the same.
	 */
	self->head += !!self->len;
	self->head %= self->buf->len;
}

void mcc_deque_pop_back(struct mcc_deque *self)
{
	if (!self || !self->len)
		return;

	if (has_destructor(self))
		call_destructor(self, self->len - 1);
	self->len--;
}

mcc_err_t mcc_deque_insert(struct mcc_deque *self, size_t index,
			   const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (self->len >= self->buf->len) {
		if (mcc_deque_reserve(self, 1) != OK)
			return CANNOT_ALLOCATE_MEMORY;
	}

	if (index == 0)
		return mcc_deque_push_front(self, value);
	else if (index == self->len)
		return mcc_deque_push_back(self, value);
	else
		return insert(self, index, value);
}

void mcc_deque_remove(struct mcc_deque *self, size_t index)
{
	if (!self || index >= self->len)
		return;

	if (index == 0)
		mcc_deque_pop_front(self);
	else if (index == self->len - 1)
		mcc_deque_pop_back(self);
	else
		remove(self, index);
}

void mcc_deque_clear(struct mcc_deque *self)
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

mcc_err_t mcc_deque_get(struct mcc_deque *self, size_t index, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	mcc_array_get(self->buf, to_real_index(self, index), value);
	return OK;
}

void *mcc_deque_get_ptr(struct mcc_deque *self, size_t index)
{
	if (!self || index >= self->len)
		return NULL;
	else
		return mcc_array_at(self->buf, to_real_index(self, index));
}

mcc_err_t mcc_deque_front(struct mcc_deque *self, void *value)
{
	return mcc_deque_get(self, 0, value);
}

void *mcc_deque_front_ptr(struct mcc_deque *self)
{
	return mcc_deque_get_ptr(self, 0);
}

mcc_err_t mcc_deque_back(struct mcc_deque *self, void *value)
{
	return mcc_deque_get(self, self->len - 1, value);
}

void *mcc_deque_back_ptr(struct mcc_deque *self)
{
	return mcc_deque_get_ptr(self, self->len - 1);
}

size_t mcc_deque_capacity(struct mcc_deque *self)
{
	return !self ? 0 : self->buf->len;
}

size_t mcc_deque_len(struct mcc_deque *self)
{
	return !self ? 0 : self->len;
}

bool mcc_deque_is_empty(struct mcc_deque *self)
{
	return !self ? true : self->len == 0;
}

mcc_err_t mcc_deque_swap(struct mcc_deque *self, size_t a, size_t b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b >= self->len)
		return OUT_OF_RANGE;

	if (a == b)
		return OK;

	a = to_real_index(self, a);
	b = to_real_index(self, b);
	mcc_array_swap(self->buf, a, b);
	return OK;
}

mcc_err_t mcc_deque_reverse(struct mcc_deque *self)
{
	size_t i, j;

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (i = 0, j = self->len - 1; i < j; i++, j--)
		mcc_deque_swap(self, i, j);
	return OK;
}

static void quick_sort(struct mcc_deque *self, size_t low, size_t high)
{
	size_t mid = low;
	size_t i = low;
	const void *pivot = mcc_deque_get_ptr(self, high);

	for (; i < high; i++) {
		if (self->buf->T->cmp(mcc_deque_get_ptr(self, i), pivot) < 0)
			mcc_deque_swap(self, i, mid++);
	}

	mcc_deque_swap(self, mid, high);

	if (low + 1 < mid)
		quick_sort(self, low, mid - 1);
	if (mid + 1 < high)
		quick_sort(self, mid + 1, high);
}

mcc_err_t mcc_deque_sort(struct mcc_deque *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	quick_sort(self, 0, self->len - 1);
	return OK;
}

void *mcc_deque_binary_search(struct mcc_deque *self, const void *key)
{
	size_t low, high, mid;
	int cmp_res;

	if (!self || !key || !self->len)
		return NULL;

	low = 0;
	high = self->len - 1;
	while (low <= high && high < self->len) {
		mid = low + ((high - low) >> 1);
		cmp_res = self->buf->T->cmp(key, mcc_deque_get_ptr(self, mid));
		if (cmp_res > 0)
			low = mid + 1;
		else if (cmp_res < 0)
			high = mid - 1;
		else
			return mcc_deque_get_ptr(self, mid);
	}

	return NULL;
}

mcc_err_t mcc_deque_iter_init(struct mcc_deque *self,
			      struct mcc_deque_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_deque_iter_next;
	iter->index = 0;
	iter->container = self;
	return OK;
}

bool mcc_deque_iter_next(struct mcc_deque_iter *iter, void *result)
{
	if (!iter || iter->index >= iter->container->len)
		return false;

	if (result)
		mcc_deque_get(iter->container, iter->index, result);
	iter->index++;
	return true;
}
