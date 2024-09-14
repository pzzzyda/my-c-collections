#ifndef _MCC_ITERATOR_H
#define _MCC_ITERATOR_H

#include "mcc_type.h"

struct mcc_iterator_interface {
	mcc_iter_next_f next;
};

#endif /* _MCC_ITERATOR_H */
