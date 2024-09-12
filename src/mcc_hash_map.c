#include "mcc_hash_map.h"
#include "mcc_err.h"
#include <stdlib.h>
#include <string.h>

struct mcc_bucket {
	struct mcc_bucket *next;
	void *k;
	void *v;
};

struct mcc_hash_map {
	struct mcc_bucket **bkts;
	size_t len;
	size_t cap;
	struct mcc_object_interface k;
	struct mcc_object_interface v;
	mcc_hash_f hash;
	mcc_equal_f eq;
};

static void mcc_hash_map_dtor(void *self)
{
	mcc_hash_map_delete(*(struct mcc_hash_map **)self);
}

const struct mcc_object_interface mcc_hash_map_object_interface = {
	.size = sizeof(struct mcc_hash_map *),
	.dtor = mcc_hash_map_dtor,
};

static struct mcc_bucket *bucket_new(const void *k, const void *v,
				     const struct mcc_object_interface *ki,
				     const struct mcc_object_interface *vi)
{
	struct mcc_bucket *self;

	self = malloc(sizeof(struct mcc_bucket) + ki->size + vi->size);
	if (!self)
		return NULL;

	self->next = NULL;
	self->k = (uint8_t *)self + sizeof(struct mcc_bucket);
	self->v = (uint8_t *)(self->k) + ki->size;

	memcpy(self->k, k, ki->size);
	memcpy(self->v, v, vi->size);

	return self;
}

static void bucket_delete(struct mcc_bucket *self,
			  const struct mcc_object_interface *ki,
			  const struct mcc_object_interface *vi)
{
	struct mcc_bucket *tmp;

	while (self) {
		tmp = self->next;
		if (ki->dtor)
			ki->dtor(self->k);
		if (vi->dtor)
			vi->dtor(self->v);
		free(self);
		self = tmp;
	}
}

struct mcc_hash_map *mcc_hash_map_new(const struct mcc_object_interface *key,
				      const struct mcc_object_interface *value,
				      mcc_hash_f hash, mcc_equal_f equal)
{
	struct mcc_hash_map *self;

	if (!hash || !equal)
		return NULL;

	if (!key || !key->size)
		return NULL;

	if (!value || !value->size)
		return NULL;

	self = malloc(sizeof(struct mcc_hash_map));
	if (!self)
		return NULL;

	self->bkts = calloc(4, sizeof(struct mcc_bucket *));
	if (!self->bkts) {
		free(self);
		return NULL;
	}

	self->len = 0;
	self->cap = 4;
	self->k = *key;
	self->v = *value;
	self->hash = hash;
	self->eq = equal;
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

int mcc_hash_map_reserve(struct mcc_hash_map *self, size_t additional)
{
	struct mcc_bucket **new_bkts, *curr, *tmp;
	size_t index, new_capacity, needs;

	if (!self)
		return INVALID_ARGUMENTS;

	needs = self->len + additional;
	if (needs <= self->cap)
		return OK;

	new_capacity = self->cap << 1;
	while (new_capacity < needs)
		new_capacity <<= 1;

	new_bkts = calloc(new_capacity, sizeof(struct mcc_bucket *));
	if (!new_bkts)
		return CANNOT_ALLOCATE_MEMORY;

	for (size_t i = 0; i < self->cap; i++) {
		curr = self->bkts[i];
		while (curr) {
			tmp = curr->next;
			index = self->hash(curr->k) & (new_capacity - 1);
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

int mcc_hash_map_insert(struct mcc_hash_map *self, const void *key,
			const void *value)
{
	size_t index;
	struct mcc_bucket **curr;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	index = self->hash(key) & (self->cap - 1);
	curr = &self->bkts[index];
	while (*curr) {
		if (self->eq(key, (*curr)->k)) {
			if (self->v.dtor)
				self->v.dtor((*curr)->v);
			memcpy((*curr)->v, value, self->v.size);
			return OK;
		}
		curr = &(*curr)->next;
	}

	*curr = bucket_new(key, value, &self->k, &self->v);
	if (!*curr)
		return CANNOT_ALLOCATE_MEMORY;

	self->len++;

	if (self->len >= self->cap * 3 / 4)
		mcc_hash_map_reserve(self, self->len);

	return OK;
}

void mcc_hash_map_remove(struct mcc_hash_map *self, const void *key)
{
	size_t index;
	struct mcc_bucket **curr, *tmp;

	if (!self || !key)
		return;

	index = self->hash(key) & (self->cap - 1);
	curr = &self->bkts[index];
	while (*curr) {
		if (self->eq(key, (*curr)->k)) {
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

	for (size_t i = 0; i < self->cap; i++) {
		bucket_delete(self->bkts[i], &self->k, &self->v);
		self->bkts[i] = NULL;
	}
	self->len = 0;
}

int mcc_hash_map_get(struct mcc_hash_map *self, const void *key, void *value)
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
	size_t index;
	struct mcc_bucket *curr;

	if (!self || !key)
		return NULL;

	index = self->hash(key) & (self->cap - 1);
	curr = self->bkts[index];
	while (curr) {
		if (self->eq(key, curr->k))
			return curr->v;
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

static bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *self, void *key,
				   void *value)
{
	if (!self || !self->curr)
		return false;

	if (self->idx >= self->container->cap && !self->curr->next)
		return false;

	if (key)
		memcpy(key, self->curr->k, self->container->k.size);

	if (value)
		memcpy(value, self->curr->v, self->container->v.size);

	if (self->curr->next) {
		self->curr = self->curr->next;
	} else {
		while (self->idx < self->container->cap &&
		       !self->container->bkts[self->idx])
			self->idx++;

		self->curr = self->idx < self->container->cap
				     ? self->container->bkts[self->idx++]
				     : NULL;
	}
	return true;
}

int mcc_hash_map_iter_init(struct mcc_hash_map *self,
			   struct mcc_hash_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->idx = 0;
	while (iter->idx < self->cap && !self->bkts[iter->idx])
		iter->idx++;

	iter->curr = iter->idx < self->cap ? self->bkts[iter->idx++] : NULL;
	iter->container = self;
	iter->next = mcc_hash_map_iter_next;
	return OK;
}
