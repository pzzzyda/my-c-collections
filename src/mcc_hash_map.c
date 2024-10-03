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
	size_t cap;
	size_t len;
};

static void mcc_hash_map_dtor(void *self)
{
	mcc_hash_map_delete(*(struct mcc_hash_map **)self);
}

static int mcc_hash_map_cmp(const void *self, const void *other)
{
	struct mcc_hash_map *const *p1 = self;
	struct mcc_hash_map *const *p2 = other;

	return SIZE_T->cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_hash_map_hash(const void *self)
{
	struct mcc_hash_map *const *p = self;

	return SIZE_T->hash(&(**p).len);
}

const struct mcc_object_interface __mcc_hash_map_obj_intf = {
	.size = sizeof(struct mcc_hash_map *),
	.dtor = &mcc_hash_map_dtor,
	.cmp = &mcc_hash_map_cmp,
	.hash = &mcc_hash_map_hash,
};

static inline void *value_of(struct mcc_hash_entry *entry)
{
	return (uint8_t *)entry + sizeof(struct mcc_hash_entry);
}

static struct mcc_hash_entry *entry_new(const struct arg_wrapper *args)
{
	struct mcc_hash_entry *self;

	size_t total_size = sizeof(struct mcc_hash_entry);
	total_size += args->V->size;

	self = calloc(1, total_size);
	if (!self)
		return NULL;

	self->key = calloc(1, args->K->size);
	if (!self->key) {
		free(self);
		return NULL;
	}
	memcpy(self->key, args->key, args->K->size);

	memcpy(value_of(self), args->value, args->V->size);

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

static inline mcc_err_t entry_add(struct mcc_hash_entry **self,
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

static mcc_err_t rehash(struct mcc_hash_map *self, size_t new_capacity)
{
	struct mcc_hash_entry **new_buckets, *curr, *tmp;
	size_t i, index;

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

mcc_err_t mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional)
{
	size_t new_capacity, needs;

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

mcc_err_t mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			      const void *value)
{
	size_t index;
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
	size_t index;
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

	for (size_t i = 0; i < self->cap; i++) {
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

mcc_err_t mcc_hash_map_get(struct mcc_hash_map *self, const void *key,
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
	size_t index;
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

size_t mcc_hash_map_capacity(struct mcc_hash_map *self)
{
	return !self ? 0 : self->cap;
}

size_t mcc_hash_map_len(struct mcc_hash_map *self)
{
	return !self ? 0 : self->len;
}

bool mcc_hash_map_is_empty(struct mcc_hash_map *self)
{
	return !self ? true : self->len == 0;
}

static inline struct mcc_hash_entry *next_valid_entry(struct mcc_hash_map *map,
						      size_t *index)
{
	while (*index < map->cap && !map->buckets[*index])
		(*index)++;

	if (*index < map->cap)
		return map->buckets[(*index)++];
	else
		return NULL;
}

static const struct mcc_iterator_interface mcc_hash_map_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_hash_map_iter_next,
};

mcc_err_t mcc_hash_map_iter_init(struct mcc_hash_map *self,
				 struct mcc_hash_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->base.iter_intf = &mcc_hash_map_iter_intf;
	iter->idx = 0;
	iter->curr = next_valid_entry(self, &iter->idx);
	iter->container = self;
	return OK;
}

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *self,
			    struct mcc_kv_pair *result)
{
	if (!self || !result)
		return false;

	if (self->idx >= self->container->cap && !self->curr)
		return false;

	memcpy(result->key, self->curr->key, self->container->K->size);
	memcpy(result->value, value_of(self->curr), self->container->V->size);

	if (self->curr->next)
		self->curr = self->curr->next;
	else
		self->curr = next_valid_entry(self->container, &self->idx);
	return true;
}
