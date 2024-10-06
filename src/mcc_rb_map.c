#include "mcc_rb_map.h"
#include <stdlib.h>

#define MCC_RB_RED false
#define MCC_RB_BLACK true

struct mcc_rb_node {
	bool color;
	bool small_key;
	struct mcc_rb_node *parent;
	struct mcc_rb_node *left;
	struct mcc_rb_node *right;
	void *key;
};

struct mcc_rb_map {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;
	struct mcc_rb_node *root;
	size_t len;
};

static void mcc_rb_map_dtor(void *self)
{
	mcc_rb_map_delete(*(struct mcc_rb_map **)self);
}

static int mcc_rb_map_cmp(const void *self, const void *other)
{
	struct mcc_rb_map *const *p1 = self;
	struct mcc_rb_map *const *p2 = other;

	return SIZE_T->cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_rb_map_hash(const void *self)
{
	struct mcc_rb_map *const *p = self;

	return SIZE_T->hash(&(**p).len);
}

const struct mcc_object_interface __mcc_rb_map_obj_intf = {
	.size = sizeof(struct mcc_rb_map *),
	.dtor = &mcc_rb_map_dtor,
	.cmp = &mcc_rb_map_cmp,
	.hash = &mcc_rb_map_hash,
};

static inline bool is_red(struct mcc_rb_node *node)
{
	return !node ? false : node->color == MCC_RB_RED;
}

static inline bool is_black(struct mcc_rb_node *node)
{
	return !node ? true : node->color == MCC_RB_BLACK;
}

static inline bool is_root(struct mcc_rb_node *node)
{
	return !node ? false : node->parent == NULL;
}

static inline void change_color(struct mcc_rb_node *node)
{
	node->color = !node->color;
}

static void rotate_left(struct mcc_rb_node **root, struct mcc_rb_node *x)
{
	/*
	 *  X->parent       X->parent
	 *    |                 |
	 *    X                 Y
	 *   / \      -->      / \
	 *  ?   Y             X   ?
	 *     / \           / \
	 *    Yl  ?         ?  Yl
	 */
	struct mcc_rb_node *y = x->right;
	if (y) {
		x->right = y->left;
		if (y->left)
			y->left->parent = x;
		y->parent = x->parent;
		if (!x->parent)
			*root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}
}

static void rotate_right(struct mcc_rb_node **root, struct mcc_rb_node *x)
{
	/*
	 *    X->parent       X->parent
	 *      |                 |
	 *      X                 Y
	 *     / \      -->      / \
	 *    Y   ?             ?   X
	 *   / \                   / \
	 *  ?  Yr                 Yr  ?
	 */
	struct mcc_rb_node *y = x->left;
	if (y) {
		x->left = y->right;
		if (y->right)
			y->right->parent = x;
		y->parent = x->parent;
		if (!x->parent)
			*root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->right = x;
		x->parent = y;
	}
}

static void fix_insert(struct mcc_rb_node **root, struct mcc_rb_node *insertion)
{
	struct mcc_rb_node *parent, *grandparent, *tmp;

	while (true) {
		/*
		 * If the insertion node is the root node, simply change the
		 * color of the insertion node to black and then end the loop.
		 */
		if (is_root(insertion)) {
			insertion->color = MCC_RB_BLACK;
			break;
		}

		parent = insertion->parent;

		/*
		 * If the parent node of the insertion node is black, end the
		 * loop directly.
		 */
		if (is_black(parent))
			break;

		grandparent = parent->parent;
		tmp = grandparent->left;
		if (parent != tmp) { /* parent == grandparent->right */
			if (is_red(tmp)) {
				/*
				 * Case 1: If the uncle node is red, the parent
				 * and uncle nodes change color, the grandfather
				 * node becomes the new insertion node, and then
				 * continue.
				 *
				 *    g(B)            g(R) <- i
				 *    /  \            /  \
				 *  u(R)  p(R)  -->  u(B) p(B)
				 *         \               \
				 *          i(R)           (R)
				 */
				parent->color = MCC_RB_BLACK;
				tmp->color = MCC_RB_BLACK;
				grandparent->color = MCC_RB_RED;
				insertion = grandparent;
				continue;
			}

			tmp = parent->right;
			if (insertion != tmp) { /* node == parent->left */
				/*
				 * Case 2: The uncle node is black and the
				 * inserted node is the left child of the parent
				 * node.
				 *
				 *     g(B)             g(B)
				 *     /  \             /  \
				 *    u(B) p(R)  -->   u(B) i(R)
				 *   /                  \
				 *  i(R)                 p(R)
				 *
				 * Rotate right at the parent node. But it
				 * hasn't been fixed yet, and it will eventually
				 * be fixed in Case 3.
				 */
				rotate_right(root, parent);
			}

			/*
			 * Case 3: The uncle node is black and the inserted node
			 * is the right child of the parent node.
			 *
			 *   g(B)             p(B)
			 *   /  \             /  \
			 *  u(B) p(R)  -->   g(R) i(R)
			 *        \         /
			 *         i(R)    u(B)
			 */
			grandparent->right->color = MCC_RB_BLACK;
			grandparent->color = MCC_RB_RED;
			rotate_left(root, grandparent);
			break;
		} else { /* parent == grandparent->left */
			tmp = grandparent->right;
			if (is_red(tmp)) {
				/* Case 1: The color of uncle node is red. */
				parent->color = MCC_RB_BLACK;
				tmp->color = MCC_RB_BLACK;
				grandparent->color = MCC_RB_RED;
				insertion = grandparent;
				continue;
			}
			tmp = parent->left;
			if (insertion != tmp) { /* node == parent->right */
				/*
				 * Case 2: The uncle node is black and the
				 * inserted node is the right child of the
				 * parent node.
				 */
				rotate_left(root, parent);
			}
			/*
			 * Case 3: The uncle node is black and the inserted node
			 * is the left child of the parent node.
			 */
			grandparent->left->color = MCC_RB_BLACK;
			grandparent->color = MCC_RB_RED;
			rotate_right(root, grandparent);
			break;
		}
	}
}

static void fix_remove(struct mcc_rb_node **root,
		       struct mcc_rb_node *double_black)
{
	struct mcc_rb_node *parent, *sibling;

	while (true) {
		/*
		 * If the double black node is the root node, the loop ends
		 * directly.
		 */
		if (is_root(double_black))
			break;

		/*
		 * If the color of the double black node is red, the loop ends
		 * directly.
		 */
		if (is_red(double_black)) {
			double_black->color = MCC_RB_BLACK;
			break;
		}

		parent = double_black->parent;

		if (double_black == parent->right) {
			sibling = parent->left;

			if (is_red(sibling)) {
				/*
				 * Case 1: The double black node is the right
				 * node, and the color of the sibling node is
				 * red.
				 *
				 *      p(B)            s(B)
				 *      /  \            /  \
				 *    s(R) db   -->   l(B) p(R)
				 *    /  \                 /  \
				 *   l(B) r(B)            r(B) db
				 *
				 * Rotate right at the parent node and
				 * continue.
				 */
				change_color(parent);
				change_color(sibling);
				rotate_right(root, parent);
				continue;
			}

			/*
			 * Case 2: The sibling node is black and its left and
			 * right child are both black.
			 *
			 *       p              p <-db
			 *      / \    -->     / \
			 *    s(B) db        s(R) #
			 *    /  \           /  \
			 *  (B)  (B)       (B)  (B)
			 *
			 * Change the color of the sibling node to red, then the
			 * parent node becomes the new double black node, and
			 * continue.
			 */
			if (is_black(sibling->left) &&
			    is_black(sibling->right)) {
				sibling->color = MCC_RB_RED;
				double_black = parent;
				parent = double_black->parent;
				continue;
			}

			/*
			 * Here, we've fixed the double black after making the
			 * corresponding rotation operation.
			 */
			if (is_red(sibling->left)) {
				/*
				 * Case 3: The sibling node is black and its
				 * left child is red.
				 *
				 *     p(?)             s(?)
				 *     /  \             /  \
				 *   s(B)  db   -->    l(B) p(B)
				 *   /                       \
				 * l(R)                       db
				 *
				 * It doesn't matter if the right child of the
				 * sibling node is red or not.
				 */
				sibling->left->color = sibling->color;
				sibling->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rotate_right(root, parent);
			} else {
				/*
				 * Case 4: The sibling node is black and its
				 * right child is red.
				 *
				 *     p(?)             p(?)          r(?)
				 *     /  \             /  \          /  \
				 *    s(B) db   -->   r(R) db  -->  s(B) p(B)
				 *     \             /                     \
				 *      r(R)        s(b)                    db
				 */
				sibling->right->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rotate_left(root, sibling);
				rotate_right(root, parent);
			}
			break;
		} else { /* double_black == parent->left */
			sibling = parent->right;
			if (is_red(sibling)) {
				/* Case 1: The sibling node is red. */
				change_color(parent);
				change_color(sibling);
				rotate_left(root, parent);
				continue;
			}
			/*
			 * Case 2: The sibling node is black and its left and
			 * right child are both black.
			 */
			if (is_black(sibling->left) &&
			    is_black(sibling->right)) {
				sibling->color = MCC_RB_RED;
				double_black = parent;
				parent = double_black->parent;
				continue;
			}
			if (is_red(sibling->right)) {
				/*
				 * Case 3: The sibling node is black and its
				 * right child is red
				 */
				sibling->right->color = sibling->color;
				sibling->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rotate_left(root, parent);
			} else {
				/*
				 * Case 4: The sibling node is black and its
				 * left child is red.
				 */
				sibling->left->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rotate_right(root, sibling);
				rotate_left(root, parent);
			}
			break;
		}
	}
}

static inline void *key_of(struct mcc_rb_node *node)
{
	return node->small_key ? &node->key : node->key;
}

static inline void *val_of(struct mcc_rb_node *node)
{
	return (uint8_t *)node + sizeof(struct mcc_rb_node);
}

static struct mcc_rb_node *create_node(const void *key, size_t key_size,
				       const void *val, size_t val_size)
{
	struct mcc_rb_node *node;

	node = calloc(1, sizeof(struct mcc_rb_node) + val_size);
	if (!node)
		return NULL;

	if (key_size <= sizeof(void *)) {
		node->small_key = true;
	} else {
		node->small_key = false;
		node->key = calloc(1, key_size);
		if (!node->key) {
			free(node);
			return NULL;
		}
	}

	memcpy(key_of(node), key, key_size);
	memcpy(val_of(node), val, val_size);

	return node;
}

static void destroy_node(struct mcc_rb_node *node,
			 const mcc_destruct_fn key_dtor,
			 const mcc_destruct_fn val_dtor,
			 const bool is_recursive)
{
	if (!node)
		return;

	if (is_recursive) {
		destroy_node(node->left, key_dtor, val_dtor, is_recursive);
		destroy_node(node->right, key_dtor, val_dtor, is_recursive);
	}

	if (key_dtor)
		key_dtor(key_of(node));

	if (val_dtor)
		val_dtor(val_of(node));

	if (!node->small_key)
		free(key_of(node));

	free(node);
}

static void inline swap_key_value(struct mcc_rb_node *a, struct mcc_rb_node *b,
				  size_t val_size)
{
	void *tmp = a->key;
	a->key = b->key;
	b->key = tmp;
	if (val_size > 0)
		mcc_memswap(val_of(a), val_of(b), val_size);
}

static struct mcc_rb_node **find(struct mcc_rb_node **node, const void *key,
				 mcc_compare_fn cmp,
				 struct mcc_rb_node **parent)
{
	int comparison;

	*parent = NULL;
	while (*node) {
		*parent = *node;
		comparison = cmp(key, key_of(*node));
		if (comparison > 0)
			node = &(*node)->right;
		else if (comparison < 0)
			node = &(*node)->left;
		else
			break;
	}

	return node;
}

struct mcc_rb_map *mcc_rb_map_new(const struct mcc_object_interface *K,
				  const struct mcc_object_interface *V)
{
	struct mcc_rb_map *self;

	if (!K || !V)
		return NULL;

	self = calloc(1, sizeof(struct mcc_rb_map));
	if (!self)
		return NULL;

	self->K = K;
	self->V = V;
	return self;
}

void mcc_rb_map_delete(struct mcc_rb_map *self)
{
	if (!self)
		return;

	mcc_rb_map_clear(self);
	free(self);
}

void mcc_rb_map_clear(struct mcc_rb_map *self)
{
	if (!self)
		return;

	destroy_node(self->root, self->K->dtor, self->V->dtor, true);
	self->root = NULL;
	self->len = 0;
}

mcc_err_t mcc_rb_map_insert(struct mcc_rb_map *self, const void *key,
			    const void *value)
{
	struct mcc_rb_node **node, *parent;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	node = find(&self->root, key, self->K->cmp, &parent);

	if (*node != NULL) { /* The key already exists. */
		/* Update the value. */
		if (self->V->dtor)
			self->V->dtor(val_of(*node));
		memcpy(val_of(*node), value, self->V->size);
	} else { /* Insert a new node. */
		*node = create_node(key, self->K->size, value, self->V->size);
		if (!*node)
			return CANNOT_ALLOCATE_MEMORY;
		(*node)->parent = parent;
		fix_insert(&self->root, *node);
		self->len++;
	}
	return OK;
}

void mcc_rb_map_remove(struct mcc_rb_map *self, const void *key)
{
	struct mcc_rb_node **node, *tmp;

	if (!self || !key)
		return;

	node = find(&self->root, key, self->K->cmp,
		    &(struct mcc_rb_node *){NULL});
	if (!*node) /* The key does not exist. */
		return;

	/*
	 * If the deleting node has two children, find its successor, swap their
	 * key-value pairs, and make the successor the deleting node.
	 */
	if ((*node)->left && (*node)->right) {
		tmp = *node;
		node = &(*node)->right;
		while ((*node)->left)
			node = &(*node)->left;
		swap_key_value(*node, tmp, self->V->size);
	}

	tmp = *node;

	/*
	 * If the node has only one child, the color of the child node must be
	 * red, and the color of the node must be black.
	 */
	if ((*node)->left || (*node)->right) {
		/*
		 * Directly replace with the child node, and then change the
		 * color of the child node to black.
		 */
		*node = (*node)->left ? (*node)->left : (*node)->right;
		(*node)->color = MCC_RB_BLACK;
		(*node)->parent = tmp->parent;
	} else { /* The node does not have children. */
		if ((*node)->color == MCC_RB_BLACK) {
			/* The deleted node becomes a double-black node.*/
			fix_remove(&self->root, *node);
		}
		*node = NULL;
	}

	destroy_node(tmp, self->K->dtor, self->V->dtor, false);
	self->len -= 1;
}

mcc_err_t mcc_rb_map_get(struct mcc_rb_map *self, const void *key, void *value)
{
	struct mcc_rb_node **node;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	node = find(&self->root, key, self->K->cmp, &(struct mcc_rb_node *){0});

	if (!*node) {
		return NONE;
	} else {
		memcpy(value, val_of(*node), self->V->size);
		return OK;
	}
}

mcc_err_t mcc_rb_map_get_key_value(struct mcc_rb_map *self, const void *key,
				   struct mcc_kv_pair *pair)
{
	struct mcc_rb_node **node;

	if (!self || !key || !pair)
		return INVALID_ARGUMENTS;

	node = find(&self->root, key, self->K->cmp, &(struct mcc_rb_node *){0});

	if (!*node) {
		return NONE;
	} else {
		memcpy(pair->key, key_of(*node), self->K->size);
		memcpy(pair->value, val_of(*node), self->V->size);
		return OK;
	}
}

void *mcc_rb_map_get_ptr(struct mcc_rb_map *self, const void *key)
{
	struct mcc_rb_node **node;

	if (!self || !key)
		return NULL;

	node = find(&self->root, key, self->K->cmp, &(struct mcc_rb_node *){0});

	if (!*node)
		return NULL;
	else
		return val_of(*node);
}

size_t mcc_rb_map_len(struct mcc_rb_map *self)
{
	return !self ? 0 : self->len;
}

bool mcc_rb_map_is_empty(struct mcc_rb_map *self)
{
	return !self ? true : self->len == 0;
}

static const struct mcc_iterator_interface mcc_rb_map_iter_intf = {
	.next = (mcc_iterator_next_fn)&mcc_rb_map_iter_next,
};

mcc_err_t mcc_rb_map_iter_init(struct mcc_rb_map *self,
			       struct mcc_rb_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->base.iter_intf = &mcc_rb_map_iter_intf;
	iter->curr = self->root;
	while (iter->curr && iter->curr->left)
		iter->curr = iter->curr->left;
	iter->container = self;
	return OK;
}

bool mcc_rb_map_iter_next(struct mcc_rb_map_iter *self,
			  struct mcc_kv_pair *result)
{
	struct mcc_rb_node *curr, *tmp;
	mcc_compare_fn cmp;

	if (!self || !result)
		return false;

	if (!self->curr)
		return false;

	curr = self->curr;
	memcpy(result->key, key_of(curr), self->container->K->size);
	memcpy(result->value, val_of(curr), self->container->V->size);

	if (curr->right) {
		/* Find the successor. */
		tmp = curr->right;
		while (tmp->left)
			tmp = tmp->left;
		self->curr = tmp;
	} else {
		tmp = curr->parent;
		cmp = self->container->K->cmp;

		while (tmp && cmp(key_of(tmp), key_of(curr)) <= 0)
			tmp = tmp->parent;
		self->curr = tmp;
	}
	return true;
}

#define _UNIT_TEST 0
#if _UNIT_TEST
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int black_height(struct mcc_rb_node *node)
{
	int left_height;
	int right_height;

	if (node == NULL)
		return 1;

	left_height = black_height(node->left);
	right_height = black_height(node->right);

	if (left_height != right_height)
		return -1;

	if (node->color == MCC_RB_BLACK)
		return left_height + 1;
	else
		return left_height;
}

static bool check_red_black_property(struct mcc_rb_node *node)
{
	if (node == NULL)
		return true;

	if (is_red(node) && (is_red(node->left) || is_red(node->right)))
		return false;

	return check_red_black_property(node->left) &&
	       check_red_black_property(node->right);
}

static bool is_valid_red_black_tree(struct mcc_rb_node *root)
{
	int height;

	if (!is_black(root))
		return false;

	height = black_height(root);
	if (height == -1)
		return false;

	if (!check_red_black_property(root))
		return false;

	return true;
}

static void print_tree(struct mcc_rb_node *node)
{
	if (!node)
		return;

	print_tree(node->left);
	printf("%d ", *(int *)(key_of(node)));
	print_tree(node->right);
}

static struct mcc_object_interface none = {
	.size = 0,
	.dtor = NULL,
	.cmp = NULL,
	.hash = NULL,
};

static void test_insert()
{
	enum { LEN = 100000, RANGE = LEN * 10 };
	struct mcc_rb_map *map = mcc_rb_map_new(INT, &none);
	assert(map != NULL);

	srand(time(NULL));
	for (size_t i = 0; i < LEN; i++)
		mcc_rb_map_insert(map, &(int){rand() % RANGE}, &(char){0});
	print_tree(map->root);
	putchar('\n');

	if (!is_valid_red_black_tree(map->root))
		puts("This tree is not a valid red-black tree.");
	else
		puts("This tree is a valid red-black tree.");

	mcc_rb_map_delete(map);
}

static void test_remove()
{
	enum { LEN = 100000, RANGE = LEN * 10 };
	struct mcc_rb_map *map = mcc_rb_map_new(INT, &none);
	assert(map != NULL);
	int *data = malloc(LEN * sizeof(int));
	assert(data != NULL);

	srand(time(NULL));
	for (size_t i = 0; i < LEN; i++) {
		data[i] = rand() % RANGE;
		mcc_rb_map_insert(map, &data[i], &(char){0});
	}

	for (size_t i = 0; i < LEN; i++)
		mcc_rb_map_remove(map, &data[i]);

	mcc_rb_map_delete(map);
	free(data);
}

int main(void)
{
	test_insert();
	test_remove();
	return 0;
}

#endif /* _UNIT_TEST */
