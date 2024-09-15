#ifndef _MCC_OBJECT_H
#define _MCC_OBJECT_H

#include "mcc_type.h"

struct mcc_object_interface {
	const char *name;
	size_t size;
	void (*dtor)(void *);
	mcc_compare_f cmp;
	mcc_hash_f hash;
};

extern const struct mcc_object_interface mcc_i8_i;
extern const struct mcc_object_interface mcc_i16_i;
extern const struct mcc_object_interface mcc_i32_i;
extern const struct mcc_object_interface mcc_i64_i;

extern const struct mcc_object_interface mcc_u8_i;
extern const struct mcc_object_interface mcc_u16_i;
extern const struct mcc_object_interface mcc_u32_i;
extern const struct mcc_object_interface mcc_u64_i;

extern const struct mcc_object_interface mcc_float_i;
extern const struct mcc_object_interface mcc_double_i;

extern const struct mcc_object_interface mcc_str_i;

extern const struct mcc_object_interface mcc_size_t_i;

extern const struct mcc_object_interface mcc_vector_i;
extern const struct mcc_object_interface mcc_heap_i;
extern const struct mcc_object_interface mcc_deque_i;
extern const struct mcc_object_interface mcc_list_i;
extern const struct mcc_object_interface mcc_hash_map_i;
extern const struct mcc_object_interface mcc_map_i;

#endif /* _MCC_OBJECT_H */
