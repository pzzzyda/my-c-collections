#include "mcc_hash_map.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_bucket {
	struct mcc_bucket *next;
	struct mcc_kv_pair pair;
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

	self = malloc(sizeof(struct mcc_bucket));
	if (!self)
		return NULL;

	self->next = NULL;

	self->pair.key = malloc(ki->size);
	if (!self->pair.key)
		goto handle_key_allocate_err;
	memcpy(self->pair.key, k, ki->size);

	self->pair.value = malloc(vi->size);
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

static int mcc_hash_map_rehash(struct mcc_hash_map *self, size_t new_capacity)
{
	struct mcc_bucket **new_bkts, *curr, *tmp;
	size_t index;

	new_bkts = calloc(new_capacity, sizeof(struct mcc_bucket *));
	if (!new_bkts)
		return CANNOT_ALLOCATE_MEMORY;

	for (size_t i = 0; i < self->cap; i++) {
		curr = self->bkts[i];
		while (curr) {
			tmp = curr->next;
			index = self->hash(curr->pair.key) & (new_capacity - 1);
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
	size_t new_capacity, needs;

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
		if (self->eq(key, (*curr)->pair.key)) {
			if (self->v.dtor)
				self->v.dtor((*curr)->pair.value);
			memcpy((*curr)->pair.value, value, self->v.size);
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
		if (self->eq(key, (*curr)->pair.key)) {
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
		if (self->eq(key, curr->pair.key))
			return curr->pair.value;
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

int mcc_hash_map_iter_init(struct mcc_hash_map *self,
			   struct mcc_hash_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iter_next_f)&mcc_hash_map_iter_next;
	iter->idx = 0;
	while (iter->idx < self->cap && !self->bkts[iter->idx])
		iter->idx++;

	iter->curr = iter->idx < self->cap ? self->bkts[iter->idx++] : NULL;
	iter->container = self;
	return OK;
}

bool mcc_hash_map_iter_next(struct mcc_hash_map_iter *iter, void *result)
{
	struct mcc_bucket *curr;
	struct mcc_hash_map *map;

	if (!iter || !iter->curr)
		return false;

	curr = iter->curr;
	map = iter->container;
	if (iter->idx >= map->cap && !curr->next)
		return false;

	if (result)
		memcpy(result, &curr->pair, sizeof(struct mcc_kv_pair));

	if (curr->next) {
		iter->curr = curr->next;
	} else {
		while (iter->idx < map->cap && !map->bkts[iter->idx])
			iter->idx++;

		if (iter->idx < map->cap) {
			iter->curr = map->bkts[iter->idx];
			iter->idx++;
		} else {
			iter->curr = NULL;
		}
	}
	return true;
}
