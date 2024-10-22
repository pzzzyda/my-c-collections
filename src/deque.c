#include "mcc_deque.h"
#include "mcc_err.h"
#include "memswap.h"
#include <stdlib.h>

struct mcc_deque_iter {
	struct mcc_deque_iter *next;
	struct mcc_deque *deque;
	size_t curr;
	bool in_use;
};

struct mcc_deque {
	const struct mcc_object_interface *T;
	struct mcc_deque_iter *iters;
	uint8_t *ptr;
	size_t len;
	size_t capacity;
	size_t head;
};

static inline size_t to_physical_index(struct mcc_deque *self,
				       size_t logic_index)
{
	return (self->head + logic_index) % self->capacity;
}

static inline void *get(struct mcc_deque *self, size_t index)
{
	return self->ptr + to_physical_index(self, index) * self->T->size;
}

static inline void move(struct mcc_deque *self, size_t to, size_t from,
			size_t n)
{
	memmove(get(self, to), get(self, from), n * self->T->size);
}

static inline int reserve(struct mcc_deque *self, size_t additional)
{
	uint8_t *new_ptr;
	size_t min_capacity, new_capacity;

	min_capacity = self->len + additional;
	if (min_capacity <= self->capacity)
		return OK;

	new_capacity = !self->capacity ? 8 : self->capacity << 1;
	while (new_capacity < min_capacity)
		new_capacity <<= 1;

	new_ptr = realloc(self->ptr, new_capacity * self->T->size);
	if (!new_ptr)
		return CANNOT_ALLOCATE_MEMORY;

	self->ptr = new_ptr;
	self->capacity = new_capacity;
	return OK;
}

static inline void insert_element(struct mcc_deque *self, size_t index,
				  const void *value)
{
	if (to_physical_index(self, index) <=
	    to_physical_index(self, self->len - 1)) {
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
		move(self, index + 1, index, self->len - index);
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
		self->head--;
		move(self, 0, 1, index);
	}
	memcpy(get(self, index), value, self->T->size);
	self->len++;
}

static inline void remove_element(struct mcc_deque *self, size_t index)
{
	if (self->T->drop)
		self->T->drop(get(self, index));

	/* see "insert_element" */
	if (to_physical_index(self, index) <=
	    to_physical_index(self, self->len - 1)) {
		move(self, index, index + 1, self->len - index - 1);
	} else {
		move(self, 1, 0, index);
		self->head++;
	}
	self->len--;
}

struct mcc_deque *mcc_deque_new(const struct mcc_object_interface *T)
{
	struct mcc_deque *self;

	if (!T)
		return NULL;

	self = calloc(1, sizeof(struct mcc_deque));
	if (!self)
		return NULL;

	self->T = T;
	return self;
}

void mcc_deque_drop(struct mcc_deque *self)
{
	struct mcc_deque_iter *next;

	if (!self)
		return;

	while (self->iters) {
		next = self->iters->next;
		free(self->iters);
		self->iters = next;
	}
	mcc_deque_clear(self);
	free(self->ptr);
	free(self);
}

int mcc_deque_reserve(struct mcc_deque *self, size_t additional)
{
	size_t diff, old_capacity;

	if (!self)
		return INVALID_ARGUMENTS;

	old_capacity = self->capacity;
	if (reserve(self, additional))
		return CANNOT_ALLOCATE_MEMORY;

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
		diff = self->capacity - old_capacity;
		move(self, diff, 0, old_capacity - self->head);
		self->head += diff;
	}
	return OK;
}

int mcc_deque_push_front(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->capacity) {
		if (mcc_deque_reserve(self, 1))
			return CANNOT_ALLOCATE_MEMORY;
	}

	/*
	 * Decrease self->head. "!!self->len" ensures that when the first
	 * element is added, the self->head remains the same.
	 */
	self->head += self->capacity;
	self->head -= !!self->len;
	self->head %= self->capacity;

	memcpy(get(self, 0), value, self->T->size);
	self->len++;
	return OK;
}

int mcc_deque_push_back(struct mcc_deque *self, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (self->len >= self->capacity) {
		if (mcc_deque_reserve(self, 1))
			return CANNOT_ALLOCATE_MEMORY;
	}

	memcpy(get(self, self->len++), value, self->T->size);
	return OK;
}

