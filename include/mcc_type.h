#ifndef _MCC_TYPE_H
#define _MCC_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int (*mcc_compare_f)(const void *a, const void *b);

typedef size_t (*mcc_hash_f)(const void *key);

typedef bool (*mcc_equal_f)(const void *a, const void *b);

typedef bool (*mcc_iter_next_f)(void *iter, void *result);

#endif /* _MCC_TYPE_H */
