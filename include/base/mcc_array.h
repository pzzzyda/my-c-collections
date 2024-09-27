#ifndef _MCC_ARR_H
#define _MCC_ARR_H

#include "core/mcc_object.h"
#include "core/mcc_types.h"

struct mcc_array {
	const struct mcc_object_interface *T;
	mcc_usize cap;
};

struct mcc_array *mcc_array_new(const struct mcc_object_interface *T,
				mcc_usize capacity);

void mcc_array_delete(struct mcc_array *self);

struct mcc_array *mcc_array_reserve(struct mcc_array *self, mcc_usize extras);

struct mcc_array *mcc_array_shrink(struct mcc_array *self, mcc_usize capacity);

void *mcc_array_at(struct mcc_array *self, mcc_usize index);

void mcc_array_set(struct mcc_array *self, mcc_usize index, const void *value);

void mcc_array_get(struct mcc_array *self, mcc_usize index, void *value);

void *mcc_array_ptr(struct mcc_array *self);

void mcc_array_move(struct mcc_array *self, mcc_usize to, mcc_usize from,
		    mcc_usize n);

void mcc_array_swap(struct mcc_array *self, mcc_usize a, mcc_usize b);

mcc_i32 mcc_array_cmp(struct mcc_array *self, mcc_usize a, mcc_usize b);

#endif /* _MCC_ARR_H */
