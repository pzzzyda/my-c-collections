#include "mcc_hash_map.h"
#include <stdlib.h>

struct mcc_bucket {
	struct mcc_bucket *next;
	struct mcc_kv_pair pair;
};

struct mcc_hash_map {
	struct mcc_bucket **bkts;
	mcc_usize len;
	mcc_usize cap;
	struct mcc_object_interface k;
	struct mcc_object_interface v;
};

static void mcc_hash_map_dtor(void *self)
{
	mcc_hash_map_delete(*(struct mcc_hash_map **)self);
}

static mcc_i32 mcc_hash_map_cmp(const void *self, const void *other)
{
	struct mcc_hash_map *const *p1 = self;
	struct mcc_hash_map *const *p2 = other;

	return mcc_usize_i.cmp(&(**p1).len, &(**p2).len);
}

static mcc_usize mcc_hash_map_hash(const void *self)
{
	struct mcc_hash_map *const *p = self;

	return mcc_usize_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_hash_map_i = {
	.size = sizeof(struct mcc_hash_map *),
	.dtor = &mcc_hash_map_dtor,
	.cmp = &mcc_hash_map_cmp,
	.hash = &mcc_hash_map_hash,
};

static struct mcc_bucket *bucket_new(const void *k, const void *v,
				     const struct mcc_hash_map *map)
{
	struct mcc_bucket *self;
	const struct mcc_object_interface *ki = &map->k;
	const struct mcc_object_interface *vi = &map->v;

	self = calloc(1, sizeof(struct mcc_bucket));
	if (!self)
		return NULL;

	self->pair.key = calloc(1, ki->size);
	if (!self->pair.key)
		goto handle_key_allocate_err;
	memcpy(self->pair.key, k, ki->size);

	self->pair.value = calloc(1, vi->size);
	if (!self->pair.value)
		goto handle_value_allocate_err;
	memcpy(self->pair.value, v, vi->size);

	return self;

handle_value_allocate_err:
	if (ki->dtor)
		ki->dtor(self->pair.key);
	free(self->pair.key);
handle_key_allocate_err:
	free(self);
	return NULL;
}

static void bucket_delete(struct mcc_bucket *self,
			  const struct mcc_object_interface *ki,
			  const struct mcc_object_interface *vi)
{
	struct mcc_bucket *tmp;

	while (self) {
		tmp = self->next;

		if (ki->dtor)
			ki->dtor(self->pair.key);
		free(self->pair.key);

		if (vi->dtor)
			vi->dtor(self->pair.value);
		free(self->pair.value);

		free(self);
		self = tmp;
	}
}

static inline mcc_usize hash_bkt(struct mcc_bucket *bkt,
				 struct mcc_hash_map *map)
{
	return map->k.hash(bkt->pair.key);
}

static inline mcc_i32 cmp_key(const void *k1, const void *k2,
			      struct mcc_hash_map *map)
{
	return map->k.cmp(k1, k2);
}

static mcc_err mcc_hash_map_rehash(struct mcc_hash_map *self,
				   mcc_usize new_capacity)
{
	struct mcc_bucket **new_bkts, *curr, *tmp;
	mcc_usize index;

	new_bkts = calloc(new_capacity, sizeof(struct mcc_bucket *));
	if (!new_bkts)
		return CANNOT_ALLOCATE_MEMORY;

	for (mcc_usize i = 0; i < self->cap; i++) {
		curr = self->bkts[i];

		while (curr) {
			tmp = curr->next;
			index = hash_bkt(curr, self) & (new_capacity - 1);
			curr->next = new_bkts[index];
			new_bkts[index] = curr;
			curr = tmp;
		}
	}

	free(self->bkts);
	self->bkts = new_bkts;
	self->cap = new_capacity;
	return OK;
}

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *key,
				      const struct mcc_object_interface *value)
{
	struct mcc_hash_map *self;

	if (!key || !value)
		return NULL;

	self = calloc(1, sizeof(struct mcc_hash_map));
	if (!self)
		return NULL;

	self->bkts = calloc(4, sizeof(struct mcc_bucket *));
	if (!self->bkts) {
		free(self);
		return NULL;
	}

	self->cap = 4;
	memcpy(&self->k, key, sizeof(self->k));
	memcpy(&self->v, value, sizeof(self->v));
	return self;
}

