#ifndef _MCC_MAP_H
#define _MCC_MAP_H

#include "mcc_object.h"
#include "mcc_utils.h"

struct mcc_map;

struct mcc_map *mcc_map_new(const struct mcc_object_interface *K,
			    const struct mcc_object_interface *V);

void mcc_map_drop(struct mcc_map *self);

int mcc_map_insert(struct mcc_map *self, const void *key, const void *value);

void mcc_map_remove(struct mcc_map *self, const void *key);

void mcc_map_clear(struct mcc_map *self);

int mcc_map_get(struct mcc_map *self, const void *key, void **ref);

int mcc_map_get_key_value(struct mcc_map *self, const void *key,
			  struct mcc_pair **ref);

size_t mcc_map_len(struct mcc_map *self);

bool mcc_map_is_empty(struct mcc_map *self);

struct mcc_map_iter;

struct mcc_map_iter *mcc_map_iter_new(struct mcc_map *map);

void mcc_map_iter_drop(struct mcc_map_iter *self);

bool mcc_map_iter_next(struct mcc_map_iter *self, struct mcc_pair **ref);

#endif /* _MCC_MAP_H */
