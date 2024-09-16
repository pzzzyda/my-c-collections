#ifndef _MCC_MAP_H
#define _MCC_MAP_H

#include "mcc_iterator.h"
#include "mcc_object.h"

struct mcc_map;

struct mcc_rb_node;

enum {
	MCC_RB_RED,
	MCC_RB_BLACK,
};

struct mcc_map_iter {
	struct mcc_iterator_interface interface;
	struct mcc_rb_node *curr;
	struct mcc_map *container;
};

struct mcc_map *mcc_map_new(const struct mcc_object_interface *key,
			    const struct mcc_object_interface *value);

void mcc_map_delete(struct mcc_map *self);

int mcc_map_insert(struct mcc_map *self, const void *key, const void *value);

void mcc_map_remove(struct mcc_map *self, const void *key);

void mcc_map_clear(struct mcc_map *self);

int mcc_map_get(struct mcc_map *self, const void *key, void *value);

void *mcc_map_get_ptr(struct mcc_map *self, const void *key);

size_t mcc_map_len(struct mcc_map *self);

bool mcc_map_is_empty(struct mcc_map *self);

int mcc_map_iter_init(struct mcc_map *self, struct mcc_map_iter *iter);

bool mcc_map_iter_next(struct mcc_map_iter *iter, void *result);

#endif /* _MCC_MAP_H */
