#ifndef _MCC_PRIORITY_QUEUE_H
#define _MCC_PRIORITY_QUEUE_H

#include "core/mcc_object.h"
#include "mcc_err.h"

struct mcc_priority_queue;

struct mcc_priority_queue *
mcc_priority_queue_new(const struct mcc_object_interface *T);

void mcc_priority_queue_delete(struct mcc_priority_queue *self);

mcc_err_t mcc_priority_queue_reserve(struct mcc_priority_queue *self,
				     size_t additional);

mcc_err_t mcc_priority_queue_push(struct mcc_priority_queue *self,
				  const void *value);

void mcc_priority_queue_pop(struct mcc_priority_queue *self);

void mcc_priority_queue_clear(struct mcc_priority_queue *self);

mcc_err_t mcc_priority_queue_front(struct mcc_priority_queue *self,
				   void *value);

size_t mcc_priority_queue_len(struct mcc_priority_queue *self);

size_t mcc_priority_queue_capacity(struct mcc_priority_queue *self);

bool mcc_priority_queue_is_empty(struct mcc_priority_queue *self);

#endif /* _MCC_PRIORITY_QUEUE_H */
