#include "mcc_list.h"
#include <stdlib.h>
#include <string.h>

struct mcc_list_node {
	union {
		struct mcc_list_node *prev;
		struct mcc_list_node *head;
	};
	union {
		struct mcc_list_node *next;
		struct mcc_list_node *tail;
	};
};

struct mcc_list {
	const struct mcc_object_interface *T;

	struct mcc_list_node *head;
	struct mcc_list_node *tail;
	size_t len;
};

static void mcc_list_dtor(void *self)
{
	mcc_list_delete(*(struct mcc_list **)self);
}

static int mcc_list_cmp(const void *self, const void *other)
{
	struct mcc_list *const *p1 = self;
	struct mcc_list *const *p2 = other;

	return SIZE_T->cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_list_hash(const void *self)
{
	struct mcc_list *const *p = self;

	return SIZE_T->hash(&(**p).len);
}

const struct mcc_object_interface __mcc_list_obj_intf = {
	.size = sizeof(struct mcc_list *),
	.dtor = &mcc_list_dtor,
	.cmp = &mcc_list_cmp,
	.hash = &mcc_list_hash,
};

static inline void *value_of(struct mcc_list_node *self)
{
	return (uint8_t *)self + sizeof(struct mcc_list_node);
}

static struct mcc_list_node *node_new(const void *value,
				      const struct mcc_object_interface *T)
{
	struct mcc_list_node *self;
	size_t n = 0;

	n += sizeof(struct mcc_list_node);
	n += T->size;
	self = calloc(1, n);
	if (!self)
		return NULL;

	memcpy(value_of(self), value, T->size);
	return self;
}

static void node_delete(struct mcc_list_node *self,
			const struct mcc_object_interface *T)
{
	if (T->dtor)
		T->dtor(value_of(self));
	free(self);
}

static inline struct mcc_list_node *get_nth(struct mcc_list *self, size_t index)
{
	struct mcc_list_node *curr;
	size_t i;

	if (index <= self->len >> 1) {
		curr = self->head;
		for (i = 0; i < index; i++)
			curr = curr->next;
	} else {
		curr = self->tail;
		for (i = self->len - 1; i > index; i--)
			curr = curr->prev;
	}

	return curr;
}

static inline mcc_err_t insert(struct mcc_list *self, size_t index,
			       const void *value)
{
	struct mcc_list_node *curr, *new_node;

	curr = get_nth(self, index);

	new_node = node_new(value, self->T);
	if (!new_node)
		return CANNOT_ALLOCATE_MEMORY;

	new_node->next = curr;
	new_node->prev = curr->prev;
	new_node->prev->next = new_node;
	new_node->next->prev = new_node;

	self->len++;
	return OK;
}

static inline void remove(struct mcc_list *self, size_t index)
{
	struct mcc_list_node *curr;

	curr = get_nth(self, index);

	curr->next->prev = curr->prev;
	curr->prev->next = curr->next;

	node_delete(curr, self->T);
	self->len--;
}

struct mcc_list *mcc_list_new(const struct mcc_object_interface *T)
{
	struct mcc_list *self;

	if (!T)
		return NULL;

	self = calloc(1, sizeof(struct mcc_list));
	if (!self)
		return NULL;

	self->T = T;
	return self;
}

void mcc_list_delete(struct mcc_list *self)
{
	if (!self)
		return;

	mcc_list_clear(self);
	free(self);
}

mcc_err_t mcc_list_push_front(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = node_new(value, self->T);
	if (!new_node)
		return CANNOT_ALLOCATE_MEMORY;

	new_node->next = self->head;
	if (self->head)
		self->head->prev = new_node;

	self->head = new_node;
	if (!self->len)
		self->tail = new_node;

	self->len++;
	return OK;
}

mcc_err_t mcc_list_push_back(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = node_new(value, self->T);
	if (!new_node)
		return CANNOT_ALLOCATE_MEMORY;

	new_node->prev = self->tail;
	if (self->tail)
		self->tail->next = new_node;

	self->tail = new_node;
	if (!self->len)
		self->head = new_node;

	self->len++;
	return OK;
}

void mcc_list_pop_front(struct mcc_list *self)
{
	struct mcc_list_node *tmp;

	if (!self)
		return;

	if (self->len) {
		tmp = self->head;
		self->head = self->head->next;

		node_delete(tmp, self->T);

		if (self->head)
			self->head->prev = NULL;
		self->len--;
		if (!self->len)
			self->tail = NULL;
	}
}

void mcc_list_pop_back(struct mcc_list *self)
{
	struct mcc_list_node *tmp;

	if (!self)
		return;

	if (self->len) {
		tmp = self->tail;
		self->tail = self->tail->prev;

		node_delete(tmp, self->T);

		if (self->tail)
			self->tail->next = NULL;
		self->len--;
		if (!self->len)
			self->head = NULL;
	}
}

mcc_err_t mcc_list_insert(struct mcc_list *self, size_t index,
			  const void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index > self->len)
		return OUT_OF_RANGE;

	if (index == 0)
		return mcc_list_push_front(self, value);
	else if (index == self->len)
		return mcc_list_push_back(self, value);
	else
		return insert(self, index, value);
}

