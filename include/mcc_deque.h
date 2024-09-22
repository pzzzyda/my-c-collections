#ifndef _MCC_DEQUE_H
#define _MCC_DEQUE_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"

struct mcc_deque;

struct mcc_deque_iter {
	struct mcc_iterator_interface interface;
	mcc_usize index;
	struct mcc_deque *container;
};

struct mcc_deque *mcc_deque_new(const struct mcc_object_interface *element);

void mcc_deque_delete(struct mcc_deque *self);

mcc_err mcc_deque_reserve(struct mcc_deque *self, mcc_usize additional);

mcc_err mcc_deque_push_front(struct mcc_deque *self, const void *value);

mcc_err mcc_deque_push_back(struct mcc_deque *self, const void *value);

void mcc_deque_pop_front(struct mcc_deque *self);

void mcc_deque_pop_back(struct mcc_deque *self);

mcc_err mcc_deque_insert(struct mcc_deque *self, mcc_usize index,
			 const void *value);

void mcc_deque_remove(struct mcc_deque *self, mcc_usize index);

void mcc_deque_clear(struct mcc_deque *self);

mcc_err mcc_deque_get(struct mcc_deque *self, mcc_usize index, void *value);

void *mcc_deque_get_ptr(struct mcc_deque *self, mcc_usize index);

mcc_err mcc_deque_front(struct mcc_deque *self, void *value);

void *mcc_deque_front_ptr(struct mcc_deque *self);

mcc_err mcc_deque_back(struct mcc_deque *self, void *value);

void *mcc_deque_back_ptr(struct mcc_deque *self);

mcc_usize mcc_deque_capacity(struct mcc_deque *self);

mcc_usize mcc_deque_len(struct mcc_deque *self);

mcc_bool mcc_deque_is_empty(struct mcc_deque *self);

mcc_err mcc_deque_swap(struct mcc_deque *self, mcc_usize a, mcc_usize b);

mcc_err mcc_deque_reverse(struct mcc_deque *self);

mcc_err mcc_deque_sort(struct mcc_deque *self);

mcc_err mcc_deque_iter_init(struct mcc_deque *self,
			    struct mcc_deque_iter *iter);

mcc_bool mcc_deque_iter_next(struct mcc_deque_iter *iter, void *result);

#endif /* _MCC_DEQUE_H */
