#ifndef _MCC_LIST_H
#define _MCC_LIST_H

#include "mcc_object.h"

struct mcc_list;

struct mcc_list *mcc_list_new(const struct mcc_object_interface *T);

void mcc_list_drop(struct mcc_list *self);

int mcc_list_push_front(struct mcc_list *self, const void *value);

int mcc_list_push_back(struct mcc_list *self, const void *value);

void mcc_list_pop_front(struct mcc_list *self);

void mcc_list_pop_back(struct mcc_list *self);

int mcc_list_insert(struct mcc_list *self, size_t index, const void *value);

void mcc_list_remove(struct mcc_list *self, size_t index);

void mcc_list_clear(struct mcc_list *self);

int mcc_list_front(struct mcc_list *self, void **ref);

int mcc_list_back(struct mcc_list *self, void **ref);

size_t mcc_list_len(struct mcc_list *self);

bool mcc_list_is_empty(struct mcc_list *self);

int mcc_list_sort(struct mcc_list *self);

struct mcc_list_iter;

struct mcc_list_iter *mcc_list_iter_new(struct mcc_list *list);

void mcc_list_iter_drop(struct mcc_list_iter *self);

bool mcc_list_iter_next(struct mcc_list_iter *self, void **ref);

#endif /* _MCC_LIST_H */
