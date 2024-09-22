#include "mcc_deque.h"
#include "mcc_utils.h"
#include <stdlib.h>

struct mcc_deque {
	mcc_u8 *ptr;
	mcc_usize head;
	mcc_usize len;
	mcc_usize cap;
	struct mcc_object_interface elem;
};

static void mcc_deque_dtor(void *self)
{
	mcc_deque_delete(*(struct mcc_deque **)self);
}

static mcc_i32 mcc_deque_cmp(const void *self, const void *other)
{
	struct mcc_deque *const *p1 = self;
	struct mcc_deque *const *p2 = other;

	return mcc_usize_i.cmp(&(**p1).len, &(**p2).len);
}

static mcc_usize mcc_deque_hash(const void *self)
{
	struct mcc_deque *const *p = self;

	return mcc_usize_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_deque_i = {
	.size = sizeof(struct mcc_deque *),
	.dtor = &mcc_deque_dtor,
	.cmp = &mcc_deque_cmp,
	.hash = &mcc_deque_hash,
};

static inline mcc_usize mod_idx(struct mcc_deque *self, mcc_usize index)
{
	return (self->head + index) % self->cap;
}

static inline void *get_ptr(struct mcc_deque *self, mcc_usize index)
{
	return self->ptr + mod_idx(self, index) * self->elem.size;
}

static inline void move_elems(struct mcc_deque *self, mcc_usize dest,
			      mcc_usize src, mcc_usize n)
{
	void *p1 = self->ptr + dest * self->elem.size;
	void *p2 = self->ptr + src * self->elem.size;
	mcc_usize total_size = n * self->elem.size;

	memmove(p1, p2, total_size);
}

static inline mcc_err insert_elem_in_the_mid(struct mcc_deque *self,
					     mcc_usize index, const void *value)
{
	if (mod_idx(self, index) < mod_idx(self, self->len - 1)) {
		/*
		 * Case 1:
		 *
		 * assume index == 3
		 *
		 *             move back
		 *               _____
		 *              |     | ->
		 *        0 1 2 3 4 5 6
		 * |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *  0 1 2 3 4 5 6 7 8 9 A B C D E F
		 *        ^     ^     ^
		 *      head         tail
		 *
		 * or
		 *
		 * assume index == 5
		 *
		 *    move back
		 *     _______
		 *    |       | ->
		 *  4 5 6 7 8 9             0 1 2 3
		 * |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
		 *  0 1 2 3 4 5 6 7 8 9 A B C D E F
		 *    ^       ^                 ^
		 *  index    tail              head
		 */
		move_elems(self, mod_idx(self, index + 1), mod_idx(self, index),
			   self->len - index);
		memcpy(get_ptr(self, index), value, self->elem.size);
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
		 *        ^           ^       ^
		 *       tail       head     index
		 */
		move_elems(self, self->head - 1, self->head, index);
		memcpy(get_ptr(self, index - 1), value, self->elem.size);
		self->head--;
	}

	self->len++;
	return OK;
}

static inline mcc_err remove_elem_in_the_mid(struct mcc_deque *self,
					     mcc_usize index)
{
	if (self->elem.dtor)
		self->elem.dtor(get_ptr(self, index));

	/* see function 'insert_elem_in_the_mid' */
	if (mod_idx(self, index) < mod_idx(self, self->len - 1)) {
		move_elems(self, mod_idx(self, index), mod_idx(self, index + 1),
			   self->len - index - 1);
	} else {
		move_elems(self, self->head + 1, self->head, index);
		self->head++;
	}

	self->len--;
	return OK;
}

static mcc_err grow(struct mcc_deque *self, mcc_usize new_capacity)
{
	mcc_usize diff, old_capacity;
	mcc_u8 *tmp;

	if (new_capacity <= self->cap)
		return OK;

	old_capacity = self->cap;

	tmp = realloc(self->ptr, new_capacity * self->elem.size);
	if (!tmp)
		return CANNOT_ALLOCATE_MEMORY;

	self->ptr = tmp;
	self->cap = new_capacity;

	if (self->head + self->len > old_capacity) {
		/*
		 *  block1              block2    |
		 *    ___             ___________ |
		 *   |   |           |           ||
		 *   7 8 9           0 1 2 3 4 5 6|
		 *  |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|...|_|
		 *   0 1 2 3 4 5 6 7 8 9 A B C D E|         |
		 *       ^           ^            |_________|
		 *      tail        head          | expanded
		 *
		 * If the distribution of elements in the deque looks
		 * like this before the capacity is expanded, block2
		 * should be moved after the capacity is expanded.
		 */
		diff = new_capacity - old_capacity;
		move_elems(self, self->head + diff, self->head,
			   old_capacity - self->head);
		self->head += diff;
	}
	return OK;
}

struct mcc_deque *mcc_deque_new(const struct mcc_object_interface *element)
{
	struct mcc_deque *self;

	if (!element)
		return NULL;

	self = calloc(1, sizeof(struct mcc_deque));
	if (!self)
		return NULL;

	memcpy(&self->elem, element, sizeof(self->elem));
	return self;
}

void mcc_deque_delete(struct mcc_deque *self)
{
	if (!self)
		return;

	mcc_deque_clear(self);
	free(self->ptr);
	free(self);
}

mcc_err mcc_deque_reserve(struct mcc_deque *self, mcc_usize additional)
{
	mcc_usize new_capacity, needs;

	if (!self)
		return INVALID_ARGUMENTS;

	needs = self->len + additional;
	if (needs <= self->cap)
		return OK;

	new_capacity = !self->cap ? 4 : self->cap << 1;
	while (new_capacity < needs)
		new_capacity <<= 1;

	return grow(self, new_capacity);
}

mcc_err mcc_deque_push_front(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->cap && mcc_deque_reserve(self, 1) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	self->head = (self->head + self->cap - !!self->len) % self->cap;
	memcpy(get_ptr(self, 0), value, self->elem.size);
	self->len++;
	return OK;
}

mcc_err mcc_deque_push_back(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->cap && mcc_deque_reserve(self, 1) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	memcpy(get_ptr(self, self->len), value, self->elem.size);
	self->len++;
	return OK;
}

void mcc_deque_pop_front(struct mcc_deque *self)
{
	if (!self)
		return;

	if (self->len) {
		if (self->elem.dtor)
			self->elem.dtor(get_ptr(self, 0));
		self->len--;
		self->head = (self->head + !!self->len) % self->cap;
	}
}

void mcc_deque_pop_back(struct mcc_deque *self)
{
	if (!self)
		return;

	if (self->len) {
		if (self->elem.dtor)
			self->elem.dtor(get_ptr(self, self->len - 1));
		self->len--;
	}
}

mcc_err mcc_deque_insert(struct mcc_deque *self, mcc_usize index,
			 const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index && index >= self->len)
		return OUT_OF_RANGE;

	if (self->len >= self->cap && mcc_deque_reserve(self, 1) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	if (index == 0)
		return mcc_deque_push_front(self, value);
	else if (index == self->len - 1)
		return mcc_deque_push_back(self, value);
	else
		return insert_elem_in_the_mid(self, index, value);
}

void mcc_deque_remove(struct mcc_deque *self, mcc_usize index)
{
	if (!self || index >= self->len)
		return;

	if (index == 0)
		mcc_deque_pop_front(self);
	else if (index == self->len - 1)
		mcc_deque_pop_back(self);
	else
		remove_elem_in_the_mid(self, index);
}

void mcc_deque_clear(struct mcc_deque *self)
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

mcc_err mcc_deque_get(struct mcc_deque *self, mcc_usize index, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	memcpy(value, get_ptr(self, index), self->elem.size);
	return OK;
}

void *mcc_deque_get_ptr(struct mcc_deque *self, mcc_usize index)
{
	return !self || index >= self->len ? NULL : get_ptr(self, index);
}

mcc_err mcc_deque_front(struct mcc_deque *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, get_ptr(self, 0), self->elem.size);
	return OK;
}

