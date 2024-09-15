#include "mcc_object.h"
#include <stdint.h>
#include <string.h>

static int i8_cmp(const void *a, const void *b)
{
	return *(const int8_t *)a - *(const int8_t *)b;
}

static size_t i8_hash(const void *key)
{
	return (size_t)(*(const int8_t *)key);
}

static int u8_cmp(const void *a, const void *b)
{
	uint8_t _a = *(const uint8_t *)a;
	uint8_t _b = *(const uint8_t *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t u8_hash(const void *key)
{
	return (size_t)(*(const uint8_t *)key);
}

static int i16_cmp(const void *a, const void *b)
{
	return *(const int16_t *)a - *(const int16_t *)b;
}

static size_t i16_hash(const void *key)
{
	return (size_t)(*(const int16_t *)key);
}

static int u16_cmp(const void *a, const void *b)
{
	uint16_t _a = *(const uint16_t *)a;
	uint16_t _b = *(const uint16_t *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t u16_hash(const void *key)
{
	return (size_t)(*(const uint16_t *)key);
}

static int i32_cmp(const void *a, const void *b)
{
	return *(const int32_t *)a - *(const int32_t *)b;
}

static size_t i32_hash(const void *key)
{
	return (size_t)(*(const int32_t *)key);
}

static int u32_cmp(const void *a, const void *b)
{
	uint32_t _a = *(const uint32_t *)a;
	uint32_t _b = *(const uint32_t *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t u32_hash(const void *key)
{
	return (size_t)(*(const uint32_t *)key);
}

static int i64_cmp(const void *a, const void *b)
{
	return *(const int64_t *)a - *(const int64_t *)b;
}

static size_t i64_hash(const void *key)
{
	return (size_t)(*(const int64_t *)key);
}

static int u64_cmp(const void *a, const void *b)
{
	uint64_t _a = *(const uint64_t *)a;
	uint64_t _b = *(const uint64_t *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t u64_hash(const void *key)
{
	return (size_t)(*(const uint64_t *)key);
}

static int float_cmp(const void *a, const void *b)
{
	float _a = *(const float *)a;
	float _b = *(const float *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t float_hash(const void *key)
{
	return (size_t)(*(const float *)key);
}

static int double_cmp(const void *a, const void *b)
{
	double _a = *(const double *)a;
	double _b = *(const double *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t double_hash(const void *key)
{
	return (size_t)(*(const double *)key);
}

static int str_cmp(const void *a, const void *b)
{
	return strcmp(a, b);
}

static size_t str_hash(const void *key)
{
	const char *c = key;
	size_t h = 0;

	while (*c) {
		h = h * 31 + *c;
		c++;
	}

	return h;
}

static int size_t_cmp(const void *a, const void *b)
{
	size_t _a = *(const size_t *)a;
	size_t _b = *(const size_t *)b;

	return _a > _b ? 1 : (_a == _b ? 0 : -1);
}

static size_t size_t_hash(const void *key)
{
	return *(const size_t *)key;
}

const struct mcc_object_interface mcc_i8_i = {
	.name = "i8",
	.size = sizeof(int8_t),
	.dtor = NULL,
	.cmp = &i8_cmp,
	.hash = &i8_hash,
};

const struct mcc_object_interface mcc_i16_i = {
	.name = "i16",
	.size = sizeof(int16_t),
	.dtor = NULL,
	.cmp = &i16_cmp,
	.hash = &i16_hash,
};

const struct mcc_object_interface mcc_i32_i = {
	.name = "i32",
	.size = sizeof(int32_t),
	.dtor = NULL,
	.cmp = &i32_cmp,
	.hash = &i32_hash,
};

const struct mcc_object_interface mcc_i64_i = {
	.name = "i64",
	.size = sizeof(int64_t),
	.dtor = NULL,
	.cmp = &i64_cmp,
	.hash = &i64_hash,
};

const struct mcc_object_interface mcc_u8_i = {
	.name = "u8",
	.size = sizeof(uint8_t),
	.dtor = NULL,
	.cmp = &u8_cmp,
	.hash = &u8_hash,
};

const struct mcc_object_interface mcc_u16_i = {
	.name = "u16",
	.size = sizeof(uint16_t),
	.dtor = NULL,
	.cmp = &u16_cmp,
	.hash = &u16_hash,
};

const struct mcc_object_interface mcc_u32_i = {
	.name = "u32",
	.size = sizeof(uint32_t),
	.dtor = NULL,
	.cmp = &u32_cmp,
	.hash = &u32_hash,
};

const struct mcc_object_interface mcc_u64_i = {
	.name = "u64",
	.size = sizeof(uint64_t),
	.dtor = NULL,
	.cmp = &u64_cmp,
	.hash = &u64_hash,
};

const struct mcc_object_interface mcc_float_i = {
	.name = "float",
	.size = sizeof(float),
	.dtor = NULL,
	.cmp = &float_cmp,
	.hash = &float_hash,
};

const struct mcc_object_interface mcc_double_i = {
	.name = "double",
	.size = sizeof(double),
	.dtor = NULL,
	.cmp = &double_cmp,
	.hash = &double_hash,
};

const struct mcc_object_interface mcc_str_i = {
	.name = "str",
	.size = sizeof(char *),
	.dtor = NULL,
	.cmp = &str_cmp,
	.hash = &str_hash,
};

const struct mcc_object_interface mcc_size_t_i = {
	.name = "size_t",
	.size = sizeof(size_t),
	.dtor = NULL,
	.cmp = &size_t_cmp,
	.hash = &size_t_hash,
};
