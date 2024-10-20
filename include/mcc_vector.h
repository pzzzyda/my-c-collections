#ifndef _MCC_VECTOR_H
#define _MCC_VECTOR_H

#include "mcc_object.h"

struct mcc_vector;

struct mcc_vector *mcc_vector_new(const struct mcc_object_interface *T);

void mcc_vector_drop(struct mcc_vector *self);

int mcc_vector_reserve(struct mcc_vector *self, size_t additional);

int mcc_vector_grow_to(struct mcc_vector *self, size_t capacity);

int mcc_vector_shrink_to(struct mcc_vector *self, size_t capacity);

int mcc_vector_shrink_to_fit(struct mcc_vector *self);

int mcc_vector_push(struct mcc_vector *self, const void *value);

void mcc_vector_pop(struct mcc_vector *self);

int mcc_vector_insert(struct mcc_vector *self, size_t index, const void *value);

void mcc_vector_remove(struct mcc_vector *self, size_t index);

void mcc_vector_clear(struct mcc_vector *self);

int mcc_vector_set(struct mcc_vector *self, size_t index, const void *value);

int mcc_vector_get(struct mcc_vector *self, size_t index, void **ref);

int mcc_vector_front(struct mcc_vector *self, void **ref);

int mcc_vector_back(struct mcc_vector *self, void **ref);

size_t mcc_vector_capacity(struct mcc_vector *self);

size_t mcc_vector_len(struct mcc_vector *self);

bool mcc_vector_is_empty(struct mcc_vector *self);

int mcc_vector_swap(struct mcc_vector *self, size_t a, size_t b);

int mcc_vector_reverse(struct mcc_vector *self);

int mcc_vector_sort(struct mcc_vector *self);

void *mcc_vector_binary_search(struct mcc_vector *self, const void *key);

struct mcc_vector_iter;

struct mcc_vector_iter *mcc_vector_iter_new(struct mcc_vector *vector);

void mcc_vector_iter_drop(struct mcc_vector_iter *self);

bool mcc_vector_iter_next(struct mcc_vector_iter *self, void **ref);

#endif /* _MCC_VECTOR_H */
