#ifndef _MCC_HASH_SET_H
#define _MCC_HASH_SET_H

#include "mcc_object.h"

struct mcc_hash_set;

struct mcc_hash_set *mcc_hash_set_new(const struct mcc_object_interface *T);

void mcc_hash_set_drop(struct mcc_hash_set *self);

int mcc_hash_set_reserve(struct mcc_hash_set *self, size_t additional);

int mcc_hash_set_insert(struct mcc_hash_set *self, const void *value);

void mcc_hash_set_remove(struct mcc_hash_set *self, const void *value);

void mcc_hash_set_clear(struct mcc_hash_set *self);

int mcc_hash_set_get(struct mcc_hash_set *self, const void *value,
		     const void **ref);

size_t mcc_hash_set_capacity(struct mcc_hash_set *self);

size_t mcc_hash_set_len(struct mcc_hash_set *self);

bool mcc_hash_set_is_empty(struct mcc_hash_set *self);

struct mcc_hash_set_iter;

struct mcc_hash_set_iter *mcc_hash_set_iter_new(struct mcc_hash_set *set);

void mcc_hash_set_iter_drop(struct mcc_hash_set_iter *self);

bool mcc_hash_set_iter_next(struct mcc_hash_set_iter *self, const void **ref);

#endif /* _MCC_HASH_SET_H */
