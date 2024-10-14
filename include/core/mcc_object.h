#ifndef _MCC_OBJECT_H
#define _MCC_OBJECT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*mcc_drop_fn)(void *self);
typedef int (*mcc_compare_fn)(const void *self, const void *other);
typedef size_t (*mcc_hash_fn)(const void *key);

struct mcc_object_interface {
	const size_t size;
	const mcc_drop_fn drop;
	const mcc_compare_fn cmp;
	const mcc_hash_fn hash;
};

typedef const char *mcc_str_t;

const struct mcc_object_interface *mcc_str(void);

const struct mcc_object_interface *mcc_char(void);
const struct mcc_object_interface *mcc_short(void);
const struct mcc_object_interface *mcc_int(void);
const struct mcc_object_interface *mcc_long(void);
const struct mcc_object_interface *mcc_long_long(void);

const struct mcc_object_interface *mcc_uchar(void);
const struct mcc_object_interface *mcc_ushort(void);
const struct mcc_object_interface *mcc_uint(void);
const struct mcc_object_interface *mcc_ulong(void);
const struct mcc_object_interface *mcc_ulong_long(void);

const struct mcc_object_interface *mcc_float(void);
const struct mcc_object_interface *mcc_double(void);
const struct mcc_object_interface *mcc_long_double(void);

#endif /* _MCC_OBJECT_H */
