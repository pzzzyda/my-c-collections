#include "mcc_object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fruit {
	char *name;
	int color;
	float weight;
};

static inline void fruit_drop(struct fruit *self)
{
	printf("%s is called for %s\n", __func__, self->name);
	free(self->name);
}

static inline int fruit_cmp(const struct fruit *self, const struct fruit *other)
{
	return strcmp(self->name, other->name);
}

static inline struct fruit *fruit_new(struct fruit *self, const char *name)
{
	self->name = strdup(name);
	self->color = 10;
	self->weight = 0.5;
	return self;
}

struct mcc_object_interface fruit_ = {
	.size = sizeof(struct fruit),
	.drop = (mcc_drop_fn)&fruit_drop,
	.cmp = (mcc_compare_fn)&fruit_cmp,
	.hash = 0,
};