void *mcc_deque_front_ptr(struct mcc_deque *self)
{
	return !self || !self->len ? NULL : get_ptr(self, 0);
}

mcc_err mcc_deque_back(struct mcc_deque *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, get_ptr(self, self->len - 1), self->elem.size);
	return OK;
}

void *mcc_deque_back_ptr(struct mcc_deque *self)
{
	return !self || !self->len ? NULL : get_ptr(self, self->len - 1);
}

mcc_usize mcc_deque_capacity(struct mcc_deque *self)
{
	return !self ? 0 : self->cap;
}

mcc_usize mcc_deque_len(struct mcc_deque *self)
{
	return !self ? 0 : self->len;
}

mcc_bool mcc_deque_is_empty(struct mcc_deque *self)
{
	return !self ? true : self->len == 0;
}

mcc_err mcc_deque_swap(struct mcc_deque *self, mcc_usize a, mcc_usize b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b >= self->len)
		return OUT_OF_RANGE;

	if (a == b)
		return OK;

	mcc_memswap(get_ptr(self, a), get_ptr(self, b), self->elem.size);
	return OK;
}

mcc_err mcc_deque_reverse(struct mcc_deque *self)
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

static void quick_sort(struct mcc_deque *self, mcc_usize low, mcc_usize high,
		       mcc_compare_fn cmp)
{
	mcc_usize mid = low;

	if (low >= high)
		return;

	for (mcc_usize i = low; i < high; i++) {
		if (cmp(get_ptr(self, i), get_ptr(self, high)) < 0)
			mcc_deque_swap(self, i, mid++);
	}

	mcc_deque_swap(self, high, mid);

	if (mid)
		quick_sort(self, low, mid - 1, cmp);
	quick_sort(self, mid + 1, high, cmp);
}

mcc_err mcc_deque_sort(struct mcc_deque *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	quick_sort(self, 0, self->len - 1, self->elem.cmp);
	return OK;
}

mcc_err mcc_deque_iter_init(struct mcc_deque *self, struct mcc_deque_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_deque_iter_next;
	iter->index = 0;
	iter->container = self;
	return OK;
}

mcc_bool mcc_deque_iter_next(struct mcc_deque_iter *iter, void *result)
{
	if (!iter || iter->index >= iter->container->len)
		return false;

	if (result)
		mcc_deque_get(iter->container, iter->index, result);
	iter->index++;
	return true;
}