void mcc_list_remove(struct mcc_list *self, size_t index)
{
	if (!self || index >= self->len)
		return;

	if (index == 0)
		mcc_list_pop_front(self);
	else if (index == self->len - 1)
		mcc_list_pop_back(self);
	else
		remove(self, index);
}

void mcc_list_clear(struct mcc_list *self)
{
	if (!self)
		return;

	while (self->head) {
		self->tail = self->head->next;
		node_delete(self->head, self->T);
		self->head = self->tail;
		self->len--;
	}
}

mcc_err_t mcc_list_front(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, value_of(self->head), self->T->size);
	return OK;
}

void *mcc_list_front_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : value_of(self->head);
}

mcc_err_t mcc_list_back(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, value_of(self->tail), self->T->size);
	return OK;
}

void *mcc_list_back_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : value_of(self->tail);
}

size_t mcc_list_len(struct mcc_list *self)
{
	return !self ? 0 : self->len;
}

bool mcc_list_is_empty(struct mcc_list *self)
{
	return !self ? true : self->len == 0;
}

static struct mcc_list_node *split(struct mcc_list_node *head)
{
	struct mcc_list_node *prev, *slow, *fast;

	if (!head || !head->next)
		return NULL;

	prev = head;
	slow = head->next;
	fast = head->next->next;

	while (fast && fast->next) {
		prev = slow;
		slow = slow->next;
		fast = fast->next->next;
	}

	prev->next = NULL;
	slow->prev = NULL;

	return slow;
}

static inline void merge(struct mcc_list_node *a, struct mcc_list_node *b,
			 struct mcc_list_node *res, mcc_compare_fn cmp)
{
	struct mcc_list_node *tmp = NULL;

	while (a || b) {
		if (!b || (a && cmp(value_of(a), value_of(b)) < 0)) {
			tmp = a;
			a = a->next;
		} else {
			tmp = b;
			b = b->next;
		}

		if (!res->head) {
			tmp->prev = tmp->next = NULL;
			res->head = res->tail = tmp;
		} else {
			res->tail->next = tmp;
			tmp->prev = res->tail;
			tmp->next = NULL;
			res->tail = tmp;
		}
	}
}

static void merge_sort(struct mcc_list_node *head, struct mcc_list_node *result,
		       mcc_compare_fn cmp)
{
	struct mcc_list_node *mid = NULL;
	struct mcc_list_node left = {0};
	struct mcc_list_node right = {0};

	if (!head || !head->next) {
		result->head = head;
		result->tail = head;
		return;
	}

	mid = split(head);
	merge_sort(head, &left, cmp);
	merge_sort(mid, &right, cmp);
	merge(left.head, right.head, result, cmp);
}

mcc_err_t mcc_list_sort(struct mcc_list *self)
{
	struct mcc_list_node sorted = {0};

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	merge_sort(self->head, &sorted, self->T->cmp);
	self->head = sorted.head;
	self->tail = sorted.tail;
	return OK;
}

static const struct mcc_iterator_interface mcc_list_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_list_iter_next,
};

mcc_err_t mcc_list_iter_init(struct mcc_list *self, struct mcc_list_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->base.iter_intf = &mcc_list_iter_intf;
	iter->curr = self->head;
	iter->container = self;
	return OK;
}

bool mcc_list_iter_next(struct mcc_list_iter *self, void *result)
{
	if (!self || !result)
		return false;

	if (!self->curr)
		return false;

	memcpy(result, value_of(self->curr), self->container->T->size);
	self->curr = self->curr->next;
	return true;
}
