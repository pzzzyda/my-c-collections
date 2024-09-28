#include "mcc_hash_map.h"
#include <stdlib.h>

struct arg_wrapper {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;
	const void *key;
	const void *value;
};

struct mcc_hash_entry {
	struct mcc_hash_entry *next;
	void *key;
};

struct mcc_hash_map {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;

	struct mcc_hash_entry **buckets;
	mcc_usize cap;
	mcc_usize len;
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

static inline void *value_of(struct mcc_hash_entry *entry)
{
	return (mcc_u8 *)entry + sizeof(struct mcc_hash_entry);
}

static struct mcc_hash_entry *entry_new(const struct arg_wrapper *args)
{
	struct mcc_hash_entry *self;
	mcc_u8 *tmp;

	mcc_usize total_size = sizeof(struct mcc_hash_entry);
	total_size += args->V->size;

	self = calloc(1, total_size);
	if (!self)
		return NULL;

	tmp = calloc(1, args->K->size);
	if (!tmp) {
		free(self);
		return NULL;
	}
	memcpy(tmp, args->key, args->K->size);
	self->key = tmp;

	if (args->V->size) {
		tmp = value_of(self);
		memcpy(tmp, args->value, args->V->size);
	}

	return self;
}

static void entry_delete(struct mcc_hash_entry *self,
			 const struct mcc_object_interface *K,
			 const struct mcc_object_interface *V)
{
	void *value = value_of(self);

	if (K->dtor)
		K->dtor(self->key);
	free(self->key);

	if (V->dtor)
		V->dtor(value);

	free(self);
}

static inline void entry_update_value(struct mcc_hash_entry *self,
				      const struct mcc_object_interface *V,
				      const void *value)
{
	void *old_value;

	if (!V->size)
		return;

	old_value = value_of(self);
	if (V->dtor)
		V->dtor(old_value);
	memcpy(old_value, value, V->size);
}

static inline mcc_err entry_add(struct mcc_hash_entry **self,
				const struct arg_wrapper *args)
{
	while (*self) {
		if (args->K->cmp(args->key, (*self)->key) == 0) {
			entry_update_value(*self, args->V, args->value);
			return OK;
		}
		self = &(*self)->next;
	}

	*self = entry_new(args);
	if (!*self)
		return CANNOT_ALLOCATE_MEMORY;
	return OK;
}

static mcc_err rehash(struct mcc_hash_map *self, mcc_usize new_capacity)
{
	struct mcc_hash_entry **new_buckets, *curr, *tmp;
	mcc_usize i, index;

	new_buckets = calloc(new_capacity, sizeof(struct mcc_hash_entry *));
	if (!new_buckets)
		return CANNOT_ALLOCATE_MEMORY;

	for (i = 0; i < self->cap; i++) {
		curr = self->buckets[i];

		while (curr) {
			tmp = curr->next;
			index = self->K->hash(curr->key) & (new_capacity - 1);
			curr->next = new_buckets[index];
			new_buckets[index] = curr;
			curr = tmp;
		}
	}

	free(self->buckets);
	self->buckets = new_buckets;
	self->cap = new_capacity;
	return OK;
}

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *K,
				      const struct mcc_object_interface *V)
{
	struct mcc_hash_map *self;

	if (!K || !V)
		return NULL;

	self = calloc(1, sizeof(struct mcc_hash_map));
	if (!self)
		return NULL;

	self->buckets = calloc(8, sizeof(struct mcc_hash_entry *));
	if (!self->buckets) {
		free(self);
		return NULL;
	}

	self->cap = 8;
	self->K = K;
	self->V = V;
	return self;
}

void mcc_hash_map_delete(struct mcc_hash_map *self)
{
	if (!self)
		return;

	mcc_hash_map_clear(self);
	free(self->buckets);
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

	return rehash(self, new_capacity);
}

mcc_err mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			    const void *value)
{
	mcc_usize index;
	const struct arg_wrapper args = {
		.K = self->K,
		.V = self->V,
		.key = key,
		.value = value,
	};

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	index = self->K->hash(key) & (self->cap - 1);
	if (entry_add(&self->buckets[index], &args) != OK)
		return CANNOT_ALLOCATE_MEMORY;

	self->len++;

	if (self->len >= self->cap * 3 / 4)
		mcc_hash_map_reserve(self, self->len);

	return OK;
}

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key)
{
	mcc_usize index;
	struct mcc_hash_entry **curr, *tmp;

	if (!self || !key)
		return;

	index = self->K->hash(key) & (self->cap - 1);
	curr = &self->buckets[index];
	while (*curr) {
		if (self->K->cmp(key, (*curr)->key) == 0) {
			tmp = *curr;
			*curr = (*curr)->next;
			entry_delete(tmp, self->K, self->V);
			self->len--;
			break;
		}
		curr = &(*curr)->next;
	}
}

void mcc_hash_map_clear(struct mcc_hash_map *self)
{
	struct mcc_hash_entry *curr, *tmp;

	if (!self || !self->len)
		return;

	for (mcc_usize i = 0; i < self->cap; i++) {
		curr = self->buckets[i];
		while (curr) {
			tmp = curr->next;
			entry_delete(curr, self->K, self->V);
			curr = tmp;
		}
		self->buckets[i] = NULL;
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

	memcpy(value, ptr, self->V->size);
	return OK;
}

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key)
{
	mcc_usize index;
	struct mcc_hash_entry *curr;

	if (!self || !key)
		return NULL;

	index = self->K->hash(key) & (self->cap - 1);
	curr = self->buckets[index];
	while (curr) {
		if (self->K->cmp(key, curr->key) == 0)
			return value_of(curr);
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

static inline struct mcc_hash_entry *next_valid_entry(struct mcc_hash_map *map,
						      mcc_usize *index)
{
	while (*index < map->cap && !map->buckets[*index])
		(*index)++;

	if (*index < map->cap)
		return map->buckets[(*index)++];
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
	iter->current = next_valid_entry(self, &iter->index);
	iter->container = self;
	return OK;
}

mcc_bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *iter,
				struct mcc_kv_pair *result)
{
	if (!iter || (iter->index >= iter->container->cap && !iter->current))
		return false;

	if (result) {
		result->key = iter->current->key;
		result->value = value_of(iter->current);
	}

	if (iter->current->next)
		iter->current = iter->current->next;
	else
		iter->current = next_valid_entry(iter->container, &iter->index);
	return true;
}
