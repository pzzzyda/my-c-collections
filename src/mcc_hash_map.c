#include "mcc_hash_map.h"
#include <stdlib.h>

struct mcc_hash_entry {
	bool small_key;
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

static inline void *key_of(struct mcc_hash_entry *entry)
{
	return entry->small_key ? &entry->key : entry->key;
}

static inline void *val_of(struct mcc_hash_entry *entry)
{
	return (uint8_t *)entry + sizeof(struct mcc_hash_entry);
}

static struct mcc_hash_entry *create_entry(const void *key, size_t key_size,
					   const void *val, size_t val_size)
{
	struct mcc_hash_entry *entry;

	entry = calloc(1, sizeof(struct mcc_hash_entry) + val_size);
	if (!entry)
		return NULL;

	if (key_size <= sizeof(void *)) {
		entry->small_key = true;
	} else {
		entry->small_key = false;
		entry->key = calloc(1, key_size);
		if (!entry->key) {
			free(entry);
			return NULL;
		}
	}

	memcpy(key_of(entry), key, key_size);
	memcpy(val_of(entry), val, val_size);

	return entry;
}

static void destroy_entry(struct mcc_hash_entry *entry,
			  const mcc_destruct_fn key_dtor,
			  const mcc_destruct_fn val_dtor)
{
	if (key_dtor)
		key_dtor(key_of(entry));

	if (val_dtor)
		val_dtor(val_of(entry));

	if (!entry->small_key)
		free(entry->key);

	free(entry);
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
			index = self->K->hash(key_of(curr)) &
				(new_capacity - 1);
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

static struct mcc_hash_entry **find(struct mcc_hash_map *self, const void *key)
{
	size_t index = self->K->hash(key) & (self->cap - 1);
	struct mcc_hash_entry **entry = &self->buckets[index];

	while (*entry) {
		if (self->K->cmp(key, key_of(*entry)) == 0)
			return entry;
		entry = &(*entry)->next;
	}

	return entry;
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
	struct mcc_hash_entry **entry;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	entry = find(self, key);

	if (*entry != NULL) {
		if (self->V->dtor)
			self->V->dtor(val_of(*entry));
		memcpy(val_of(*entry), value, self->V->size);
		return OK;
	} else {
		*entry = create_entry(key, self->K->size, value, self->V->size);
		if (!*entry)
			return INVALID_ARGUMENTS;
		self->len++;
		if (self->len >= self->cap * 3 / 4)
			mcc_hash_map_reserve(self, self->len);
		return OK;
	}
}

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key)
{
	struct mcc_hash_entry **entry, *tmp;

	if (!self || !key)
		return;

	entry = find(self, key);
	if (*entry != NULL) {
		tmp = *entry;
		*entry = (*entry)->next;
		destroy_entry(tmp, self->K->dtor, self->V->dtor);
		self->len--;
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
			destroy_entry(curr, self->K->dtor, self->V->dtor);
			curr = tmp;
		}
		self->buckets[i] = NULL;
	}
	self->len = 0;
}

mcc_err_t mcc_hash_map_get(struct mcc_hash_map *self, const void *key,
			   void *value)
{
	struct mcc_hash_entry **entry;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	entry = find(self, key);

	if (*entry != NULL) {
		memcpy(value, val_of(*entry), self->V->size);
		return OK;
	} else {
		return NONE;
	}
}

void *mcc_hash_map_get_ptr(struct mcc_hash_map *self, const void *key)
{
	struct mcc_hash_entry **entry;

	if (!self || !key)
		return NULL;

	entry = find(self, key);

	return *entry != NULL ? val_of(*entry) : NULL;
}

mcc_err_t mcc_hash_map_get_key_value(struct mcc_hash_map *self, const void *key,
				     struct mcc_kv_pair *pair)
{
	struct mcc_hash_entry **entry;

	if (!self || !key || !pair)
		return INVALID_ARGUMENTS;

	entry = find(self, key);

	if (*entry != NULL) {
		memcpy(pair->key, key_of(*entry), self->K->size);
		memcpy(pair->value, val_of(*entry), self->V->size);
		return OK;
	} else {
		return NONE;
	}
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

static const struct mcc_iterator_interface mcc_hash_map_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_hash_map_iter_next,
};

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

	memcpy(result->key, key_of(self->curr), self->container->K->size);
	memcpy(result->value, val_of(self->curr), self->container->V->size);

	if (self->curr->next)
		self->curr = self->curr->next;
	else
		self->curr = next_valid_entry(self->container, &self->idx);
	return true;
}
