#ifndef _MCC_VECTOR_H
#define _MCC_VECTOR_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"

struct mcc_vector;

struct mcc_vector_iter {
	struct mcc_iterator_interface interface;
	mcc_usize index;
	struct mcc_vector *container;
};

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *element);

void mcc_vector_delete(struct mcc_vector *self);

mcc_err mcc_vector_reserve(struct mcc_vector *self, mcc_usize additional);

mcc_err mcc_vector_shrink_to_fit(struct mcc_vector *self);

mcc_err mcc_vector_push(struct mcc_vector *self, const void *value);

void mcc_vector_pop(struct mcc_vector *self);

mcc_err mcc_vector_insert(struct mcc_vector *self, mcc_usize index,
			  const void *value);

void mcc_vector_remove(struct mcc_vector *self, mcc_usize index);

void mcc_vector_clear(struct mcc_vector *self);

mcc_err mcc_vector_get(struct mcc_vector *self, mcc_usize index, void *value);

void *mcc_vector_get_ptr(struct mcc_vector *self, mcc_usize index);

mcc_err mcc_vector_front(struct mcc_vector *self, void *value);

void *mcc_vector_front_ptr(struct mcc_vector *self);

mcc_err mcc_vector_back(struct mcc_vector *self, void *value);

void *mcc_vector_back_ptr(struct mcc_vector *self);

mcc_usize mcc_vector_capacity(struct mcc_vector *self);

mcc_usize mcc_vector_len(struct mcc_vector *self);

mcc_bool mcc_vector_is_empty(struct mcc_vector *self);

mcc_err mcc_vector_swap(struct mcc_vector *self, mcc_usize a, mcc_usize b);

mcc_err mcc_vector_reverse(struct mcc_vector *self);

mcc_err mcc_vector_sort(struct mcc_vector *self);

mcc_err mcc_vector_iter_init(struct mcc_vector *self,
			     struct mcc_vector_iter *iter);

mcc_bool mcc_vector_iter_next(struct mcc_vector_iter *iter, void *result);

#endif /* _MCC_VECTOR_H */
