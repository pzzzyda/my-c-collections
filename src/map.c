#include "mcc_err.h"
#include "mcc_map.h"
#include "memswap.h"
#include <stdlib.h>

enum { RED, BLACK };

struct mcc_rb_node {
	int color;
	struct mcc_rb_node *parent;
	struct mcc_rb_node *left;
	struct mcc_rb_node *right;
	struct mcc_pair pair;
};

struct mcc_map_iter {
	struct mcc_map_iter *next;
	struct mcc_map *map;
	struct mcc_rb_node *curr;
	bool in_use;
};

struct mcc_map {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;
	struct mcc_map_iter *iters;
	struct mcc_rb_node *root;
	size_t len;
};

static inline bool is_red(struct mcc_rb_node *node)
{
	return !node ? false : node->color == RED;
}

static inline bool is_black(struct mcc_rb_node *node)
{
	return !node ? true : node->color == BLACK;
}

static inline bool is_root(struct mcc_rb_node *node)
{
	return !node ? false : node->parent == NULL;
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

static void fix_insert(struct mcc_rb_node **root, struct mcc_rb_node *node)
{
	struct mcc_rb_node *parent, *grandparent, *tmp;

	while (true) {
		/*
		 * If the insertion node is the root node, simply change the
		 * color of the insertion node to black and then end the loop.
		 */
		if (is_root(node)) {
			node->color = BLACK;
			break;
		}

		parent = node->parent;

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
				parent->color = BLACK;
				tmp->color = BLACK;
				grandparent->color = RED;
				node = grandparent;
				continue;
			}

			tmp = parent->right;
			if (node != tmp) { /* node == parent->left */
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
			grandparent->right->color = BLACK;
			grandparent->color = RED;
			rotate_left(root, grandparent);
			break;
		} else { /* parent == grandparent->left */
			tmp = grandparent->right;
			if (is_red(tmp)) {
				/* Case 1: The color of uncle node is red. */
				parent->color = BLACK;
				tmp->color = BLACK;
				grandparent->color = RED;
				node = grandparent;
				continue;
			}
			tmp = parent->left;
			if (node != tmp) { /* node == parent->right */
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
			grandparent->left->color = BLACK;
			grandparent->color = RED;
			rotate_right(root, grandparent);
			break;
		}
	}
}

static void fix_remove(struct mcc_rb_node **root, struct mcc_rb_node *parent)
{
	struct mcc_rb_node *sibling;
	struct mcc_rb_node *db = NULL;

	if (!parent)
		return;

	while (true) {
		/*
		 * If the double black node is the root node, the loop ends
		 * directly.
		 */
		if (is_root(db))
			break;

		/*
		 * If the color of the double black node is red, the loop ends
		 * directly.
		 */
		if (is_red(db)) {
			db->color = BLACK;
			break;
		}

		if (db == parent->right) {
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
				parent->color = RED;
				sibling->color = BLACK;
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
				sibling->color = RED;
				db = parent;
				parent = db->parent;
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
				 * l(R)                       # -> db(X)
				 *
				 * It doesn't matter if the right child of the
				 * sibling node is red or not.
				 */
				sibling->left->color = sibling->color;
				sibling->color = parent->color;
				parent->color = BLACK;
				rotate_right(root, parent);
			} else {
				/*
				 * Case 4: The sibling node is black and its
				 * right child is red.
				 *
				 *     p(?)             r(?)
				 *     /  \             /  \
				 *    s(B) db   -->    s(B) p(B)
				 *     \                     \
				 *      r(R)                  # -> db(X)
				 */
				sibling->right->color = parent->color;
				parent->color = BLACK;
				rotate_left(root, sibling);
				rotate_right(root, parent);
			}
			break;
		} else { /* db == parent->left */
			sibling = parent->right;
			if (is_red(sibling)) {
				/* Case 1: The sibling node is red. */
				parent->color = RED;
				sibling->color = BLACK;
				rotate_left(root, parent);
				continue;
			}
			/*
			 * Case 2: The sibling node is black and its left and
			 * right child are both black.
			 */
			if (is_black(sibling->left) &&
			    is_black(sibling->right)) {
				sibling->color = RED;
				db = parent;
				parent = db->parent;
				continue;
			}
			if (is_red(sibling->right)) {
				/*
				 * Case 3: The sibling node is black and its
				 * right child is red
				 */
				sibling->right->color = sibling->color;
				sibling->color = parent->color;
				parent->color = BLACK;
				rotate_left(root, parent);
			} else {
				/*
				 * Case 4: The sibling node is black and its
				 * left child is red.
				 */
				sibling->left->color = parent->color;
				parent->color = BLACK;
				rotate_right(root, sibling);
				rotate_left(root, parent);
			}
			break;
		}
	}
}

