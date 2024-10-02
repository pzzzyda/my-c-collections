#ifndef _MCC_ARRAY_H
#define _MCC_ARRAY_H

#include "../core/mcc_object.h"

typedef struct {
	const struct mcc_object_interface *T;
	size_t len;
} mcc_array_t;

mcc_array_t *mcc_array_new(const struct mcc_object_interface *T, size_t len);

void mcc_array_delete(mcc_array_t *self);

mcc_array_t *mcc_array_reserve(mcc_array_t *self, size_t additional);

mcc_array_t *mcc_array_resize(mcc_array_t *self, size_t new_len);

void *mcc_array_at(mcc_array_t *self, size_t index);

void mcc_array_set(mcc_array_t *self, size_t index, const void *value);

void mcc_array_get(mcc_array_t *self, size_t index, void *value);

void *mcc_array_ptr(mcc_array_t *self);

void mcc_array_move(mcc_array_t *self, size_t to, size_t from, size_t n);

void mcc_array_swap(mcc_array_t *self, size_t a, size_t b);

#endif /* _MCC_ARRAY_H */
