#ifndef _MCC_HASH_MAP_H
#define _MCC_HASH_MAP_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_bucket;

struct mcc_hash_map;

struct mcc_hash_map_iter {
	struct mcc_iterator_interface interface;
	mcc_usize index;
	struct mcc_bucket *current;
	struct mcc_hash_map *container;
};

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *key,
				      const struct mcc_object_interface *value);

void mcc_hash_map_delete(struct mcc_hash_map *self);

mcc_err mcc_hash_map_reserve(struct mcc_hash_map *self, mcc_usize additional);

mcc_err mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			    const void *value);

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key);

void mcc_hash_map_clear(struct mcc_hash_map *self);

mcc_err mcc_hash_map_get(struct mcc_hash_map *self, const void *key,
			 void *value);

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key);

mcc_usize mcc_hash_map_capacity(struct mcc_hash_map *self);

mcc_usize mcc_hash_map_len(struct mcc_hash_map *self);

mcc_bool mcc_hash_map_is_empty(struct mcc_hash_map *self);

mcc_err mcc_hash_map_iter_init(struct mcc_hash_map *self,
			       struct mcc_hash_map_iter *iter);

mcc_bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *iter,
				struct mcc_kv_pair *result);

#endif /* _MCC_HASH_MAP_H */
