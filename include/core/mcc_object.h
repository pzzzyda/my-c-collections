#ifndef _MCC_OBJECT_H
#define _MCC_OBJECT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*mcc_destruct_fn)(void *self);
typedef int (*mcc_compare_fn)(const void *self, const void *other);
typedef size_t (*mcc_hash_fn)(const void *key);

struct mcc_object_interface {
	const size_t size;
	const mcc_destruct_fn dtor;
	const mcc_compare_fn cmp;
	const mcc_hash_fn hash;
};

typedef const char *mcc_str_t;

#ifndef CHAR
#define CHAR (&__mcc_char_obj_intf)
#endif
#ifndef UCHAR
#define UCHAR (&__mcc_uchar_obj_intf)
#endif
#ifndef SHORT
#define SHORT (&__mcc_short_obj_intf)
#endif
#ifndef USHORT
#define USHORT (&__mcc_ushort_obj_intf)
#endif
#ifndef INT
#define INT (&__mcc_int_obj_intf)
#endif
#ifndef UINT
#define UINT (&__mcc_uint_obj_intf)
#endif
#ifndef LONG
#define LONG (&__mcc_long_obj_intf)
#endif
#ifndef ULONG
#define ULONG (&__mcc_ulong_obj_intf)
#endif
#ifndef LONG_LONG
#define LONG_LONG (&__mcc_long_long_obj_intf)
#endif
#ifndef ULONG_LONG
#define ULONG_LONG (&__mcc_ulong_long_obj_intf)
#endif
#ifndef FLOAT
#define FLOAT (&__mcc_float_obj_intf)
#endif
#ifndef DOUBLE
#define DOUBLE (&__mcc_double_obj_intf)
#endif
#ifndef LONG_DOUBLE
#define LONG_DOUBLE (&__mcc_long_double_obj_intf)
#endif
#ifndef INT8_T
#define INT8_T (&__mcc_int8_t_obj_intf)
#endif
#ifndef UINT8_T
#define UINT8_T (&__mcc_uint8_t_obj_intf)
#endif
#ifndef INT16_T
#define INT16_T (&__mcc_int16_t_obj_intf)
#endif
#ifndef UINT16_T
#define UINT16_T (&__mcc_uint16_t_obj_intf)
#endif
#ifndef INT32_T
#define INT32_T (&__mcc_int32_t_obj_intf)
#endif
#ifndef UINT32_T
#define UINT32_T (&__mcc_uint32_t_obj_intf)
#endif
#ifndef INT64_T
#define INT64_T (&__mcc_int64_t_obj_intf)
#endif
#ifndef UINT64_T
#define UINT64_T (&__mcc_uint64_t_obj_intf)
#endif
#ifndef SIZE_T
#define SIZE_T (&__mcc_size_t_obj_intf)
#endif
#ifndef STR
#define STR (&__mcc_str_obj_intf)
#endif
#define MCC_VECTOR (&__mcc_vector_obj_intf)
#define MCC_DEQUE (&__mcc_deque_obj_intf)
#define MCC_LIST (&__mcc_list_obj_intf)
#define MCC_HASH_MAP (&__mcc_hash_map_obj_intf)
#define MCC_RB_MAP (&__mcc_rb_map_obj_intf)

extern const struct mcc_object_interface __mcc_char_obj_intf;
extern const struct mcc_object_interface __mcc_uchar_obj_intf;
extern const struct mcc_object_interface __mcc_short_obj_intf;
extern const struct mcc_object_interface __mcc_ushort_obj_intf;
extern const struct mcc_object_interface __mcc_int_obj_intf;
extern const struct mcc_object_interface __mcc_uint_obj_intf;
extern const struct mcc_object_interface __mcc_long_obj_intf;
extern const struct mcc_object_interface __mcc_ulong_obj_intf;
extern const struct mcc_object_interface __mcc_long_long_obj_intf;
extern const struct mcc_object_interface __mcc_ulong_long_obj_intf;
extern const struct mcc_object_interface __mcc_float_obj_intf;
extern const struct mcc_object_interface __mcc_double_obj_intf;
extern const struct mcc_object_interface __mcc_long_double_obj_intf;
extern const struct mcc_object_interface __mcc_int8_t_obj_intf;
extern const struct mcc_object_interface __mcc_uint8_t_obj_intf;
extern const struct mcc_object_interface __mcc_int16_t_obj_intf;
extern const struct mcc_object_interface __mcc_uint16_t_obj_intf;
extern const struct mcc_object_interface __mcc_int32_t_obj_intf;
extern const struct mcc_object_interface __mcc_uint32_t_obj_intf;
extern const struct mcc_object_interface __mcc_int64_t_obj_intf;
extern const struct mcc_object_interface __mcc_uint64_t_obj_intf;
extern const struct mcc_object_interface __mcc_size_t_obj_intf;
extern const struct mcc_object_interface __mcc_str_obj_intf;
extern const struct mcc_object_interface __mcc_vector_obj_intf;
extern const struct mcc_object_interface __mcc_deque_obj_intf;
extern const struct mcc_object_interface __mcc_list_obj_intf;
extern const struct mcc_object_interface __mcc_hash_map_obj_intf;
extern const struct mcc_object_interface __mcc_rb_map_obj_intf;

#endif /* _MCC_OBJECT_H */
