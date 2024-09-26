#include "mcc_list.h"
#include "mcc_err.h"
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
	void *data;
};

struct mcc_list {
	struct mcc_list_node *head;
	struct mcc_list_node *tail;
	mcc_usize len;
	struct mcc_object_interface elem;
};

static void mcc_list_dtor(void *self)
{
	mcc_list_delete(*(struct mcc_list **)self);
}

static mcc_i32 mcc_list_cmp(const void *self, const void *other)
{
	struct mcc_list *const *p1 = self;
	struct mcc_list *const *p2 = other;

	return mcc_usize_i.cmp(&(**p1).len, &(**p2).len);
}

static mcc_usize mcc_list_hash(const void *self)
{
	struct mcc_list *const *p = self;

	return mcc_usize_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_list_i = {
	.size = sizeof(struct mcc_list *),
	.dtor = &mcc_list_dtor,
	.cmp = &mcc_list_cmp,
	.hash = &mcc_list_hash,
};

static struct mcc_list_node *node_new(const void *value,
				      const struct mcc_object_interface *elem)
{
	struct mcc_list_node *self;

	self = calloc(1, sizeof(struct mcc_list_node));
	if (!self)
		return NULL;

	self->data = calloc(1, elem->size);
	if (!self->data) {
		free(self);
		return NULL;
	}

	memcpy(self->data, value, elem->size);
	return self;
}

static void node_delete(struct mcc_list_node *self,
			const struct mcc_object_interface *elem)
{
	if (elem->dtor)
		elem->dtor(self->data);
	free(self->data);
	free(self);
}

static inline mcc_err insert(struct mcc_list *self, mcc_usize index,
			     const void *value)
{
	struct mcc_list_node *curr, *new_node;
	mcc_usize i;

	if (index <= self->len >> 1) {
		curr = self->head;
		for (i = 0; i < index; i++)
			curr = curr->next;
	} else {
		curr = self->tail;
		for (i = self->len - 1; i > index; i--)
			curr = curr->prev;
	}

	new_node = node_new(value, &self->elem);
	if (!new_node)
		return CANNOT_ALLOCATE_MEMORY;

	new_node->next = curr;
	new_node->prev = curr->prev;
	new_node->prev->next = new_node;
	new_node->next->prev = new_node;
	self->len++;
	return OK;
}

static inline void remove(struct mcc_list *self, mcc_usize index)
{
	struct mcc_list_node *curr;
	mcc_usize i;

	if (index <= self->len >> 1) {
		curr = self->head;
		for (i = 0; i < index; i++)
			curr = curr->next;
	} else {
		curr = self->tail;
		for (i = self->len - 1; i > index; i--)
			curr = curr->prev;
	}

	curr->next->prev = curr->prev;
	curr->prev->next = curr->next;
	node_delete(curr, &self->elem);
	self->len--;
}

struct mcc_list *mcc_list_new(const struct mcc_object_interface *element)
{
	struct mcc_list *self;

	if (!element)
		return NULL;

	self = calloc(1, sizeof(struct mcc_list));
	if (!self)
		return NULL;

	memcpy(&self->elem, element, sizeof(self->elem));
	return self;
}

void mcc_list_delete(struct mcc_list *self)
{
	if (!self)
		return;

	mcc_list_clear(self);
	free(self);
}

mcc_err mcc_list_push_front(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = node_new(value, &self->elem);
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

mcc_err mcc_list_push_back(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = node_new(value, &self->elem);
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

		node_delete(tmp, &self->elem);

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

		node_delete(tmp, &self->elem);

		if (self->tail)
			self->tail->next = NULL;
		self->len--;
		if (!self->len)
			self->head = NULL;
	}
}

mcc_err mcc_list_insert(struct mcc_list *self, mcc_usize index,
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

void mcc_list_remove(struct mcc_list *self, mcc_usize index)
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
		node_delete(self->head, &self->elem);
		self->head = self->tail;
		self->len--;
	}
}

mcc_err mcc_list_front(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, self->head->data, self->elem.size);
	return OK;
}

void *mcc_list_front_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : self->head->data;
}

mcc_err mcc_list_back(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	memcpy(value, self->tail->data, self->elem.size);
	return OK;
}

void *mcc_list_back_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : self->tail->data;
}

mcc_usize mcc_list_len(struct mcc_list *self)
{
	return !self ? 0 : self->len;
}

mcc_bool mcc_list_is_empty(struct mcc_list *self)
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
		if (!b || (a && cmp(a->data, b->data) < 0)) {
			tmp = a;
			a = a->next;
		} else {
			tmp = b;
			b = b->next;
		}

		if (!res->head) {
			tmp->prev = NULL;
			tmp->next = NULL;
			res->head = tmp;
			res->tail = tmp;
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

mcc_err mcc_list_sort(struct mcc_list *self)
{
	struct mcc_list_node sorted = {0};

	if (!self)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	merge_sort(self->head, &sorted, self->elem.cmp);
	self->head = sorted.head;
	self->tail = sorted.tail;
	return OK;
}

mcc_err mcc_list_iter_init(struct mcc_list *self, struct mcc_list_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_list_iter_next;
	iter->current = self->head;
	iter->container = self;
	return OK;
}

mcc_bool mcc_list_iter_next(struct mcc_list_iter *iter, void *result)
{
	if (!iter || !iter->current)
		return false;

	if (result)
		memcpy(result, iter->current->data, iter->container->elem.size);
	iter->current = iter->current->next;
	return true;
}
