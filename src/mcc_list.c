#include "mcc_list.h"
#include "mcc_err.h"
#include "mcc_type.h"
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
	struct mcc_list_node *head;
	struct mcc_list_node *tail;
	size_t len;
	struct mcc_object_interface elem;
};

static void mcc_list_dtor(void *self)
{
	mcc_list_delete(*(struct mcc_list **)self);
}

const struct mcc_object_interface mcc_list_object_interface = {
	.size = sizeof(struct mcc_list *),
	.dtor = mcc_list_dtor,
};

static inline void *get_data_ptr(struct mcc_list_node *self)
{
	return (uint8_t *)self + sizeof(struct mcc_list_node);
}

static inline void get_data(struct mcc_list_node *self, void *value,
			    const struct mcc_object_interface *i)
{
	memcpy(value, get_data_ptr(self), i->size);
}

static struct mcc_list_node *node_new(const void *value,
				      const struct mcc_object_interface *i)
{
	struct mcc_list_node *self;
	self = malloc(sizeof(struct mcc_list_node) + i->size);
	if (!self)
		return NULL;

	self->prev = NULL;
	self->next = NULL;
	memcpy(get_data_ptr(self), value, i->size);
	return self;
}

static void node_delete(struct mcc_list_node *self,
			const struct mcc_object_interface *i)
{
	if (i->dtor)
		i->dtor(get_data_ptr(self));
	free(self);
}

struct mcc_list *mcc_list_new(const struct mcc_object_interface *element)
{
	struct mcc_list *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(struct mcc_list));
	if (!self)
		return NULL;

	self->head = NULL;
	self->tail = NULL;
	self->len = 0;
	self->elem = *element;
	return self;
}

void mcc_list_delete(struct mcc_list *self)
{
	if (!self)
		return;

	mcc_list_clear(self);
	free(self);
}

int mcc_list_push_front(struct mcc_list *self, const void *value)
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
	if (!self->tail)
		self->tail = new_node;
	self->len++;
	return OK;
}

int mcc_list_push_back(struct mcc_list *self, const void *value)
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
	if (!self->head)
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
		tmp = self->head->next;
		node_delete(self->head, &self->elem);
		self->head = tmp;
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
		tmp = self->tail->prev;
		node_delete(self->tail, &self->elem);
		self->tail = tmp;
		if (self->tail)
			self->tail->next = NULL;
		self->len--;
		if (!self->len)
			self->head = NULL;
	}
}

int mcc_list_insert(struct mcc_list *self, size_t index, const void *value)
{
	struct mcc_list_node *curr, *new_node;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (index && index >= self->len)
		return OUT_OF_RANGE;

	if (index == 0) {
		return mcc_list_push_front(self, value);
	} else if (index == self->len - 1) {
		return mcc_list_push_back(self, value);
	} else {
		if (index <= self->len >> 1) {
			curr = self->head;
			for (size_t i = 0; i < index; i++)
				curr = curr->next;
		} else {
			curr = self->tail;
			for (size_t i = self->len - 1; i > index; i--)
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
}

void mcc_list_remove(struct mcc_list *self, size_t index)
{
	struct mcc_list_node *curr;

	if (!self || index >= self->len)
		return;

	if (index == 0) {
		mcc_list_pop_front(self);
	} else if (index == self->len - 1) {
		mcc_list_pop_back(self);
	} else {
		if (index <= self->len >> 1) {
			curr = self->head;
			for (size_t i = 0; i < index; i++)
				curr = curr->next;
		} else {
			curr = self->tail;
			for (size_t i = self->len - 1; i > index; i--)
				curr = curr->prev;
		}

		curr->next->prev = curr->prev;
		curr->prev->next = curr->next;
		node_delete(curr, &self->elem);
		self->len--;
	}
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

int mcc_list_front(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	get_data(self->head, value, &self->elem);
	return OK;
}

void *mcc_list_front_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : get_data_ptr(self->head);
}

int mcc_list_back(struct mcc_list *self, void *value)
{
	if (!self || !value)
		return INVALID_ARGUMENTS;

	if (!self->len)
		return NONE;

	get_data(self->tail, value, &self->elem);
	return OK;
}

void *mcc_list_back_ptr(struct mcc_list *self)
{
	return !self || !self->len ? NULL : get_data_ptr(self->tail);
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

static inline void merge(struct mcc_list_node *left,
			 struct mcc_list_node *right,
			 struct mcc_list_node *result, mcc_compare_f cmp)
{
	struct mcc_list_node *tmp = NULL;

	while (left || right) {
		if ((left && right &&
		     cmp(get_data_ptr(left), get_data_ptr(right)) < 0) ||
		    !right) {
			tmp = left;
			left = left->next;
		} else {
			tmp = right;
			right = right->next;
		}

		if (!result->head) {
			tmp->prev = NULL;
			tmp->next = NULL;
			result->head = tmp;
			result->tail = tmp;
		} else {
			result->tail->next = tmp;
			tmp->prev = result->tail;
			tmp->next = NULL;
			result->tail = tmp;
		}
	}
}

static void merge_sort(struct mcc_list_node *head, struct mcc_list_node *result,
		       mcc_compare_f cmp)
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

int mcc_list_sort(struct mcc_list *self, mcc_compare_f cmp)
{
	struct mcc_list_node sorted = {0};

	if (!self || !cmp)
		return INVALID_ARGUMENTS;

	if (self->len <= 1)
		return OK;

	merge_sort(self->head, &sorted, cmp);
	self->head = sorted.head;
	self->tail = sorted.tail;
	return OK;
}

static bool mcc_list_iter_next(struct mcc_list_iter *self, void *value)
{
	if (!self || !self->curr)
		return false;

	if (value)
		get_data(self->curr, value, &self->list->elem);
	self->curr = self->curr->next;
	return true;
}

int mcc_list_iter_init(struct mcc_list *self, struct mcc_list_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->curr = self->head;
	iter->list = self;
	iter->next = mcc_list_iter_next;
	return OK;
}
