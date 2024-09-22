#ifndef _MCC_ITERATOR_H
#define _MCC_ITERATOR_H

#include "mcc_types.h"

typedef mcc_bool (*mcc_iterator_next_fn)(void *iter, void *result);

struct mcc_iterator_interface {
	mcc_iterator_next_fn next;
};

static inline mcc_bool mcc_iter_next(void *iter, void *result)
{
	return ((struct mcc_iterator_interface *)iter)->next(iter, result);
}

#endif /* _MCC_ITERATOR_H */
