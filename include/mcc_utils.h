#ifndef _MCC_UTILS_H
#define _MCC_UTILS_H

#include "mcc_type.h"
#include <stdlib.h>
#include <string.h>

static inline void mcc_memswap(void *p1, void *p2, size_t n)
{
	uint8_t *tmp;
	if (n >= 32 && ((tmp = malloc(n)) != NULL)) {
		memcpy(tmp, p1, n);
		memcpy(p1, p2, n);
		memcpy(p2, tmp, n);
		free(tmp);
	} else {
		uint8_t t[32];
		while (n >= 32) {
			memcpy(t, p1, 32);
			p1 = (uint8_t *)memcpy(p1, p2, 32) + 32;
			p2 = (uint8_t *)memcpy(p2, t, 32) + 32;
			n -= 32;
		}
		while (n > 0) {
			t[0] = ((uint8_t *)p1)[--n];
			((uint8_t *)p1)[n] = ((uint8_t *)p2)[n];
			((uint8_t *)p2)[n] = t[0];
		}
	}
}

#endif /* _MCC_UTILS_H */