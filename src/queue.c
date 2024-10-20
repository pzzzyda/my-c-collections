#include "mcc_deque.h"
#include "mcc_queue.h"

#define DEQUE(PTR) ((struct mcc_deque *)(PTR))
#define QUEUE(PTR) ((struct mcc_queue *)(PTR))

struct mcc_queue *mcc_queue_new(const struct mcc_object_interface *T)
{
	return QUEUE(mcc_deque_new(T));
}

void mcc_queue_drop(struct mcc_queue *self)
{
	mcc_deque_drop(DEQUE(self));
}

int mcc_queue_push(struct mcc_queue *self, const void *value)
{
	return mcc_deque_push_back(DEQUE(self), value);
}

void mcc_queue_pop(struct mcc_queue *self)
{
	mcc_deque_pop_front(DEQUE(self));
}

int mcc_queue_front(struct mcc_queue *self, void **ref)
{
	return mcc_deque_front(DEQUE(self), ref);
}

void mcc_queue_clear(struct mcc_queue *self)
{
	mcc_deque_clear(DEQUE(self));
}

size_t mcc_queue_len(struct mcc_queue *self)
{
	return mcc_deque_len(DEQUE(self));
}

bool mcc_queue_is_empty(struct mcc_queue *self)
{
	return mcc_deque_is_empty(DEQUE(self));
}
