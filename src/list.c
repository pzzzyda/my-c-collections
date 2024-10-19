#include "mcc_err.h"
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

struct mcc_list_iter {
	struct mcc_list_iter *next;
	struct mcc_list *list;
	struct mcc_list_node *curr;
	bool in_use;
};

struct mcc_list {
	const struct mcc_object_interface *T;
	struct mcc_list_iter *iters;
	struct mcc_list_node *head;
	struct mcc_list_node *tail;
	size_t len;
};

static inline void *value_of(struct mcc_list_node *self)
{
	return (uint8_t *)self + sizeof(struct mcc_list_node);
}

static struct mcc_list_node *create_node(const void *value, size_t size)
{
	struct mcc_list_node *node;

	node = calloc(1, sizeof(struct mcc_list_node) + size);
	if (!node)
		return NULL;

	memcpy(value_of(node), value, size);
	return node;
}

static void destroy_node(struct mcc_list_node *node, const mcc_drop_fn drop)
{
	if (drop)
		drop(value_of(node));
	free(node);
}

static struct mcc_list_node *get_nth(struct mcc_list *self, size_t index)
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

static int insert_element(struct mcc_list *self, size_t index,
			  const void *value)
{
	struct mcc_list_node *curr, *new_node;

	curr = get_nth(self, index);

	new_node = create_node(value, self->T->size);
	if (!new_node)
		return CANNOT_ALLOCATE_MEMORY;

	new_node->next = curr;
	new_node->prev = curr->prev;
	new_node->prev->next = new_node;
	new_node->next->prev = new_node;

	self->len++;
	return OK;
}

static void remove_element(struct mcc_list *self, size_t index)
{
	struct mcc_list_node *curr;

	curr = get_nth(self, index);

	curr->next->prev = curr->prev;
	curr->prev->next = curr->next;

	destroy_node(curr, self->T->drop);
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

void mcc_list_drop(struct mcc_list *self)
{
	struct mcc_list_iter *tmp;

	if (!self)
		return;

	mcc_list_clear(self);
	while (self->iters) {
		tmp = self->iters->next;
		free(self->iters);
		self->iters = tmp;
	}
	free(self);
}

int mcc_list_push_front(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = create_node(value, self->T->size);
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

int mcc_list_push_back(struct mcc_list *self, const void *value)
{
	struct mcc_list_node *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	new_node = create_node(value, self->T->size);
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

	if (!self || !self->len)
		return;

	tmp = self->head;
	self->head = self->head->next;
	if (self->head)
		self->head->prev = NULL;
	self->len--;

	destroy_node(tmp, self->T->drop);
}

void mcc_list_pop_back(struct mcc_list *self)
{
	struct mcc_list_node *tmp;

	if (!self || !self->len)
		return;

	tmp = self->tail;
	self->tail = self->tail->prev;
	if (self->tail)
		self->tail->next = NULL;
	self->len--;

	destroy_node(tmp, self->T->drop);
}

int mcc_list_insert(struct mcc_list *self, size_t index, const void *value)
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
		return insert_element(self, index, value);
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
		remove_element(self, index);
}

void mcc_list_clear(struct mcc_list *self)
{
	if (!self)
		return;

	while (self->head) {
		self->tail = self->head->next;
		destroy_node(self->head, self->T->drop);
		self->head = self->tail;
		self->len--;
	}
}

int mcc_list_front(struct mcc_list *self, void **ref)
{
	if (!self || !ref)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	*ref = value_of(self->head);
	return OK;
}

int mcc_list_back(struct mcc_list *self, void **ref)
{
	if (!self || !ref)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	*ref = value_of(self->tail);
	return OK;
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

static void merge(struct mcc_list_node *a, struct mcc_list_node *b,
		  struct mcc_list_node *result, mcc_compare_fn cmp)
{
	struct mcc_list_node *tmp = NULL;

	while (a->head && b->head) {
		if (cmp(value_of(a->head), value_of(b->head)) <= 0) {
			tmp = a->head;
			a->head = a->head->next;
		} else {
			tmp = b->head;
			b->head = b->head->next;
		}

		if (!result->tail) {
			result->tail = result->head = tmp;
		} else {
			result->tail->next = tmp;
			tmp->prev = result->tail;
			result->tail = tmp;
		}
	}

	if (a->head) {
		if (!result->tail) {
			*result = *a;
		} else {
			result->tail->next = a->head;
			a->head->prev = result->tail;
			result->tail = a->tail;
		}
	}

	if (b->head) {
		if (!result->tail) {
			*result = *b;
		} else {
			result->tail->next = b->head;
			b->head->prev = result->tail;
			result->tail = b->tail;
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
	merge(&left, &right, result, cmp);
}

int mcc_list_sort(struct mcc_list *self)
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

struct mcc_list_iter *mcc_list_iter_new(struct mcc_list *list)
{
	struct mcc_list_iter *self;

	if (!list)
		return NULL;

	self = list->iters;
	while (self) {
		if (!self->in_use)
			goto reset_iterator;
		self = self->next;
	}

	self = calloc(1, sizeof(struct mcc_list_iter));
	if (!self)
		return NULL;

	self->next = list->iters;
	list->iters = self;
	self->list = list;
reset_iterator:
	self->curr = list->head;
	self->in_use = true;
	return self;
}

void mcc_list_iter_drop(struct mcc_list_iter *self)
{
	if (self)
		self->in_use = false;
}

bool mcc_list_iter_next(struct mcc_list_iter *self, void **ref)
{
	if (!self || !ref || !self->curr)
		return false;

	*ref = value_of(self->curr);
	self->curr = self->curr->next;
	return true;
}
