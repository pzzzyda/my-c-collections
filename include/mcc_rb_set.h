#ifndef _MCC_RB_SET_H
#define _MCC_RB_SET_H

#include "mcc_rb_map.h"

struct mcc_rb_set;

struct mcc_rb_set_iter {
	struct mcc_rb_map_iter base;
	struct mcc_rb_set *container;
};

struct mcc_rb_set *mcc_rb_set_new(const struct mcc_object_interface *T);

void mcc_rb_set_delete(struct mcc_rb_set *self);

mcc_err_t mcc_rb_set_insert(struct mcc_rb_set *self, const void *value);

void mcc_rb_set_remove(struct mcc_rb_set *self, const void *value);

void mcc_rb_set_clear(struct mcc_rb_set *self);

mcc_err_t mcc_rb_set_get(struct mcc_rb_set *self, const void *value,
			 void *result);

size_t mcc_rb_set_len(struct mcc_rb_set *self);

bool mcc_rb_set_is_empty(struct mcc_rb_set *self);

mcc_err_t mcc_rb_set_iter_init(struct mcc_rb_set *self,
			       struct mcc_rb_set_iter *iter);

bool mcc_rb_set_iter_next(struct mcc_rb_set_iter *self, void *result);

#endif /* _MCC_RB_SET_H */
