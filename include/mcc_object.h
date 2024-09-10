#ifndef _MCC_OBJECT_H
#define _MCC_OBJECT_H

#include "mcc_type.h"

struct mcc_object_interface {
	size_t size;
	void (*dtor)(void *);
};

#define MCC_DESTRUCTOR(name, body)                                             \
	void name##_dtor(void *__ptr)                                          \
	{                                                                      \
		struct name *self = __ptr;                                     \
		body                                                           \
	}

#define MCC_OBJECT_CREATE(name, destructor)                                    \
	&(struct mcc_object_interface)                                         \
	{                                                                      \
		.size = sizeof(name), .dtor = destructor                       \
	}

#define MCC_OBJECT(name)                                                       \
	&(struct mcc_object_interface)                                         \
	{                                                                      \
		.size = sizeof(struct name), .dtor = name##_dtor               \
	}

#define MCC_BASICS(name)                                                       \
	&(struct mcc_object_interface)                                         \
	{                                                                      \
		.size = sizeof(name), .dtor = NULL                             \
	}

#endif /* _MCC_OBJECT_H */
