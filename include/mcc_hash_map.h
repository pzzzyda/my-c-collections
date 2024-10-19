#ifndef _MCC_HASH_MAP_H
#define _MCC_HASH_MAP_H

#include "mcc_object.h"
#include "mcc_utils.h"

struct mcc_hash_map;

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *K,
				      const struct mcc_object_interface *V);

void mcc_hash_map_drop(struct mcc_hash_map *self);

int mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional);

int mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			const void *value);

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key);

void mcc_hash_map_clear(struct mcc_hash_map *self);

int mcc_hash_map_get(struct mcc_hash_map *self, const void *key, void **ref);

int mcc_hash_map_get_key_value(struct mcc_hash_map *self, const void *key,
			       struct mcc_pair **ref);

size_t mcc_hash_map_capacity(struct mcc_hash_map *self);

size_t mcc_hash_map_len(struct mcc_hash_map *self);

bool mcc_hash_map_is_empty(struct mcc_hash_map *self);

struct mcc_hash_map_iter;

struct mcc_hash_map_iter *mcc_hash_map_iter_new(struct mcc_hash_map *map);

void mcc_hash_map_iter_drop(struct mcc_hash_map_iter *self);

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *self,
			    struct mcc_pair **ref);

#endif /* _MCC_HASH_MAP_H */
