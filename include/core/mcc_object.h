#ifndef _MCC_OBJECT_H
#define _MCC_OBJECT_H

#include "mcc_types.h"

typedef void (*mcc_destruct_fn)(void *);
typedef mcc_i32 (*mcc_compare_fn)(const void *, const void *);
typedef mcc_usize (*mcc_hash_fn)(const void *);

struct mcc_object_interface {
	mcc_usize size;
	mcc_destruct_fn dtor;
	mcc_compare_fn cmp;
	mcc_hash_fn hash;
};

extern const struct mcc_object_interface mcc_i8_i;
extern const struct mcc_object_interface mcc_i16_i;
extern const struct mcc_object_interface mcc_i32_i;
extern const struct mcc_object_interface mcc_i64_i;
extern const struct mcc_object_interface mcc_u8_i;
extern const struct mcc_object_interface mcc_u16_i;
extern const struct mcc_object_interface mcc_u32_i;
extern const struct mcc_object_interface mcc_u64_i;
extern const struct mcc_object_interface mcc_f32_i;
extern const struct mcc_object_interface mcc_f64_i;
extern const struct mcc_object_interface mcc_str_i;
extern const struct mcc_object_interface mcc_usize_i;
extern const struct mcc_object_interface mcc_vector_i;
extern const struct mcc_object_interface mcc_deque_i;
extern const struct mcc_object_interface mcc_list_i;
extern const struct mcc_object_interface mcc_hash_map_i;
extern const struct mcc_object_interface mcc_map_i;

#endif /* _MCC_OBJECT_H */
