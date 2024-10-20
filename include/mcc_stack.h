#ifndef _MCC_STACK_H
#define _MCC_STACK_H

#include "mcc_object.h"

struct mcc_stack;

struct mcc_stack *mcc_stack_new(const struct mcc_object_interface *T);

void mcc_stack_drop(struct mcc_stack *self);

int mcc_stack_push(struct mcc_stack *self, const void *value);

void mcc_stack_pop(struct mcc_stack *self);

int mcc_stack_top(struct mcc_stack *self, void **ref);

void mcc_stack_clear(struct mcc_stack *self);

size_t mcc_stack_len(struct mcc_stack *self);

bool mcc_stack_is_empty(struct mcc_stack *self);

#endif /* _MCC_STACK_H */
