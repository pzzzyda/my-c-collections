#include "mcc_stack.h"
#include "mcc_vector.h"

#define VECTOR(PTR) ((struct mcc_vector *)(PTR))
#define STACK(PTR) ((struct mcc_stack *)(PTR))

struct mcc_stack *mcc_stack_new(const struct mcc_object_interface *T)
{
	return STACK(mcc_vector_new(T));
}

void mcc_stack_drop(struct mcc_stack *self)
{
	mcc_vector_drop(VECTOR(self));
}

int mcc_stack_push(struct mcc_stack *self, const void *value)
{
	return mcc_vector_push(VECTOR(self), value);
}

void mcc_stack_pop(struct mcc_stack *self)
{
	mcc_vector_pop(VECTOR(self));
}

int mcc_stack_top(struct mcc_stack *self, void **ref)
{
	return mcc_vector_back(VECTOR(self), ref);
}

void mcc_stack_clear(struct mcc_stack *self)
{
	mcc_vector_clear(VECTOR(self));
}

size_t mcc_stack_len(struct mcc_stack *self)
{
	return mcc_vector_len(VECTOR(self));
}

bool mcc_stack_is_empty(struct mcc_stack *self)
{
	return mcc_vector_is_empty(VECTOR(self));
}
