#ifndef _MCC_HASH_MAP_H
#define _MCC_HASH_MAP_H

#include "mcc_iterator.h"
#include "mcc_object.h"

struct mcc_bucket;

struct mcc_hash_map;

struct mcc_hash_map_iter {
	struct mcc_iterator_interface interface;
	size_t idx;
	struct mcc_bucket *curr;
	struct mcc_hash_map *container;
};

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *key,
				      const struct mcc_object_interface *value);

void mcc_hash_map_delete(struct mcc_hash_map *self);

int mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional);

int mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			const void *value);

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key);

void mcc_hash_map_clear(struct mcc_hash_map *self);

int mcc_hash_map_get(struct mcc_hash_map *self, const void *key, void *value);

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key);

size_t mcc_hash_map_capacity(struct mcc_hash_map *self);

size_t mcc_hash_map_len(struct mcc_hash_map *self);

bool mcc_hash_map_is_empty(struct mcc_hash_map *self);

int mcc_hash_map_iter_init(struct mcc_hash_map *self,
			   struct mcc_hash_map_iter *iter);

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *iter, void *result);

#endif /* _MCC_HASH_MAP_H */
