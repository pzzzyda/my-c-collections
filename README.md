# my-c-collections
my-c-collections is a simple generic container library implemented in C.
| Name | Description |
| - | - |
| `mcc_vector` | A dynamic array that scales automatically. |
| `mcc_deque` | A double-ended queue based on a growable ring buffer implementation. |
| `mcc_list` | A doubly linked list. |
| `mcc_rb_map` | An ordered map based on a red-black tree. |
| `mcc_hash_map` | A hash map. |
| `mcc_rb_set` | An ordered set based on mcc_rb_map |
| `mcc_hash_set` | A hash set based on mcc_hash_map |
### Install
```bash
sudo make install
```
### Uninstall
```bash
sudo make uninstall
```
### Example
```c
#include "mcc_vector.h"
#include <stdio.h>
#include <string.h>

struct person {
	char *name;
	int age;
	int gender;
};

int person_cmp(const void *a, const void *b)
{
	const struct person *pa = a;
	const struct person *pb = b;

	return strcmp(pa->name, pb->name);
}

struct mcc_object_interface person_i = {
	.size = sizeof(struct person),
	.dtor = NULL,
	.cmp = &person_cmp, /* Sort if needed. */
	.hash = NULL,
};

int main(void)
{
	struct mcc_vector *vec = mcc_vector_new(&person_i);
	
	if (!vec) {
		return 1;
	}

	if (mcc_vector_push(vec, &(struct person){ "Peter", 20, 0 }) != OK) {
		/* Handle error. */
	}
	mcc_vector_push(vec, &(struct person){ "Frank", 30, 0 });
	mcc_vector_push(vec, &(struct person){ "Tom", 25, 0 });

	mcc_vector_sort(vec); /* The cmp field should not be NULL */

	struct mcc_vector_iter iter;
	mcc_vector_iter_init(vec, &iter);
	for (struct person elem; mcc_vector_iter_next(&iter, &elem);) {
		printf("name: %s\n", elem.name);
		printf("age: %d\n", elem.age);
		printf("name: %d\n", elem.gender);
	}

	mcc_vector_delete(vec);

	return 0;
}

```
