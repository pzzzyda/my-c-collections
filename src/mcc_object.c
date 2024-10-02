#include "core/mcc_object.h"
#include <string.h>

static int mcc_char_cmp(const void *self, const void *other)
{
	const signed char *a = self;
	const signed char *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_char_hash(const void *key)
{
	return *(signed char *)key;
}

static int mcc_uchar_cmp(const void *self, const void *other)
{
	const unsigned char *a = self;
	const unsigned char *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uchar_hash(const void *key)
{
	return *(unsigned char *)key;
}

static int mcc_short_cmp(const void *self, const void *other)
{
	const signed short *a = self;
	const signed short *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_short_hash(const void *key)
{
	return *(signed short *)key;
}

static int mcc_ushort_cmp(const void *self, const void *other)
{
	const unsigned short *a = self;
	const unsigned short *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_ushort_hash(const void *key)
{
	return *(unsigned short *)key;
}

static int mcc_int_cmp(const void *self, const void *other)
{
	const signed int *a = self;
	const signed int *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_int_hash(const void *key)
{
	return *(signed int *)key;
}

static int mcc_uint_cmp(const void *self, const void *other)
{
	const unsigned int *a = self;
	const unsigned int *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uint_hash(const void *key)
{
	return *(unsigned int *)key;
}

static int mcc_long_cmp(const void *self, const void *other)
{
	const signed long *a = self;
	const signed long *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_long_hash(const void *key)
{
	return *(signed long *)key;
}

static int mcc_ulong_cmp(const void *self, const void *other)
{
	const unsigned long *a = self;
	const unsigned long *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_ulong_hash(const void *key)
{
	return *(unsigned long *)key;
}

static int mcc_long_long_cmp(const void *self, const void *other)
{
	const signed long long *a = self;
	const signed long long *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_long_long_hash(const void *key)
{
	return *(signed long long *)key;
}

static int mcc_ulong_long_cmp(const void *self, const void *other)
{
	const unsigned long long *a = self;
	const unsigned long long *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_ulong_long_hash(const void *key)
{
	return *(unsigned long long *)key;
}

static int mcc_int8_t_cmp(const void *self, const void *other)
{
	const int8_t *a = self;
	const int8_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_int8_t_hash(const void *key)
{
	return *(int8_t *)key;
}

static int mcc_uint8_t_cmp(const void *self, const void *other)
{
	const uint8_t *a = self;
	const uint8_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uint8_t_hash(const void *key)
{
	return *(uint8_t *)key;
}

static int mcc_int16_t_cmp(const void *self, const void *other)
{
	const int16_t *a = self;
	const int16_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_int16_t_hash(const void *key)
{
	return *(int16_t *)key;
}

static int mcc_uint16_t_cmp(const void *self, const void *other)
{
	const uint16_t *a = self;
	const uint16_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uint16_t_hash(const void *key)
{
	return *(uint16_t *)key;
}

static int mcc_int32_t_cmp(const void *self, const void *other)
{
	const int32_t *a = self;
	const int32_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_int32_t_hash(const void *key)
{
	return *(int32_t *)key;
}

static int mcc_uint32_t_cmp(const void *self, const void *other)
{
	const uint32_t *a = self;
	const uint32_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uint32_t_hash(const void *key)
{
	return *(uint32_t *)key;
}

static int mcc_int64_t_cmp(const void *self, const void *other)
{
	const int64_t *a = self;
	const int64_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_int64_t_hash(const void *key)
{
	return *(int64_t *)key;
}

static int mcc_uint64_t_cmp(const void *self, const void *other)
{
	const uint64_t *a = self;
	const uint64_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_uint64_t_hash(const void *key)
{
	return *(uint64_t *)key;
}

static int mcc_float_cmp(const void *self, const void *other)
{
	const float *a = self;
	const float *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_float_hash(const void *key)
{
	return *(float *)key;
}

static int mcc_double_cmp(const void *self, const void *other)
{
	const double *a = self;
	const double *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_double_hash(const void *key)
{
	return *(double *)key;
}

static int mcc_long_double_cmp(const void *self, const void *other)
{
	const long double *a = self;
	const long double *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_long_double_hash(const void *key)
{
	return *(long double *)key;
}

static int mcc_size_t_cmp(const void *self, const void *other)
{
	const size_t *a = self;
	const size_t *b = other;

	return *a > *b ? 1 : (*a == *b ? 0 : -1);
}

static size_t mcc_size_t_hash(const void *key)
{
	return *(size_t *)key;
}

static int mcc_str_cmp(const void *self, const void *other)
{
	const mcc_str_t *a = self;
	const mcc_str_t *b = other;

	return strcmp(*a, *b);
}

static size_t mcc_str_hash(const void *key)
{
	const char *c = *((const mcc_str_t *)key);
	size_t h = 0;

	while (*c) {
		h = h * 31 + *c;
		c++;
	}

	return h;
}

const struct mcc_object_interface __mcc_char_obj_intf = {
	.size = sizeof(char),
	.dtor = NULL,
	.cmp = &mcc_char_cmp,
	.hash = &mcc_char_hash,
};

const struct mcc_object_interface __mcc_uchar_obj_intf = {
	.size = sizeof(unsigned char),
	.dtor = NULL,
	.cmp = &mcc_uchar_cmp,
	.hash = &mcc_uchar_hash,
};

const struct mcc_object_interface __mcc_short_obj_intf = {
	.size = sizeof(short),
	.dtor = NULL,
	.cmp = &mcc_short_cmp,
	.hash = &mcc_short_hash,
};

const struct mcc_object_interface __mcc_ushort_obj_intf = {
	.size = sizeof(unsigned short),
	.dtor = NULL,
	.cmp = &mcc_ushort_cmp,
	.hash = &mcc_ushort_hash,
};

const struct mcc_object_interface __mcc_int_obj_intf = {
	.size = sizeof(int),
	.dtor = NULL,
	.cmp = &mcc_int_cmp,
	.hash = &mcc_int_hash,
};

const struct mcc_object_interface __mcc_uint_obj_intf = {
	.size = sizeof(unsigned int),
	.dtor = NULL,
	.cmp = &mcc_uint_cmp,
	.hash = &mcc_uint_hash,
};

const struct mcc_object_interface __mcc_long_obj_intf = {
	.size = sizeof(long),
	.dtor = NULL,
	.cmp = &mcc_long_cmp,
	.hash = &mcc_long_hash,
};

const struct mcc_object_interface __mcc_ulong_obj_intf = {
	.size = sizeof(char),
	.dtor = NULL,
	.cmp = &mcc_ulong_cmp,
	.hash = &mcc_ulong_hash,
};

const struct mcc_object_interface __mcc_long_long_obj_intf = {
	.size = sizeof(long long),
	.dtor = NULL,
	.cmp = &mcc_long_long_cmp,
	.hash = &mcc_long_long_hash,
};

const struct mcc_object_interface __mcc_ulong_long_obj_intf = {
	.size = sizeof(unsigned long long),
	.dtor = NULL,
	.cmp = &mcc_ulong_long_cmp,
	.hash = &mcc_ulong_long_hash,
};

const struct mcc_object_interface __mcc_float_obj_intf = {
	.size = sizeof(float),
	.dtor = NULL,
	.cmp = &mcc_float_cmp,
	.hash = &mcc_float_hash,
};

const struct mcc_object_interface __mcc_double_obj_intf = {
	.size = sizeof(double),
	.dtor = NULL,
	.cmp = &mcc_double_cmp,
	.hash = &mcc_double_hash,
};

const struct mcc_object_interface __mcc_long_double_obj_intf = {
	.size = sizeof(long double),
	.dtor = NULL,
	.cmp = &mcc_long_double_cmp,
	.hash = &mcc_long_double_hash,
};

const struct mcc_object_interface __mcc_int8_t_obj_intf = {
	.size = sizeof(int8_t),
	.dtor = NULL,
	.cmp = &mcc_int8_t_cmp,
	.hash = &mcc_int8_t_hash,
};

const struct mcc_object_interface __mcc_int16_t_obj_intf = {
	.size = sizeof(int16_t),
	.dtor = NULL,
	.cmp = &mcc_int16_t_cmp,
	.hash = &mcc_int16_t_hash,
};

const struct mcc_object_interface __mcc_int32_t_obj_intf = {
	.size = sizeof(int),
	.dtor = NULL,
	.cmp = &mcc_int32_t_cmp,
	.hash = &mcc_int32_t_hash,
};

const struct mcc_object_interface __mcc_int64_t_obj_intf = {
	.size = sizeof(int64_t),
	.dtor = NULL,
	.cmp = &mcc_int64_t_cmp,
	.hash = &mcc_int64_t_hash,
};

const struct mcc_object_interface __mcc_uint8_t_obj_intf = {
	.size = sizeof(uint8_t),
	.dtor = NULL,
	.cmp = &mcc_uint8_t_cmp,
	.hash = &mcc_uint8_t_hash,
};

const struct mcc_object_interface __mcc_uint16_t_obj_intf = {
	.size = sizeof(uint16_t),
	.dtor = NULL,
	.cmp = &mcc_uint16_t_cmp,
	.hash = &mcc_uint16_t_hash,
};

const struct mcc_object_interface __mcc_uint32_t_obj_intf = {
	.size = sizeof(uint32_t),
	.dtor = NULL,
	.cmp = &mcc_uint32_t_cmp,
	.hash = &mcc_uint32_t_hash,
};

const struct mcc_object_interface __mcc_uint64_t_obj_intf = {
	.size = sizeof(uint64_t),
	.dtor = NULL,
	.cmp = &mcc_uint64_t_cmp,
	.hash = &mcc_uint64_t_hash,
};

const struct mcc_object_interface __mcc_size_t_obj_intf = {
	.size = sizeof(size_t),
	.dtor = NULL,
	.cmp = &mcc_size_t_cmp,
	.hash = &mcc_size_t_hash,
};

const struct mcc_object_interface __mcc_str_obj_intf = {
	.size = sizeof(mcc_str_t),
	.dtor = NULL,
	.cmp = &mcc_str_cmp,
	.hash = &mcc_str_hash,
};
