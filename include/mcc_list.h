#ifndef _MCC_LIST_H
#define _MCC_LIST_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"

struct mcc_list_node;

struct mcc_list;

struct mcc_list_iter {
	struct mcc_iterator_interface interface;
	struct mcc_list_node *current;
	struct mcc_list *container;
};

struct mcc_list *mcc_list_new(const struct mcc_object_interface *element);

void mcc_list_delete(struct mcc_list *self);

mcc_err mcc_list_push_front(struct mcc_list *self, const void *value);

mcc_err mcc_list_push_back(struct mcc_list *self, const void *value);

void mcc_list_pop_front(struct mcc_list *self);

void mcc_list_pop_back(struct mcc_list *self);

mcc_err mcc_list_insert(struct mcc_list *self, mcc_usize index,
			const void *value);

void mcc_list_remove(struct mcc_list *self, mcc_usize index);

void mcc_list_clear(struct mcc_list *self);

mcc_err mcc_list_front(struct mcc_list *self, void *value);

void *mcc_list_front_ptr(struct mcc_list *self);

mcc_err mcc_list_back(struct mcc_list *self, void *value);

void *mcc_list_back_ptr(struct mcc_list *self);

mcc_usize mcc_list_len(struct mcc_list *self);

mcc_bool mcc_list_is_empty(struct mcc_list *self);

mcc_err mcc_list_sort(struct mcc_list *self);

mcc_err mcc_list_iter_init(struct mcc_list *self, struct mcc_list_iter *iter);

mcc_bool mcc_list_iter_next(struct mcc_list_iter *iter, void *result);

#endif /* _MCC_LIST_H */
