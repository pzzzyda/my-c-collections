#ifndef _MCC_SET_H
#define _MCC_SET_H

#include "mcc_object.h"

struct mcc_set;

struct mcc_set *mcc_set_new(const struct mcc_object_interface *T);

void mcc_set_drop(struct mcc_set *self);

int mcc_set_insert(struct mcc_set *self, const void *value);

void mcc_set_remove(struct mcc_set *self, const void *value);

void mcc_set_clear(struct mcc_set *self);

int mcc_set_get(struct mcc_set *self, const void *value, const void **ref);

size_t mcc_set_len(struct mcc_set *self);

bool mcc_set_is_empty(struct mcc_set *self);

struct mcc_set_iter;

struct mcc_set_iter *mcc_set_iter_new(struct mcc_set *set);

void mcc_set_iter_drop(struct mcc_set_iter *self);

bool mcc_set_iter_next(struct mcc_set_iter *self, const void **ref);

#endif /* _MCC_SET_H */
