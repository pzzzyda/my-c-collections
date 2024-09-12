#include "mcc_hash_map.h"
#include "mcc_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_key_dtor(void *self)
{
	if (*(char **)self)
		free(*(char **)self);
}

size_t str_key_hash(const void *key)
{
	const char *c = *(const char **)key;
	size_t h = 0;
	while (*c) {
		h = h * 31 + *c;
		c++;
	}
	return h;
}

bool str_key_eq(const void *a, const void *b)
{
	return strcmp(*(const char **)a, *(const char **)b) == 0;
}

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

int main()
{
	struct mcc_hash_map *h1;
	struct mcc_hash_map *h2;
	struct mcc_hash_map *h3;
	struct mcc_vector *tmp;
	struct mcc_hash_map_iter iter;
	char *k;
	int v;

	h1 = mcc_hash_map_new(MCC_OBJECT_CREATE(char *, str_key_dtor),
			      MCC_BASICS(int), str_key_hash, str_key_eq);

	mcc_hash_map_insert(h1, &(char *){strdup("Apple")}, &(int){0});
	mcc_hash_map_insert(h1, &(char *){strdup("Banana")}, &(int){1});
	mcc_hash_map_insert(h1, &(char *){strdup("Pear")}, &(int){2});
	mcc_hash_map_insert(h1, &(char *){strdup("Peach")}, &(int){3});
	mcc_hash_map_insert(h1, &(char *){strdup("Watermelon")}, &(int){4});
	mcc_hash_map_insert(h1, &(char *){strdup("Pineapple")}, &(int){5});

	mcc_hash_map_insert(h1, &(char *){"Pear"}, &(int){20});

	mcc_hash_map_iter_init(h1, &iter);
	while (iter.next(&iter, &k, &v)) {
		printf("(%s: %d)\n", k, v);
	}

	h2 = mcc_hash_map_new(MCC_OBJECT_CREATE(char *, str_key_dtor),
			      MCC_OBJECT(fruit), str_key_hash, str_key_eq);

	mcc_hash_map_insert(h2, &(char *){strdup("Apple")},
			    &(struct fruit){strdup("Apple"), 0});
	mcc_hash_map_insert(h2, &(char *){strdup("Banana")},
			    &(struct fruit){strdup("Banana"), 0});
	mcc_hash_map_insert(h2, &(char *){strdup("Pear")},
			    &(struct fruit){strdup("Pear"), 0});
	mcc_hash_map_insert(h2, &(char *){strdup("Peach")},
			    &(struct fruit){strdup("Peach"), 0});
	mcc_hash_map_insert(h2, &(char *){strdup("Watermelon")},
			    &(struct fruit){strdup("Watermelon"), 0});
	mcc_hash_map_insert(h2, &(char *){strdup("Pineapple")},
			    &(struct fruit){strdup("Pineapple"), 0});

	mcc_hash_map_iter_init(h2, &iter);
	for (struct fruit f; iter.next(&iter, &k, &f);) {
		printf("(%s, {%s, %d})\n", k, f.name, f.color);
	}

	h3 = mcc_hash_map_new(MCC_OBJECT_CREATE(char *, str_key_dtor),
			      MCC_VECTOR, str_key_hash, str_key_eq);

	tmp = mcc_vector_new(MCC_BASICS(int));
	mcc_hash_map_insert(h3, &(char *){strdup("Apple")}, &tmp);
	tmp = mcc_vector_new(MCC_BASICS(int));
	mcc_hash_map_insert(h3, &(char *){strdup("Banana")}, &tmp);
	tmp = mcc_vector_new(MCC_BASICS(int));
	mcc_hash_map_insert(h3, &(char *){strdup("Orange")}, &tmp);

	mcc_hash_map_iter_init(h3, &iter);
	for (; iter.next(&iter, &k, &tmp);) {
		for (int i= 0; i < 5; i++)
			mcc_vector_push(tmp, &i);
	}

	mcc_hash_map_iter_init(h3, &iter);
	for (; iter.next(&iter, &k, &tmp);) {
		puts(k);
		struct mcc_vector_iter i;
		mcc_vector_iter_init(tmp, &i);
		for (int t; i.next(&i, &t);)
			printf("%d ", t);
		putchar('\n');
	}

	mcc_hash_map_delete(h1);
	mcc_hash_map_delete(h2);
	mcc_hash_map_delete(h3);
	return 0;
}