static inline void *data_addr(struct mcc_rb_node *node)
{
	return (uint8_t *)node + sizeof(struct mcc_rb_node);
}

static inline const void *key_of(struct mcc_rb_node *node)
{
	return node->pair.key;
}

static inline void *value_of(struct mcc_rb_node *node)
{
	return node->pair.value;
}

static struct mcc_rb_node *create_node(const void *key, size_t key_size,
				       const void *val, size_t val_size)
{
	struct mcc_rb_node *node;
	uint8_t *ptr;
	size_t total_size = 0;

	total_size += sizeof(struct mcc_rb_node);
	total_size += key_size;
	total_size += val_size;
	node = calloc(1, total_size);
	if (!node)
		return NULL;

	ptr = (uint8_t *)node + sizeof(struct mcc_rb_node);
	memcpy(ptr, key, key_size);
	node->pair.key = ptr;

	ptr += key_size;
	memcpy(ptr, val, val_size);
	node->pair.value = ptr;

	return node;
}

static void destroy_node(struct mcc_rb_node *node, const mcc_drop_fn key_drop,
			 const mcc_drop_fn val_drop, const bool is_recursive)
{
	if (!node)
		return;

	if (is_recursive) {
		destroy_node(node->left, key_drop, val_drop, is_recursive);
		destroy_node(node->right, key_drop, val_drop, is_recursive);
	}

	if (key_drop)
		key_drop(data_addr(node));

	if (val_drop)
		val_drop(value_of(node));

	free(node);
}

static struct mcc_rb_node **get_node(struct mcc_map *self, const void *key,
				     struct mcc_rb_node **parent)
{
	int cmp_res;
	struct mcc_rb_node *node_parent = NULL;
	struct mcc_rb_node **node = &(self->root);

	while (*node) {
		cmp_res = self->K->cmp(key, key_of(*node));
		if (cmp_res > 0) {
			node_parent = *node;
			node = &((*node)->right);
		} else if (cmp_res < 0) {
			node_parent = *node;
			node = &((*node)->left);
		} else {
			break;
		}
	}

	if (parent)
		*parent = node_parent;

	return node;
}

struct mcc_map *mcc_map_new(const struct mcc_object_interface *K,
			    const struct mcc_object_interface *V)
{
	struct mcc_map *self;

	if (!K || !V)
		return NULL;

	self = calloc(1, sizeof(struct mcc_map));
	if (!self)
		return NULL;

	self->K = K;
	self->V = V;
	return self;
}

void mcc_map_drop(struct mcc_map *self)
{
	struct mcc_map_iter *tmp;

	if (!self)
		return;

	mcc_map_clear(self);

	while (self->iters) {
		tmp = self->iters;
		self->iters = self->iters->next;
		free(tmp);
	}

	free(self);
}

void mcc_map_clear(struct mcc_map *self)
{
	if (!self)
		return;

	destroy_node(self->root, self->K->drop, self->V->drop, true);
	self->root = NULL;
	self->len = 0;
}

int mcc_map_insert(struct mcc_map *self, const void *key, const void *value)
{
	struct mcc_rb_node **node, *parent;

	if (!self || !key || !value)
		return INVALID_ARGUMENTS;

	node = get_node(self, key, &parent);
	if (*node) { /* Update the value. */
		if (!self->V->size)
			return OK;

		if (self->V->drop)
			self->V->drop(value_of(*node));
		memcpy(value_of(*node), value, self->V->size);
		return OK;
	} else { /* Insert a new node. */
		*node = create_node(key, self->K->size, value, self->V->size);
		if (!*node)
			return CANNOT_ALLOCATE_MEMORY;

		(*node)->parent = parent;
		fix_insert(&self->root, *node);
		self->len++;
		return OK;
	}
}

static inline void swap_key_value(struct mcc_rb_node *a, struct mcc_rb_node *b,
				  size_t key_size, size_t val_size)
{
	memswap(data_addr(a), data_addr(b), key_size + val_size);
}

