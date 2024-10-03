#ifndef _MCC_HASH_MAP_H
#define _MCC_HASH_MAP_H

#include "core/mcc_iterator.h"
#include "core/mcc_object.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_hash_entry;

struct mcc_hash_map;

struct mcc_hash_map_iter {
	struct mcc_iter base;

	size_t idx;
	struct mcc_hash_entry *curr;
	struct mcc_hash_map *container;
};

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *K,
				      const struct mcc_object_interface *V);

void mcc_hash_map_delete(struct mcc_hash_map *self);

mcc_err_t mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional);

mcc_err_t mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			      const void *value);

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key);

void mcc_hash_map_clear(struct mcc_hash_map *self);

mcc_err_t mcc_hash_map_get(struct mcc_hash_map *self, const void *key,
			   void *value);

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key);

mcc_err_t mcc_hash_map_get_key_value(struct mcc_hash_map *self, const void *key,
				     struct mcc_kv_pair *pair);

size_t mcc_hash_map_capacity(struct mcc_hash_map *self);

size_t mcc_hash_map_len(struct mcc_hash_map *self);

bool mcc_hash_map_is_empty(struct mcc_hash_map *self);

mcc_err_t mcc_hash_map_iter_init(struct mcc_hash_map *self,
				 struct mcc_hash_map_iter *iter);

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *self,
			    struct mcc_kv_pair *result);

#endif /* _MCC_HASH_MAP_H */
