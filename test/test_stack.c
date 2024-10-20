#include "mcc_stack.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
	struct mcc_stack *s = mcc_stack_new(mcc_int());
	assert(s != NULL);
	for (int i = 0; i < 10; i++)
		assert(!mcc_stack_push(s, &i));
	while (!mcc_stack_is_empty(s)) {
		int *ref;
		assert(!mcc_stack_top(s, (void **)&ref));
		printf("%d ", *ref);
		mcc_stack_pop(s);
	}
	putchar('\n');
	mcc_stack_drop(s);
	return 0;
}
