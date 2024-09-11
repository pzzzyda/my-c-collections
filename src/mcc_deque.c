#include "mcc_deque.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_deque {
	uint8_t *ptr;
	size_t head;
	size_t len;
	size_t cap;
	struct mcc_object_interface elem;
};

static void mcc_deque_dtor(void *self)
{
	mcc_deque_delete(*(struct mcc_deque **)self);
}

const struct mcc_object_interface mcc_deque_object_interface = {
	.size = sizeof(struct mcc_deque *),
	.dtor = mcc_deque_dtor,
};

static inline size_t mod_idx(struct mcc_deque *self, size_t index)
{
	return (self->head + index) % self->cap;
}

static inline void *get_ptr(struct mcc_deque *self, size_t index)
{
	return self->ptr + mod_idx(self, index) * self->elem.size;
}

static inline void move_elems(struct mcc_deque *self, size_t dest, size_t src,
			      size_t n)
{
	memmove(self->ptr + dest * self->elem.size,
		self->ptr + src * self->elem.size, n * self->elem.size);
}

static inline int insert_elem_in_the_mid(struct mcc_deque *self, size_t index,
					 const void *value)
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

static inline int remove_elem_in_the_mid(struct mcc_deque *self, size_t index)
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

static int grow(struct mcc_deque *self, size_t new_capacity)
{
	size_t diff, old_capacity;
	uint8_t *tmp;

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

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(struct mcc_deque));
	if (!self)
		return NULL;

	self->ptr = NULL;
	self->len = 0;
	self->head = 0;
	self->cap = 0;
	self->elem = *element;
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

int mcc_deque_reserve(struct mcc_deque *self, size_t additional)
{
	size_t new_capacity, needs;

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

int mcc_deque_push_front(struct mcc_deque *self, const void *value)
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

int mcc_deque_push_back(struct mcc_deque *self, const void *value)
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

int mcc_deque_insert(struct mcc_deque *self, size_t index, const void *value)
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

void mcc_deque_remove(struct mcc_deque *self, size_t index)
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

int mcc_deque_get(struct mcc_deque *self, size_t index, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index >= self->len)
		return NONE;

	memcpy(value, get_ptr(self, index), self->elem.size);
	return OK;
}

void *mcc_deque_get_ptr(struct mcc_deque *self, size_t index)
{
	return !self || index >= self->len ? NULL : get_ptr(self, index);
}

int mcc_deque_front(struct mcc_deque *self, void *value)
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

int mcc_deque_back(struct mcc_deque *self, void *value)
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

size_t mcc_deque_capacity(struct mcc_deque *self)
{
	return !self ? 0 : self->cap;
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

	if (a == b)
		return OK;

	mcc_memswap(get_ptr(self, a), get_ptr(self, b), self->elem.size);
	return OK;
}

int mcc_deque_reverse(struct mcc_deque *self)
{
	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	for (size_t i = 0, j = self->len - 1; i < j; i++, j--)
		mcc_memswap(get_ptr(self, i), get_ptr(self, j),
			    self->elem.size);
	return OK;
}

static void quick_sort(struct mcc_deque *self, size_t low, size_t high,
		       mcc_compare_f cmp)
{
	size_t mid = low;

	if (low >= high)
		return;

	for (size_t i = low; i < high; i++) {
		if (cmp(get_ptr(self, i), get_ptr(self, high)) < 0)
			mcc_deque_swap(self, i, mid++);
	}

	mcc_deque_swap(self, high, mid);

	if (mid)
		quick_sort(self, low, mid - 1, cmp);
	quick_sort(self, mid + 1, high, cmp);
}

int mcc_deque_sort(struct mcc_deque *self, mcc_compare_f cmp)
{
	if (!self || !cmp)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	quick_sort(self, 0, self->len - 1, cmp);
	return OK;
}

static bool mcc_deque_iter_next(struct mcc_deque_iter *self, void *value)
{
	if (!self || self->idx >= self->container->len)
		return false;

	if (value)
		mcc_deque_get(self->container, self->idx, value);
	self->idx++;
	return true;
}

int mcc_deque_iter_init(struct mcc_deque *self, struct mcc_deque_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->idx = 0;
	iter->container = self;
	iter->next = mcc_deque_iter_next;
	return OK;
}
