#ifndef _MCC_RB_MAP_H
#define _MCC_RB_MAP_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_rb_map;

struct mcc_rb_node;

#define MCC_RB_RED 0
#define MCC_RB_BLACK 1

struct mcc_rb_map_iter {
	struct mcc_iterator_interface interface;
	struct mcc_rb_node *current;
	struct mcc_rb_map *container;
};

struct mcc_rb_map *mcc_rb_map_new(const struct mcc_object_interface *key,
				  const struct mcc_object_interface *value);

void mcc_rb_map_delete(struct mcc_rb_map *self);

mcc_err mcc_rb_map_insert(struct mcc_rb_map *self, const void *key,
			  const void *value);

void mcc_rb_map_remove(struct mcc_rb_map *self, const void *key);

void mcc_rb_map_clear(struct mcc_rb_map *self);

mcc_err mcc_rb_map_get(struct mcc_rb_map *self, const void *key, void *value);

void *mcc_rb_map_get_ptr(struct mcc_rb_map *self, const void *key);

mcc_usize mcc_rb_map_len(struct mcc_rb_map *self);

mcc_bool mcc_rb_map_is_empty(struct mcc_rb_map *self);

mcc_err mcc_rb_map_iter_init(struct mcc_rb_map *self,
			     struct mcc_rb_map_iter *iter);

mcc_bool mcc_rb_map_iter_next(struct mcc_rb_map_iter *iter,
			      struct mcc_kv_pair *result);

#endif /* _MCC_RB_MAP_H */
