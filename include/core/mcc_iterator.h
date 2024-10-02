#ifndef _MCC_ITERATOR_H
#define _MCC_ITERATOR_H

#include <stdbool.h>

typedef bool (*mcc_iterator_next_fn)(void *self, void *result);

struct mcc_iterator_interface {
	mcc_iterator_next_fn next;
};

struct mcc_iterator {
	const struct mcc_iterator_interface *iter_intf;
};

static inline const struct mcc_iterator_interface *mcc_iter_intf_of(void *self)
{
	return ((struct mcc_iterator *)self)->iter_intf;
}

static inline bool mcc_iter_next(void *self, void *result)
{
	return mcc_iter_intf_of(self)->next(self, result);
}

#endif /* _MCC_ITERATOR_H */
