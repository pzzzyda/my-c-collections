#ifndef _MCC_PRIORITY_QUEUE_H
#define _MCC_PRIORITY_QUEUE_H

#include "mcc_object.h"

struct mcc_priority_queue;

struct mcc_priority_queue *
mcc_priority_queue_new(const struct mcc_object_interface *T);

void mcc_priority_queue_drop(struct mcc_priority_queue *self);

int mcc_priority_queue_reserve(struct mcc_priority_queue *self,
			       size_t additional);

int mcc_priority_queue_push(struct mcc_priority_queue *self, const void *value);

void mcc_priority_queue_pop(struct mcc_priority_queue *self);

void mcc_priority_queue_clear(struct mcc_priority_queue *self);

int mcc_priority_queue_front(struct mcc_priority_queue *self, void **ref);

size_t mcc_priority_queue_len(struct mcc_priority_queue *self);

size_t mcc_priority_queue_capacity(struct mcc_priority_queue *self);

bool mcc_priority_queue_is_empty(struct mcc_priority_queue *self);

struct mcc_priority_queue_iter;

struct mcc_priority_queue_iter *
mcc_priority_queue_iter_new(struct mcc_priority_queue *queue);

void mcc_priority_queue_iter_drop(struct mcc_priority_queue_iter *self);

bool mcc_priority_queue_iter_next(struct mcc_priority_queue_iter *self,
				  const void **ref);

#endif /* _MCC_PRIORITY_QUEUE_H */
