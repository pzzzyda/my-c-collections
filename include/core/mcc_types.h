#ifndef _MCC_TYPES_H
#define _MCC_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t mcc_i8;
typedef int16_t mcc_i16;
typedef int32_t mcc_i32;
typedef int64_t mcc_i64;
typedef uint8_t mcc_u8;
typedef uint16_t mcc_u16;
typedef uint32_t mcc_u32;
typedef uint64_t mcc_u64;
typedef float mcc_f32;
typedef double mcc_f64;
typedef size_t mcc_usize;
typedef bool mcc_bool;

typedef struct {
	const char *ptr;
} mcc_str;

#endif /* _MCC_TYPES_H */
