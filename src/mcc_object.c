#include "core/mcc_object.h"
#include <string.h>

static mcc_i32 i8_cmp(const void *a, const void *b)
{
	return *(const mcc_i8 *)a - *(const mcc_i8 *)b;
}

static mcc_usize i8_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_i8 *)key);
}

static mcc_i32 u8_cmp(const void *a, const void *b)
{
	mcc_u8 _a = *(const mcc_u8 *)a;
	mcc_u8 _b = *(const mcc_u8 *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize u8_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_u8 *)key);
}

static mcc_i32 i16_cmp(const void *a, const void *b)
{
	return *(const mcc_i16 *)a - *(const mcc_i16 *)b;
}

static mcc_usize i16_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_i16 *)key);
}

static mcc_i32 u16_cmp(const void *a, const void *b)
{
	mcc_u16 _a = *(const mcc_u16 *)a;
	mcc_u16 _b = *(const mcc_u16 *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize u16_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_u16 *)key);
}

static mcc_i32 i32_cmp(const void *a, const void *b)
{
	return *(const mcc_i32 *)a - *(const mcc_i32 *)b;
}

static mcc_usize i32_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_i32 *)key);
}

static mcc_i32 u32_cmp(const void *a, const void *b)
{
	mcc_u32 _a = *(const mcc_u32 *)a;
	mcc_u32 _b = *(const mcc_u32 *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize u32_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_u32 *)key);
}

static mcc_i32 i64_cmp(const void *a, const void *b)
{
	return *(const mcc_i64 *)a - *(const mcc_i64 *)b;
}

static mcc_usize i64_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_i64 *)key);
}

static mcc_i32 u64_cmp(const void *a, const void *b)
{
	mcc_u64 _a = *(const mcc_u64 *)a;
	mcc_u64 _b = *(const mcc_u64 *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize u64_hash(const void *key)
{
	return (mcc_usize)(*(const mcc_u64 *)key);
}

static mcc_i32 f32_cmp(const void *a, const void *b)
{
	float _a = *(const float *)a;
	float _b = *(const float *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize f32_hash(const void *key)
{
	return (mcc_usize)(*(const float *)key);
}

static mcc_i32 f64_cmp(const void *a, const void *b)
{
	double _a = *(const double *)a;
	double _b = *(const double *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize f64_hash(const void *key)
{
	return (mcc_usize)(*(const double *)key);
}

static mcc_i32 str_cmp(const void *a, const void *b)
{
	return strcmp(*((const mcc_str *)a), *((const mcc_str *)b));
}

static mcc_usize str_hash(const void *key)
{
	const char *c = *((const mcc_str *)key);
	mcc_usize h = 0;

	while (*c) {
		h = h * 31 + *c;
		c++;
	}

	return h;
}

static mcc_i32 usize_cmp(const void *a, const void *b)
{
	mcc_usize _a = *(const mcc_usize *)a;
	mcc_usize _b = *(const mcc_usize *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static mcc_usize usize_hash(const void *key)
{
	return *(const mcc_usize *)key;
}

const struct mcc_object_interface mcc_i8_i = {
	.size = sizeof(mcc_i8),
	.dtor = NULL,
	.cmp = &i8_cmp,
	.hash = &i8_hash,
};

const struct mcc_object_interface mcc_i16_i = {
	.size = sizeof(mcc_i16),
	.dtor = NULL,
	.cmp = &i16_cmp,
	.hash = &i16_hash,
};

const struct mcc_object_interface mcc_i32_i = {
	.size = sizeof(mcc_i32),
	.dtor = NULL,
	.cmp = &i32_cmp,
	.hash = &i32_hash,
};

const struct mcc_object_interface mcc_i64_i = {
	.size = sizeof(mcc_i64),
	.dtor = NULL,
	.cmp = &i64_cmp,
	.hash = &i64_hash,
};

const struct mcc_object_interface mcc_u8_i = {
	.size = sizeof(mcc_u8),
	.dtor = NULL,
	.cmp = &u8_cmp,
	.hash = &u8_hash,
};

const struct mcc_object_interface mcc_u16_i = {
	.size = sizeof(mcc_u16),
	.dtor = NULL,
	.cmp = &u16_cmp,
	.hash = &u16_hash,
};

const struct mcc_object_interface mcc_u32_i = {
	.size = sizeof(mcc_u32),
	.dtor = NULL,
	.cmp = &u32_cmp,
	.hash = &u32_hash,
};

const struct mcc_object_interface mcc_u64_i = {
	.size = sizeof(mcc_u64),
	.dtor = NULL,
	.cmp = &u64_cmp,
	.hash = &u64_hash,
};

const struct mcc_object_interface mcc_f32_i = {
	.size = sizeof(float),
	.dtor = NULL,
	.cmp = &f32_cmp,
	.hash = &f32_hash,
};

const struct mcc_object_interface mcc_f64_i = {
	.size = sizeof(double),
	.dtor = NULL,
	.cmp = &f64_cmp,
	.hash = &f64_hash,
};

const struct mcc_object_interface mcc_str_i = {
	.size = sizeof(char *),
	.dtor = NULL,
	.cmp = &str_cmp,
	.hash = &str_hash,
};

const struct mcc_object_interface mcc_usize_i = {
	.size = sizeof(mcc_usize),
	.dtor = NULL,
	.cmp = &usize_cmp,
	.hash = &usize_hash,
};
