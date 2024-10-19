#include "mcc_object.h"
#include <string.h>

/* mcc_str_t (const char *) */

static int mcc_str_cmp(const mcc_str_t *self, const mcc_str_t *other)
{
	return strcmp(*self, *other);
}

static size_t mcc_str_hash(const mcc_str_t *key)
{
	const char *c = *key;
	size_t h = 0;

	while (*c)
		h = h * 31 + *c++;

	return h;
}

static const struct mcc_object_interface mcc_str_obj_intf = {
	.size = sizeof(mcc_str_t),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_str_cmp,
	.hash = (mcc_hash_fn)&mcc_str_hash,
};

const struct mcc_object_interface *mcc_str()
{
	return &mcc_str_obj_intf;
}

/* signed char */

static int mcc_char_cmp(const signed char *self, const signed char *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_char_hash(const signed char *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_char_obj_intf = {
	.size = sizeof(signed char),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_char_cmp,
	.hash = (mcc_hash_fn)&mcc_char_hash,
};

const struct mcc_object_interface *mcc_char()
{
	return &mcc_char_obj_intf;
}

/* signed short */

static int mcc_short_cmp(const signed short *self, const signed short *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_short_hash(const signed short *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_short_obj_intf = {
	.size = sizeof(signed short),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_short_cmp,
	.hash = (mcc_hash_fn)&mcc_short_hash,
};

const struct mcc_object_interface *mcc_short()
{
	return &mcc_short_obj_intf;
}

/* signed int */

static int mcc_int_cmp(const signed int *self, const signed int *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_int_hash(const signed int *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_int_obj_intf = {
	.size = sizeof(signed int),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_int_cmp,
	.hash = (mcc_hash_fn)&mcc_int_hash,
};

const struct mcc_object_interface *mcc_int()
{
	return &mcc_int_obj_intf;
}

/* signed long */

static int mcc_long_cmp(const signed long *self, const signed long *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_long_hash(const signed long *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_long_obj_intf = {
	.size = sizeof(signed long),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_long_cmp,
	.hash = (mcc_hash_fn)&mcc_long_hash,
};

const struct mcc_object_interface *mcc_long()
{
	return &mcc_long_obj_intf;
}

/* signed long long */

static int mcc_long_long_cmp(const signed long long *self,
			     const signed long long *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_long_long_hash(const signed long long *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_long_long_obj_intf = {
	.size = sizeof(signed long long),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_long_long_cmp,
	.hash = (mcc_hash_fn)&mcc_long_long_hash,
};

const struct mcc_object_interface *mcc_long_long()
{
	return &mcc_long_long_obj_intf;
}

/* unsigned char */

static int mcc_uchar_cmp(const unsigned char *self, const unsigned char *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_uchar_hash(const unsigned char *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_uchar_obj_intf = {
	.size = sizeof(unsigned char),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_uchar_cmp,
	.hash = (mcc_hash_fn)&mcc_uchar_hash,
};

const struct mcc_object_interface *mcc_uchar()
{
	return &mcc_uchar_obj_intf;
}

/* unsigned short */

static int mcc_ushort_cmp(const unsigned short *self,
			  const unsigned short *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_ushort_hash(const unsigned short *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_ushort_obj_intf = {
	.size = sizeof(unsigned short),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_ushort_cmp,
	.hash = (mcc_hash_fn)&mcc_ushort_hash,
};

const struct mcc_object_interface *mcc_ushort()
{
	return &mcc_ushort_obj_intf;
}

/* unsigned int */

static int mcc_uint_cmp(const unsigned int *self, const unsigned int *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_uint_hash(const unsigned int *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_uint_obj_intf = {
	.size = sizeof(unsigned int),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_uint_cmp,
	.hash = (mcc_hash_fn)&mcc_uint_hash,
};

const struct mcc_object_interface *mcc_uint()
{
	return &mcc_uint_obj_intf;
}

/* unsigned long */

static int mcc_ulong_cmp(const unsigned long *self, const unsigned long *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_ulong_hash(const unsigned long *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_ulong_obj_intf = {
	.size = sizeof(unsigned long),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_ulong_cmp,
	.hash = (mcc_hash_fn)&mcc_ulong_hash,
};

const struct mcc_object_interface *mcc_ulong()
{
	return &mcc_ulong_obj_intf;
}

/* unsigned long long */

static int mcc_ulong_long_cmp(const unsigned long long *self,
			      const unsigned long long *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_ulong_long_hash(const unsigned long long *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_ulong_long_obj_intf = {
	.size = sizeof(unsigned long long),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_ulong_long_cmp,
	.hash = (mcc_hash_fn)&mcc_ulong_long_hash,
};

const struct mcc_object_interface *mcc_ulong_long()
{
	return &mcc_ulong_long_obj_intf;
}

/* float */

static int mcc_float_cmp(const float *self, const float *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_float_hash(const float *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_float_obj_intf = {
	.size = sizeof(float),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_float_cmp,
	.hash = (mcc_hash_fn)&mcc_float_hash,
};

const struct mcc_object_interface *mcc_float()
{
	return &mcc_float_obj_intf;
}

/* double */
static int mcc_double_cmp(const double *self, const double *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_double_hash(const double *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_double_obj_intf = {
	.size = sizeof(double),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_double_cmp,
	.hash = (mcc_hash_fn)&mcc_double_hash,
};

const struct mcc_object_interface *mcc_double()
{
	return &mcc_double_obj_intf;
}

/* long double */

static int mcc_long_double_cmp(const long double *self,
			       const long double *other)
{
	return *self > *other ? 1 : *self == *other ? 0 : -1;
}

static size_t mcc_long_double_hash(const long double *key)
{
	return *key;
}

static const struct mcc_object_interface mcc_long_double_obj_intf = {
	.size = sizeof(long double),
	.drop = (mcc_drop_fn)0,
	.cmp = (mcc_compare_fn)&mcc_long_double_cmp,
	.hash = (mcc_hash_fn)&mcc_long_double_hash,
};

const struct mcc_object_interface *mcc_long_double()
{
	return &mcc_long_double_obj_intf;
}