void mcc_deque_pop_front(struct mcc_deque *self)
{
	if (!self || !self->len)
		return;

	if (self->T->drop)
		self->T->drop(get(self, 0));
	self->len--;

	/*
	 * Increase self->head. "!!self->len" ensures that when the last element
	 * is removed, the self->head remains the same.
	 */
	self->head += !!self->len;
	self->head %= self->capacity;
}

void mcc_deque_pop_back(struct mcc_deque *self)
{
	if (!self || !self->len)
		return;

	if (self->T->drop)
		self->T->drop(get(self, self->len - 1));
	self->len--;
}

int mcc_deque_insert(struct mcc_deque *self, size_t index, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (index == 0) {
		return mcc_deque_push_front(self, value);
	} else if (index == self->len) {
		return mcc_deque_push_back(self, value);
	} else {
		if (self->len >= self->capacity) {
			if (mcc_deque_reserve(self, 1))
				return CANNOT_ALLOCATE_MEMORY;
		}

		insert_element(self, index, value);
		return OK;
	}
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
		remove_element(self, index);
}

void mcc_deque_clear(struct mcc_deque *self)
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

int mcc_deque_set(struct mcc_deque *self, size_t index, const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	if (self->T->drop)
		self->T->drop(get(self, index));
	memcpy(get(self, index), value, self->T->size);
	return OK;
}

int mcc_deque_get(struct mcc_deque *self, size_t index, void **ref)
{
	if (!self || !ref)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	*ref = get(self, index);
	return OK;
}

int mcc_deque_front(struct mcc_deque *self, void **ref)
{
	return mcc_deque_get(self, 0, ref);
}

int mcc_deque_back(struct mcc_deque *self, void **ref)
{
	return mcc_deque_get(self, self->len - 1, ref);
}

size_t mcc_deque_capacity(struct mcc_deque *self)
{
	return !self ? 0 : self->capacity;
}

size_t mcc_deque_len(struct mcc_deque *self)
{
	return !self ? 0 : self->len;
}

bool mcc_deque_is_empty(struct mcc_deque *self)
{
	return !self ? true : self->len == 0;
}

int mcc_deque_swap(struct mcc_deque *self, size_t a, size_t b)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (a >= self->len || b >= self->len)
		return OUT_OF_RANGE;

	if (a != b)
		memswap(get(self, a), get(self, b), self->T->size);
	return OK;
}

int mcc_deque_reverse(struct mcc_deque *self)
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

static void quick_sort(struct mcc_deque *self, size_t low, size_t high)
{
	size_t mid = low;
	size_t i = low;
	const void *pivot = get(self, high);

	for (; i < high; i++) {
		if (self->T->cmp(get(self, i), pivot) < 0)
			mcc_deque_swap(self, i, mid++);
	}

	mcc_deque_swap(self, mid, high);

	if (low + 1 < mid)
		quick_sort(self, low, mid - 1);
	if (mid + 1 < high)
		quick_sort(self, mid + 1, high);
}

int mcc_deque_sort(struct mcc_deque *self)
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
		cmp_res = self->T->cmp(key, get(self, mid));
		if (cmp_res > 0)
			low = mid + 1;
		else if (cmp_res < 0)
			high = mid - 1;
		else
			return get(self, mid);
	}

	return NULL;
}

struct mcc_deque_iter *mcc_deque_iter_new(struct mcc_deque *deque)
{
	struct mcc_deque_iter *self;

	if (!deque)
		return NULL;

	self = deque->iters;
	while (self) {
		if (!self->in_use)
			goto reset_iterator;
		self = self->next;
	}

	self = calloc(1, sizeof(struct mcc_deque_iter));
	if (!self)
		return NULL;

	self->next = deque->iters;
	deque->iters = self;
	self->deque = deque;
reset_iterator:
	self->curr = 0;
	self->in_use = true;
	return self;
}

void mcc_deque_iter_drop(struct mcc_deque_iter *self)
{
	if (self)
		self->in_use = false;
}

bool mcc_deque_iter_next(struct mcc_deque_iter *self, void **ref)
{
	if (!self || !ref || self->curr >= self->deque->len)
		return false;

	*ref = get(self->deque, self->curr++);
	return true;
}
