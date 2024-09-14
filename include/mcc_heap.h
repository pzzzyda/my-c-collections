#ifndef _MCC_HEAP_H
#define _MCC_HEAP_H

#include "mcc_iterator.h"
#include "mcc_object.h"

struct mcc_heap;

struct mcc_heap_iter {
	struct mcc_iterator_interface interface;
	size_t idx;
	struct mcc_heap *container;
};

struct mcc_heap *mcc_heap_new(const struct mcc_object_interface *element,
			      mcc_compare_f cmp);

void mcc_heap_delete(struct mcc_heap *self);

int mcc_heap_reserve(struct mcc_heap *self, size_t additional);

int mcc_heap_shrink_to_fit(struct mcc_heap *self);

int mcc_heap_push(struct mcc_heap *self, void *value);

void mcc_heap_pop(struct mcc_heap *self);

void mcc_heap_clear(struct mcc_heap *self);

int mcc_heap_peek(struct mcc_heap *self, void *value);

void *mcc_heap_peek_ptr(struct mcc_heap *self);

size_t mcc_heap_capacity(struct mcc_heap *self);

size_t mcc_heap_len(struct mcc_heap *self);

bool mcc_heap_is_empty(struct mcc_heap *self);

int mcc_heap_iter_init(struct mcc_heap *self, struct mcc_heap_iter *iter);

bool mcc_heap_iter_next(struct mcc_heap_iter *iter, void *result);

extern const struct mcc_object_interface mcc_heap_object_interface;

#endif /* _MCC_HEAP_H */
