#ifndef _MCC_DEQUE_H
#define _MCC_DEQUE_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"

struct mcc_deque;

struct mcc_deque_iter {
	struct mcc_iter base;

	size_t idx;
	struct mcc_deque *container;
};

struct mcc_deque *mcc_deque_new(const struct mcc_object_interface *T);

void mcc_deque_delete(struct mcc_deque *self);

int mcc_deque_reserve(struct mcc_deque *self, size_t additional);

int mcc_deque_push_front(struct mcc_deque *self, const void *value);

int mcc_deque_push_back(struct mcc_deque *self, const void *value);

void mcc_deque_pop_front(struct mcc_deque *self);

void mcc_deque_pop_back(struct mcc_deque *self);

int mcc_deque_insert(struct mcc_deque *self, size_t index, const void *value);

void mcc_deque_remove(struct mcc_deque *self, size_t index);

void mcc_deque_clear(struct mcc_deque *self);

int mcc_deque_get(struct mcc_deque *self, size_t index, void *value);

void *mcc_deque_get_ptr(struct mcc_deque *self, size_t index);

int mcc_deque_front(struct mcc_deque *self, void *value);

void *mcc_deque_front_ptr(struct mcc_deque *self);

int mcc_deque_back(struct mcc_deque *self, void *value);

void *mcc_deque_back_ptr(struct mcc_deque *self);

size_t mcc_deque_capacity(struct mcc_deque *self);

size_t mcc_deque_len(struct mcc_deque *self);

bool mcc_deque_is_empty(struct mcc_deque *self);

int mcc_deque_swap(struct mcc_deque *self, size_t a, size_t b);

int mcc_deque_reverse(struct mcc_deque *self);

int mcc_deque_sort(struct mcc_deque *self);

void *mcc_deque_binary_search(struct mcc_deque *self, const void *key);

int mcc_deque_iter_init(struct mcc_deque *self, struct mcc_deque_iter *iter);

bool mcc_deque_iter_next(struct mcc_deque_iter *self, void *result);

#endif /* _MCC_DEQUE_H */
