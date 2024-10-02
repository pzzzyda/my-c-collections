#ifndef _MCC_UTILS_H
#define _MCC_UTILS_H

#include <stdint.h>
#include <string.h>

static inline void mcc_memswap(void *p1, void *p2, size_t n)
{
	uint8_t tmp[32];
	uint8_t *b1 = p1;
	uint8_t *b2 = p2;
	while (n >= 32) {
		memcpy(tmp, b1, 32);
		memcpy(b1, b2, 32);
		memcpy(b2, tmp, 32);
		b1 += 32;
		b2 += 32;
		n -= 32;
	}
	while (n > 0) {
		tmp[0] = *b1;
		*b1++ = *b2;
		*b2++ = tmp[0];
		n -= 1;
	}
}

struct mcc_kv_pair {
	void *key;
	void *value;
};

#endif /* _MCC_UTILS_H */
