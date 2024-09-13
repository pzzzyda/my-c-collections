#include "mcc_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_key_dtor(void *self)
{
	if (*(char **)self)
		free(*(char **)self);
}

int str_key_cmp(const void *a, const void *b)
{
	return strcmp(*(const char **)a, *(const char **)b);
}

int main()
{
	struct mcc_map *m1;
	struct mcc_map_iter iter;
	char *k;

	m1 = mcc_map_new(&(struct mcc_object_interface){"char *",
							sizeof(char *),
							str_key_dtor},
			 &(struct mcc_object_interface){"int", sizeof(int), 0},
			 str_key_cmp);

	mcc_map_insert(m1, &(char *){strdup("A")}, &(int){'A'});
	mcc_map_insert(m1, &(char *){strdup("E")}, &(int){'E'});
	mcc_map_insert(m1, &(char *){strdup("D")}, &(int){'D'});
	mcc_map_insert(m1, &(char *){strdup("C")}, &(int){'C'});
	mcc_map_insert(m1, &(char *){strdup("B")}, &(int){'B'});
	mcc_map_insert(m1, &(char *){strdup("F")}, &(int){'F'});
	mcc_map_insert(m1, &(char *){strdup("b")}, &(int){'b'});
	mcc_map_insert(m1, &(char *){strdup("w")}, &(int){'w'});
	mcc_map_insert(m1, &(char *){strdup("k")}, &(int){'k'});
	mcc_map_insert(m1, &(char *){strdup("Z")}, &(int){'Z'});
	mcc_map_insert(m1, &(char *){strdup("n")}, &(int){'n'});
	mcc_map_insert(m1, &(char *){strdup("O")}, &(int){'O'});
	mcc_map_insert(m1, &(char *){strdup("s")}, &(int){'s'});
	mcc_map_insert(m1, &(char *){strdup("v")}, &(int){'v'});
	mcc_map_insert(m1, &(char *){strdup("K")}, &(int){'K'});

	mcc_map_iter_init(m1, &iter);
	for (int v; iter.next(&iter, &k, &v);)
		printf("(%s, %d)\n", k, v);

	puts("===============removed 'C', 'b' 'Z'==================");

	mcc_map_remove(m1, &(char *){"C"});
	mcc_map_remove(m1, &(char *){"b"});
	mcc_map_remove(m1, &(char *){"Z"});

	mcc_map_iter_init(m1, &iter);
	for (int v; iter.next(&iter, &k, &v);)
		printf("(%s, %d)\n", k, v);

	mcc_map_delete(m1);
	return 0;
}
