# my-c-collections
my-c-collections is a simple generic container library implemented in C.
| Name | Description |
| - | - |
| `mcc_vector` | A dynamic array that scales automatically. |
| `mcc_deque` | A double-ended queue based on a growable ring buffer implementation. |
| `mcc_list` | A doubly linked list. |
| `mcc_map` | An ordered map based on a red-black tree. |
| `mcc_hash_map` | A hash map. |
### Install
```bash
sudo make install
```
### Uninstall
```bash
sudo make uninstall
```
## Example
### For the basic type
```c
#include "mcc_vector.h"
#include "mcc_err.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	/*
	 * mcc_i32_i is a built-in interface.
	 * You can also customize.
	 *
	 * For example:
	 *
	 * struct mcc_object_interface int_i = {
	 * 	.name = "int",
	 * 	.size = sizeof(int),
	 * 	.dtor = &destructor,
	 * 	.cmp = &compare_function,
	 * 	.hash = &hash_function,
	 * };
	 *
	 */
	struct mcc_vector *vec = mcc_vector_new(&mcc_i32_i);
	struct mcc_vector_iter iter;

	if (vec == NULL) {
		return 1;
	}

	for (int i = 0; i < 5; i++) {
		if (mcc_vector_push(vec, &i) != OK) {
			/* Handle error. */
		}
	}

	if (mcc_vector_insert(vec, 2, &(int){ 30 }) != OK) {
		/* Handle error. */
	}

	mcc_vector_iter_init(vec, &iter);
	for (int tmp; mcc_vector_iter_next(&iter, &tmp);) {
		printf("%d\n", tmp);
	}

	mcc_vector_delete(vec);
	vec = NULL;
	return 0;
}

```
### For the custom type
```c
#include "mcc_vector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct person {
	char *name;
	int age;
};

void person_dtor(void *self)
{
	free(((struct person *)self)->name);
}

int person_cmp(const void *a, const void *b)
{
	return strcmp(((struct person *)a)->name, ((struct person *)b)->name);
}

struct mcc_object_interface person_i = {
	.name = "struct person",
	.size = sizeof(struct person),
	.dtor = &person_dtor,
	.cmp = &person_cmp, /* Sort if needed. */
	.hash = NULL,
};

int main(int argc, char **argv)
{
	struct mcc_vector *vec = mcc_vector_new(&person_i);
	struct mcc_vector_iter iter;

	mcc_vector_push(vec, &(struct person){ strdup("Peter"), 0 });
	mcc_vector_push(vec, &(struct person){ strdup("Jack"), 0 });
	mcc_vector_push(vec, &(struct person){ strdup("Frank"), 0 });

	mcc_vector_iter_init(vec, &iter);
	for (struct person tmp; mcc_vector_iter_next(&iter, &tmp);) {
		printf("name: %s, age: %d\n", tmp.name, tmp.age);
	}

	mcc_vector_sort(vec);

	mcc_vector_iter_init(vec, &iter);
	for (struct person tmp; mcc_vector_iter_next(&iter, &tmp);) {
		printf("name: %s, age: %d\n", tmp.name, tmp.age);
	}

	mcc_vector_delete(vec);
	vec = NULL;
	return 0;
}

```
