#include "mcc_err.h"
#include "mcc_hash_map.h"
#include <stdlib.h>
#include <string.h>

struct mcc_hash_map_entry {
	struct mcc_hash_map_entry *next;
	struct mcc_pair pair;
};

struct mcc_hash_map_iter {
	struct mcc_hash_map_iter *next;
	struct mcc_hash_map *map;
	struct mcc_hash_map_entry *curr;
	size_t index;
	bool in_use;
};

struct mcc_hash_map {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;
	struct mcc_hash_map_iter *iters;
	struct mcc_hash_map_entry **bkts;
	size_t len;
	size_t cap;
};

static struct mcc_hash_map_entry *create_entry(const void *key, size_t key_size,
					       const void *val, size_t val_size)
{
	struct mcc_hash_map_entry *entry;
	uint8_t *ptr;
	size_t total_size = 0;

	total_size += sizeof(struct mcc_hash_map_entry);
	total_size += key_size;
	total_size += val_size;
	entry = calloc(1, total_size);
	if (!entry)
		return NULL;

	ptr = (uint8_t *)entry + sizeof(struct mcc_hash_map_entry);
	memcpy(ptr, key, key_size);
	entry->pair.key = ptr;

	ptr += key_size;
	memcpy(ptr, val, val_size);
	entry->pair.value = ptr;

	return entry;
}

static void destroy_entry(struct mcc_hash_map_entry *entry,
			  const mcc_drop_fn key_drop,
			  const mcc_drop_fn val_drop)
{
	if (key_drop)
		key_drop((uint8_t *)entry + sizeof(struct mcc_hash_map_entry));

	if (val_drop)
		val_drop(entry->pair.value);

	free(entry);
}

static struct mcc_hash_map_entry **get_entry(struct mcc_hash_map *self,
					     const void *key)
{
	size_t h = self->K->hash(key) & (self->cap - 1);
	struct mcc_hash_map_entry **entry = &self->bkts[h];

	while (*entry) {
		if (!self->K->cmp(key, (*entry)->pair.key))
			break;
		entry = &((*entry)->next);
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

	self->bkts = calloc(8, sizeof(struct mcc_hash_map_entry *));
	if (!self->bkts) {
		free(self);
		return NULL;
	}

	self->cap = 8;
	self->K = K;
	self->V = V;
	return self;
}

void mcc_hash_map_drop(struct mcc_hash_map *self)
{
	struct mcc_hash_map_iter *next;

	if (!self)
		return;

	while (self->iters) {
		next = self->iters->next;
		free(self->iters);
		self->iters = next;
	}
	mcc_hash_map_clear(self);
	free(self->bkts);
	free(self);
}

int mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional)
{
	size_t min_capacity, new_capacity, i, h;
	struct mcc_hash_map_entry **new_buckets, *curr, *next;

	if (!self)
		return INVALID_ARGUMENTS;

	min_capacity = self->len + additional;
	if (self->cap >= min_capacity)
		return OK;

	new_capacity = self->cap << 1;
	while (new_capacity < min_capacity)
		new_capacity <<= 1;

	new_buckets = calloc(new_capacity, sizeof(struct mcc_hash_map_entry *));
	if (!new_buckets)
		return CANNOT_ALLOCATE_MEMORY;

	for (i = 0; i < self->cap; i++) {
		for (curr = self->bkts[i]; curr; curr = next) {
			next = curr->next;
			h = self->K->hash(curr->pair.key) & (new_capacity - 1);
			curr->next = new_buckets[h];
			new_buckets[h] = curr;
		}
	}

	free(self->bkts);
	self->bkts = new_buckets;
	self->cap = new_capacity;
	return OK;
}

int mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			const void *value)
{
	struct mcc_hash_map_entry **entry;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	entry = get_entry(self, key);
	if (*entry) { /* Just update the value. */
		/* When used as mcc_hash_set, the size of V is 0. */
		if (!self->V->size)
			return OK;

		if (self->V->drop)
			self->V->drop((*entry)->pair.value);

		memcpy((*entry)->pair.value, value, self->V->size);
		return OK;
	} else { /* Try creating a new entry. */
		*entry = create_entry(key, self->K->size, value, self->V->size);
		if (!(*entry))
			return CANNOT_ALLOCATE_MEMORY;

		self->len++;

		/* The load factor is 0.75. */
		if (self->len >= (self->cap * 3) >> 2)
			mcc_hash_map_reserve(self, self->len);

		return OK;
	}
}

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key)
{
	struct mcc_hash_map_entry **curr, *tmp;

	if (!self || !key)
		return;

	curr = get_entry(self, key);
	if (*curr) {
		tmp = *curr;
		*curr = (*curr)->next;
		destroy_entry(tmp, self->K->drop, self->V->drop);
	}
}

void mcc_hash_map_clear(struct mcc_hash_map *self)
{
	struct mcc_hash_map_entry *curr, *next;

	if (!self || !self->len)
		return;

	for (size_t i = 0; i < self->cap; i++) {
		curr = self->bkts[i];
		while (curr) {
			next = curr->next;
			destroy_entry(curr, self->K->drop, self->V->drop);
			curr = next;
		}
		self->bkts[i] = NULL;
	}
	self->len = 0;
}

int mcc_hash_map_get(struct mcc_hash_map *self, const void *key, void **ref)
{
	struct mcc_hash_map_entry **curr;

	if (!self || !key || !ref)
		return INVALID_ARGUMENTS;

	curr = get_entry(self, key);
	if (*curr) {
		*ref = (*curr)->pair.value;
		return OK;
	} else {
		return NONE;
	}
}

int mcc_hash_map_get_key_value(struct mcc_hash_map *self, const void *key,
			       struct mcc_pair **ref)
{
	struct mcc_hash_map_entry **curr;

	if (!self || !key || !ref)
		return INVALID_ARGUMENTS;

	curr = get_entry(self, key);
	if (*curr) {
		*ref = &(*curr)->pair;
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

static void find_next_valid_entry(struct mcc_hash_map_iter *self)
{
	if (self->curr && self->curr->next) {
		self->curr = self->curr->next;
		return;
	}

	while (self->index < self->map->cap && !self->map->bkts[self->index])
		self->index++;

	if (self->index < self->map->cap)
		self->curr = self->map->bkts[self->index++];
	else
		self->curr = NULL;
}

struct mcc_hash_map_iter *mcc_hash_map_iter_new(struct mcc_hash_map *map)
{
	struct mcc_hash_map_iter *self;

	if (!map)
		return NULL;

	self = map->iters;
	while (self) {
		if (!self->in_use)
			goto reset_iterator;
		self = self->next;
	}

	self = calloc(1, sizeof(struct mcc_hash_map_iter));
	if (!self)
		return NULL;

	self->next = map->iters;
	map->iters = self;
	self->map = map;
reset_iterator:
	self->curr = NULL;
	self->index = 0;
	self->in_use = true;
	find_next_valid_entry(self);
	return self;
}

void mcc_hash_map_iter_drop(struct mcc_hash_map_iter *self)
{
	if (self)
		self->in_use = false;
}

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *self,
			    struct mcc_pair **ref)
{
	if (!self || !ref || !self->curr)
		return false;

	*ref = &self->curr->pair;
	find_next_valid_entry(self);
	return true;
}
