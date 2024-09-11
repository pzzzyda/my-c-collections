#include "mcc_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fruit {
	char *name;
	int color;
};

MCC_DESTRUCTOR(fruit, {
	if (self->name) {
		printf("~destruct: %s\n", self->name);
		free(self->name);
	}
})

int main(void)
{
	struct mcc_vector *v1;
	struct mcc_vector *v2;
	struct mcc_vector *v3;
	struct mcc_vector_iter iter;

	v1 = mcc_vector_new(MCC_BASICS(int));

	mcc_vector_push(v1, &(int){0});
	mcc_vector_push(v1, &(int){1});
	mcc_vector_push(v1, &(int){2});
	mcc_vector_push(v1, &(int){3});
	mcc_vector_push(v1, &(int){4});
	mcc_vector_push(v1, &(int){5});
	mcc_vector_push(v1, &(int){6});
	mcc_vector_push(v1, &(int){7});
	mcc_vector_pop(v1);
	mcc_vector_insert(v1, 2, &(int){9});
	mcc_vector_insert(v1, 5, &(int){9});
	mcc_vector_reverse(v1);

	mcc_vector_iter_init(v1, &iter);
	for (int i; iter.next(&iter, &i);)
		printf("%d ", i);
	putchar('\n');

	v2 = mcc_vector_new(MCC_OBJECT(fruit));

	mcc_vector_push(v2, &(struct fruit){strdup("A"), 0});
	mcc_vector_push(v2, &(struct fruit){strdup("B"), 0});
	mcc_vector_push(v2, &(struct fruit){strdup("C"), 0});
	mcc_vector_push(v2, &(struct fruit){strdup("D"), 0});

	mcc_vector_iter_init(v2, &iter);
	for (struct fruit f; iter.next(&iter, &f);)
		printf("name: %s, color: %d\n", f.name, f.color);

	v3 = mcc_vector_new(MCC_VECTOR);

	for (size_t i = 0; i < 5; i++) {
		struct mcc_vector *tmp;
		tmp = mcc_vector_new(MCC_BASICS(int));
		mcc_vector_push(v3, &tmp);
	}

	mcc_vector_iter_init(v3, &iter);
	for (struct mcc_vector *tmp; iter.next(&iter, &tmp);) {
		mcc_vector_push(tmp, &(int){0});
		mcc_vector_push(tmp, &(int){1});
		mcc_vector_push(tmp, &(int){2});
		mcc_vector_push(tmp, &(int){3});
	}

	mcc_vector_iter_init(v3, &iter);
	for (struct mcc_vector *tmp; iter.next(&iter, &tmp);) {
		size_t len = mcc_vector_len(tmp);
		for (size_t i = 0; i < len; i++) {
			int t;
			mcc_vector_get(tmp, i, &t);
			printf("%d ", t);
		}
		putchar('\n');
	}

	mcc_vector_delete(v1);
	mcc_vector_delete(v2);
	mcc_vector_delete(v3);
	return 0;
}
