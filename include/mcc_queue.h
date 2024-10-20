#ifndef _MCC_QUEUE_H
#define _MCC_QUEUE_H

#include "mcc_object.h"

struct mcc_queue;

struct mcc_queue *mcc_queue_new(const struct mcc_object_interface *T);

void mcc_queue_drop(struct mcc_queue *self);

int mcc_queue_push(struct mcc_queue *self, const void *value);

void mcc_queue_pop(struct mcc_queue *self);

int mcc_queue_front(struct mcc_queue *self, void **ref);

void mcc_queue_clear(struct mcc_queue *self);

size_t mcc_queue_len(struct mcc_queue *self);

bool mcc_queue_is_empty(struct mcc_queue *self);

#endif /* _MCC_QUEUE_H */
