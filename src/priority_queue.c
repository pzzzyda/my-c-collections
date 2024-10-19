#include "mcc_priority_queue.h"
#include "mcc_vector.h"
#include "sift_down.h"

#define VECTOR(PTR) ((struct mcc_vector *)(PTR))
#define PRIORITY_QUEUE(PTR) ((struct mcc_priority_queue *)(PTR))
#define VECTOR_ITER(PTR) ((struct mcc_vector_iter *)(PTR))
#define PRIORITY_QUEUE_ITER(PTR) ((struct mcc_priority_queue_iter *)(PTR))

struct mcc_priority_queue *
mcc_priority_queue_new(const struct mcc_object_interface *T)
{
	return PRIORITY_QUEUE(mcc_vector_new(T));
}

void mcc_priority_queue_drop(struct mcc_priority_queue *self)
{
	mcc_vector_drop(VECTOR(self));
}

int mcc_priority_queue_reserve(struct mcc_priority_queue *self,
			       size_t additional)
{
	return mcc_vector_reserve(VECTOR(self), additional);
}

int mcc_priority_queue_push(struct mcc_priority_queue *self, const void *value)
{
	int err;
	size_t i;

	err = mcc_vector_push(VECTOR(self), value);
	if (err)
		return err;

	i = mcc_vector_len(VECTOR(self));
	if (i > 1) {
		i >>= 1;
		while (true) {
			sift_down(VECTOR(self), i);
			if (i-- == 0)
				break;
		}
	}
	return err;
}

void mcc_priority_queue_pop(struct mcc_priority_queue *self)
{
	size_t len;

	if (!self)
		return;

	len = mcc_vector_len(VECTOR(self));
	if (!len)
		return;

	mcc_vector_swap(VECTOR(self), 0, len - 1);
	mcc_vector_pop(VECTOR(self));
	sift_down(VECTOR(self), 0);
}

void mcc_priority_queue_clear(struct mcc_priority_queue *self)
{
	mcc_vector_clear(VECTOR(self));
}

int mcc_priority_queue_front(struct mcc_priority_queue *self, void **ref)
{
	return mcc_vector_front(VECTOR(self), ref);
}

size_t mcc_priority_queue_len(struct mcc_priority_queue *self)
{
	return mcc_vector_len(VECTOR(self));
}

size_t mcc_priority_queue_capacity(struct mcc_priority_queue *self)
{
	return mcc_vector_capacity(VECTOR(self));
}

bool mcc_priority_queue_is_empty(struct mcc_priority_queue *self)
{
	return mcc_vector_is_empty(VECTOR(self));
}

struct mcc_priority_queue_iter *
mcc_priority_queue_iter_new(struct mcc_priority_queue *queue)
{
	return PRIORITY_QUEUE_ITER(mcc_vector_iter_new(VECTOR(queue)));
}

void mcc_priority_queue_iter_drop(struct mcc_priority_queue_iter *self)
{
	mcc_vector_iter_drop(VECTOR_ITER(self));
}

bool mcc_priority_queue_iter_next(struct mcc_priority_queue_iter *self,
				  const void **ref)
{
	void *mut_ref;

	if (mcc_vector_iter_next(VECTOR_ITER(self), &mut_ref)) {
		*ref = mut_ref;
		return true;
	} else {
		return false;
	}
}