void mcc_hash_map_delete(struct mcc_hash_map *self)
{
	if (!self)
		return;

	mcc_hash_map_clear(self);
	free(self->bkts);
	free(self);
}

mcc_err mcc_hash_map_reserve(struct mcc_hash_map *self, mcc_usize additional)
{
	mcc_usize new_capacity, needs;

	if (!self)
		return INVALID_ARGUMENTS;

	needs = self->len + additional;
	if (needs <= self->cap)
		return OK;

	new_capacity = self->cap << 1;
	while (new_capacity < needs)
		new_capacity <<= 1;

	return mcc_hash_map_rehash(self, new_capacity);
}

mcc_err mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			    const void *value)
{
	mcc_usize index;
	struct mcc_bucket **curr;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	index = self->k.hash(key) & (self->cap - 1);
	curr = &self->bkts[index];
	while (*curr) {
		if (cmp_key(key, (*curr)->pair.key, self) == 0) {
			if (self->v.dtor)
				self->v.dtor((*curr)->pair.value);
			memcpy((*curr)->pair.value, value, self->v.size);
			return OK;
		}
		curr = &(*curr)->next;
	}

	*curr = bucket_new(key, value, self);
	if (!*curr)
		return CANNOT_ALLOCATE_MEMORY;

	self->len++;

	if (self->len >= self->cap * 3 / 4)
		mcc_hash_map_reserve(self, self->len);

	return OK;
}

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key)
{
	mcc_usize index;
	struct mcc_bucket **curr, *tmp;

	if (!self || !key)
		return;

	index = self->k.hash(key) & (self->cap - 1);
	curr = &self->bkts[index];
	while (*curr) {
		if (cmp_key(key, (*curr)->pair.key, self) == 0) {
			tmp = *curr;
			*curr = (*curr)->next;
			tmp->next = NULL;
			bucket_delete(tmp, &self->k, &self->v);
			break;
		}
		curr = &(*curr)->next;
	}
}

void mcc_hash_map_clear(struct mcc_hash_map *self)
{
	if (!self)
		return;

	for (mcc_usize i = 0; i < self->cap; i++) {
		bucket_delete(self->bkts[i], &self->k, &self->v);
		self->bkts[i] = NULL;
	}
	self->len = 0;
}

mcc_err mcc_hash_map_get(struct mcc_hash_map *self, const void *key,
			 void *value)
{
	void *ptr;

	if (!self || !key)
		return INVALID_ARGUMENTS;

	ptr = mcc_hash_map_get_ptr(self, key);
	if (!ptr)
		return NONE;

	memcpy(value, ptr, self->v.size);
	return OK;
}

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key)
{
	mcc_usize index;
	struct mcc_bucket *curr;

	if (!self || !key)
		return NULL;

	index = self->k.hash(key) & (self->cap - 1);
	curr = self->bkts[index];
	while (curr) {
		if (cmp_key(key, curr->pair.key, self) == 0)
			return curr->pair.value;
		curr = curr->next;
	}
	return NULL;
}

mcc_usize mcc_hash_map_capacity(struct mcc_hash_map *self)
{
	return !self ? 0 : self->cap;
}

mcc_usize mcc_hash_map_len(struct mcc_hash_map *self)
{
	return !self ? 0 : self->len;
}

mcc_bool mcc_hash_map_is_empty(struct mcc_hash_map *self)
{
	return !self ? true : self->len == 0;
}

static inline struct mcc_bucket *next_valid_bkt(struct mcc_hash_map *map,
						mcc_usize *index)
{
	while (*index < map->cap && !map->bkts[*index])
		(*index)++;

	if (*index < map->cap)
		return map->bkts[(*index)++];
	else
		return NULL;
}

mcc_err mcc_hash_map_iter_init(struct mcc_hash_map *self,
			       struct mcc_hash_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_hash_map_iter_next;
	iter->index = 0;
	iter->current = next_valid_bkt(self, &iter->index);
	iter->container = self;
	return OK;
}

mcc_bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *iter,
				struct mcc_kv_pair *result)
{
	if (!iter || (iter->index >= iter->container->cap && !iter->current))
		return false;

	if (result)
		memcpy(result, &iter->current->pair,
		       sizeof(struct mcc_kv_pair));

	if (iter->current->next)
		iter->current = iter->current->next;
	else
		iter->current = next_valid_bkt(iter->container, &iter->index);
	return true;
}
