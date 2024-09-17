#ifndef _MCC_ITERATOR_H
#define _MCC_ITERATOR_H

#include "mcc_type.h"

struct mcc_iterator_interface {
	mcc_iter_next_f next;
};

static inline bool mcc_iter_next(void *iter, void *result) {
	return ((struct mcc_iterator_interface *)iter)->next(iter, result);
}

#endif /* _MCC_ITERATOR_H */