void mcc_map_remove(struct mcc_map *self, const void *key)
{
	struct mcc_rb_node **node, *tmp;

	if (!self || !key)
		return;

	node = get_node(self, key, NULL);
	if (!*node)
		return;

	if ((*node)->left && (*node)->right) {
		tmp = *node;
		node = &((*node)->right);
		while ((*node)->left)
			node = &((*node)->left);

		swap_key_value(tmp, *node, self->K->size, self->V->size);
	}

	tmp = *node;

	if ((*node)->left || (*node)->right) {
		*node = (*node)->left ? (*node)->left : (*node)->right;
		(*node)->color = BLACK;
		(*node)->parent = tmp->parent;
	} else {
		*node = NULL;
		if (tmp->color == BLACK)
			fix_remove(&(self->root), tmp->parent);
	}

	destroy_node(tmp, self->K->drop, self->V->drop, false);
	self->len--;
}

int mcc_map_get(struct mcc_map *self, const void *key, void **ref)
{
	struct mcc_rb_node **node;

	if (!self || !key || !ref)
		return INVALID_ARGUMENTS;

	node = get_node(self, key, NULL);
	if (*node) {
		*ref = value_of(*node);
		return OK;
	} else {
		return NONE;
	}
}

int mcc_map_get_key_value(struct mcc_map *self, const void *key,
			  struct mcc_pair **ref)
{
	struct mcc_rb_node **node;

	if (!self || !key || !ref)
		return INVALID_ARGUMENTS;

	node = get_node(self, key, NULL);
	if (*node) {
		*ref = &((*node)->pair);
		return OK;
	} else {
		return NONE;
	}
}

size_t mcc_map_len(struct mcc_map *self)
{
	return !self ? 0 : self->len;
}

bool mcc_map_is_empty(struct mcc_map *self)
{
	return !self ? true : self->len == 0;
}

struct mcc_map_iter *mcc_map_iter_new(struct mcc_map *map)
{
	struct mcc_map_iter *self;

	if (!map)
		return NULL;

	self = map->iters;
	while (self) {
		if (!self->in_use)
			goto reset_iterator;
		self = self->next;
	}

	self = calloc(1, sizeof(struct mcc_map_iter));
	if (!self)
		return NULL;

	self->next = map->iters;
	map->iters = self;
	self->map = map;
reset_iterator:
	self->curr = map->root;
	while (self->curr && self->curr->left)
		self->curr = self->curr->left;
	self->in_use = true;
	return self;
}

void mcc_map_iter_drop(struct mcc_map_iter *self)
{
	if (self)
		self->in_use = false;
}

bool mcc_map_iter_next(struct mcc_map_iter *self, struct mcc_pair **result)
{
	struct mcc_rb_node *tmp;

	if (!self || !result || !self->curr)
		return false;

	*result = &self->curr->pair;

	if (self->curr->right) {
		/* Find the successor. */
		tmp = self->curr->right;
		while (tmp->left)
			tmp = tmp->left;
		self->curr = tmp;
	} else {
		tmp = self->curr->parent;

		while (tmp &&
		       self->map->K->cmp(key_of(tmp), key_of(self->curr)) <= 0)
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

	if (node->color == BLACK)
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

static struct mcc_object_interface none = {
	.size = 0,
	.drop = NULL,
	.cmp = NULL,
	.hash = NULL,
};

static int compare_integer(const int *self, const int *other)
{
	return *self - *other;
}

static struct mcc_object_interface integer = {
	.size = sizeof(int),
	.drop = 0,
	.cmp = (mcc_compare_fn)&compare_integer,
	.hash = 0,
};

static void test_insert()
{
	enum { LEN = 100000, RANGE = LEN * 10 };
	struct mcc_map *map = mcc_map_new(&integer, &none);
	assert(map != NULL);

	srand(time(NULL));
	for (size_t i = 0; i < LEN; i++) {
		int r = rand() % RANGE;
		int n = 0;
		assert(!mcc_map_insert(map, &r, &n));
	}

	if (!is_valid_red_black_tree(map->root))
		puts("This tree is not a valid red-black tree.");
	else
		puts("This tree is a valid red-black tree.");

	mcc_map_drop(map);
}

static void test_remove()
{
	enum { LEN = 100000, RANGE = LEN * 10 };
	struct mcc_map *map = mcc_map_new(&integer, &none);
	assert(map != NULL);
	int *data = malloc(LEN * sizeof(int));
	assert(data != NULL);

	srand(time(NULL));
	for (size_t i = 0; i < LEN; i++) {
		data[i] = rand() % RANGE;
		assert(!mcc_map_insert(map, &data[i], &data[i]));
	}

	for (size_t i = 0; i < LEN; i++)
		mcc_map_remove(map, &data[i]);

	mcc_map_drop(map);
	free(data);
}

int main(void)
{
	test_insert();
	test_remove();
	puts("testing done");
	return 0;
}

#endif /* _UNIT_TEST */
