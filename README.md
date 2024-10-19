# my-c-collections
my-c-collections is a simple generic container library implemented in C.
| Name | Description |
| - | - |
| `mcc_vector` | A dynamic array that scales automatically. |
| `mcc_deque` | A double-ended queue based on a growable ring buffer implementation. |
| `mcc_list` | A doubly linked list. |
| `mcc_map` | An ordered map based on red-black tree. |
| `mcc_hash_map` | A hash map. |
| `mcc_set` | An ordered set based on red-black tree. |
| `mcc_hash_set` | A hash set. |
| `mcc_priority_queue` | A priority queue implemented using a binary heap. |
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
#include "mcc_err.h"
#include "mcc_vector.h"
#include <stdio.h>
#include <string.h>

struct person {
	char *name;
	int age;
	int gender;
};

int person_cmp(const struct person *self, const struct person *other)
{
	return strcmp(self->name, self->name);
}

struct mcc_object_interface person_i = {
	.size = sizeof(struct person),
	.drop = NULL,
	.cmp = (mcc_compare_fn)&person_cmp, /* Sort if needed. */
	.hash = NULL,
};

int main(void)
{
	struct mcc_vector *vec = mcc_vector_new(&person_i);

	if (!vec) {
		return 1;
	}

	if (mcc_vector_push(vec, &(struct person){"Peter", 20, 0}) != OK) {
		/* Handle error. */
	}
	mcc_vector_push(vec, &(struct person){"Frank", 30, 0});
	mcc_vector_push(vec, &(struct person){"Tom", 25, 0});

	mcc_vector_sort(vec); /* The cmp field should not be NULL */

	struct mcc_vector_iter *iter = mcc_vector_iter_new(vec);
	if (!iter) {
		/* Handle error. */
	}

	for (struct person *p; mcc_vector_iter_next(iter, (void **)&p);) {
		printf("name: %s\n", p->name);
		printf("age: %d\n", p->age);
		printf("name: %d\n", p->gender);
	}

	/*
	 * Optional. (All iterators are automatically dropped
	 * when dropping the vector itself.)
	 */
	mcc_vector_iter_drop(iter);

	mcc_vector_drop(vec);

	return 0;
}

```
