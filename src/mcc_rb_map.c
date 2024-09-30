#include "mcc_rb_map.h"
#include "mcc_err.h"
#include <stdlib.h>

struct arg_wrapper {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;
	const void *key;
	const void *value;
};

struct mcc_rb_node {
	mcc_i32 color;
	struct mcc_rb_node *parent;
	struct mcc_rb_node *left;
	struct mcc_rb_node *right;
	void *key;
};

struct mcc_rb_map {
	const struct mcc_object_interface *K;
	const struct mcc_object_interface *V;

	struct mcc_rb_node *root;
	mcc_usize len;
};

static void mcc_rb_map_dtor(void *self)
{
	mcc_rb_map_delete(*(struct mcc_rb_map **)self);
}

static mcc_i32 mcc_rb_map_cmp(const void *self, const void *other)
{
	struct mcc_rb_map *const *p1 = self;
	struct mcc_rb_map *const *p2 = other;

	return mcc_usize_i.cmp(&(**p1).len, &(**p2).len);
}

static mcc_usize mcc_rb_map_hash(const void *self)
{
	struct mcc_rb_map *const *p = self;

	return mcc_usize_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_rb_map_i = {
	.size = sizeof(struct mcc_rb_map *),
	.dtor = &mcc_rb_map_dtor,
	.cmp = &mcc_rb_map_cmp,
	.hash = &mcc_rb_map_hash,
};

static inline mcc_bool rb_is_red(struct mcc_rb_node *node)
{
	return !node ? false : node->color == MCC_RB_RED;
}

static inline mcc_bool rb_is_black(struct mcc_rb_node *node)
{
	return !node ? true : node->color == MCC_RB_BLACK;
}

static inline mcc_bool rb_is_root(struct mcc_rb_node *node)
{
	return !node ? false : node->parent == NULL;
}

static void rb_left_rotate(struct mcc_rb_node **root, struct mcc_rb_node *x)
{
	if (!root || !*root || !x)
		return;
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

static void rb_right_rotate(struct mcc_rb_node **root, struct mcc_rb_node *x)
{
	if (!root || !*root || !x)
		return;
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

static void rb_fix_insert(struct mcc_rb_node **root, struct mcc_rb_node *node)
{
	struct mcc_rb_node *parent, *grandparent, *tmp;

	if (!root || !*root || !node)
		return;

	while (true) {
		/*
		 * If the insertion node is the root node, simply change the
		 * color of the insertion node to black and then end the loop.
		 */
		if (rb_is_root(node)) {
			node->color = MCC_RB_BLACK;
			break;
		}

		parent = node->parent;

		/*
		 * If the parent node of the insertion node is black, end the
		 * loop directly.
		 */
		if (rb_is_black(parent))
			break;

		grandparent = parent->parent;
		tmp = grandparent->left;
		if (parent != tmp) { /* parent == grandparent->right */
			if (rb_is_red(tmp)) {
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
				rb_right_rotate(root, parent);
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
			rb_left_rotate(root, grandparent);
			break;
		} else { /* parent == grandparent->left */
			tmp = grandparent->right;
			if (rb_is_red(tmp)) {
				/* Case 1: The color of uncle node is red. */
				parent->color = MCC_RB_BLACK;
				tmp->color = MCC_RB_BLACK;
				grandparent->color = MCC_RB_RED;
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
				rb_left_rotate(root, parent);
			}
			/*
			 * Case 3: The uncle node is black and the inserted node
			 * is the left child of the parent node.
			 */
			grandparent->left->color = MCC_RB_BLACK;
			grandparent->color = MCC_RB_RED;
			rb_right_rotate(root, grandparent);
			break;
		}
	}
}

static void rb_fix_remove(struct mcc_rb_node **root, struct mcc_rb_node *parent)
{
	struct mcc_rb_node *db = NULL, *sibling;

	if (!root || !*root || !parent)
		return;

	while (true) {
		/*
		 * If the double black node is the root node, the loop ends
		 * directly.
		 */
		if (rb_is_root(db))
			break;

		/*
		 * If the color of the double black node is red, the loop ends
		 * directly.
		 */
		if (rb_is_red(db)) {
			db->color = MCC_RB_BLACK;
			break;
		}

		sibling = parent->left;
		if (db != sibling) { /* db == parent->right */
			if (rb_is_red(sibling)) {
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
				parent->color = MCC_RB_RED;
				sibling->color = MCC_RB_BLACK;
				rb_right_rotate(root, parent);
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
			if (rb_is_black(sibling->left) &&
			    rb_is_black(sibling->right)) {
				sibling->color = MCC_RB_RED;
				db = parent;
				parent = db->parent;
				continue;
			}

			/*
			 * Here, we've fixed the double black after making the
			 * corresponding rotation operation.
			 */
			if (rb_is_red(sibling->left)) {
				/*
				 * Case 3: The sibling node is black and its
				 * left child is red.
				 *
				 *     p(?)             s(?)
				 *     /  \             /  \
				 *   s(B)  db   -->    r(B) p(B)
				 *   /                       \
				 * r(R)                       # -> db(X)
				 *
				 * It doesn't matter if the right child of the
				 * sibling node is red or not.
				 */
				sibling->left->color = sibling->color;
				sibling->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rb_right_rotate(root, parent);
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
				parent->color = MCC_RB_BLACK;
				rb_left_rotate(root, sibling);
				rb_right_rotate(root, parent);
			}
			break;
		} else { /* db == parent->left */
			sibling = parent->right;
			if (rb_is_red(sibling)) {
				/* Case 1: The sibling node is red. */
				parent->color = MCC_RB_RED;
				sibling->color = MCC_RB_BLACK;
				rb_left_rotate(root, parent);
				continue;
			}
			/*
			 * Case 2: The sibling node is black and its left and
			 * right child are both black.
			 */
			if (rb_is_black(sibling->left) &&
			    rb_is_black(sibling->right)) {
				sibling->color = MCC_RB_RED;
				db = parent;
				parent = db->parent;
				continue;
			}
			if (rb_is_red(sibling->right)) {
				/*
				 * Case 3: The sibling node is black and its
				 * right child is red
				 */
				sibling->right->color = sibling->color;
				sibling->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rb_left_rotate(root, parent);
			} else {
				/*
				 * Case 4: The sibling node is black and its
				 * left child is red.
				 */
				sibling->left->color = parent->color;
				parent->color = MCC_RB_BLACK;
				rb_right_rotate(root, sibling);
				rb_left_rotate(root, parent);
			}
			break;
		}
	}
}

static inline void *value_of(struct mcc_rb_node *node)
{
	return (mcc_u8 *)node + sizeof(struct mcc_rb_node);
}

static struct mcc_rb_node *rb_node_new(const struct arg_wrapper *args)
{
	struct mcc_rb_node *self;
	mcc_usize total_size = 0;

	total_size += sizeof(struct mcc_rb_node);
	total_size += args->V->size;
	self = calloc(1, total_size);
	if (!self)
		return NULL;

	self->key = calloc(1, args->K->size);
	if (!self->key) {
		free(self);
		return NULL;
	}
	memcpy(self->key, args->key, args->K->size);
	self->key = self->key;

	memcpy(value_of(self), args->value, args->V->size);

	return self;
}

static void rb_node_delete(struct mcc_rb_node *self,
			   const struct mcc_object_interface *K,
			   const struct mcc_object_interface *V,
			   const mcc_bool is_recursive)
{
	if (!self)
		return;

	if (is_recursive) {
		rb_node_delete(self->left, K, V, is_recursive);
		rb_node_delete(self->right, K, V, is_recursive);
	}

	if (K->dtor)
		K->dtor(self->key);
	free(self->key);

	if (V->dtor)
		V->dtor(value_of(self));

	free(self);
}

struct mcc_rb_map *mcc_rb_map_new(const struct mcc_object_interface *key,
				  const struct mcc_object_interface *value)
{
	struct mcc_rb_map *self;

	if (!key || !value)
		return NULL;

	self = calloc(1, sizeof(struct mcc_rb_map));
	if (!self)
		return NULL;

	self->K = key;
	self->V = value;
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

	rb_node_delete(self->root, self->K, self->V, true);
	self->root = NULL;
	self->len = 0;
}

mcc_err mcc_rb_map_insert(struct mcc_rb_map *self, const void *key,
			  const void *value)
{
	struct mcc_rb_node **curr, *parent;
	mcc_i32 cmp_res;
	const struct arg_wrapper args = {
		.K = self->K,
		.V = self->V,
		.key = key,
		.value = value,
	};

	if (!self || !value)
		return INVALID_ARGUMENTS;

	curr = &self->root;
	parent = NULL;
	while (*curr) {
		parent = *curr;
		cmp_res = self->K->cmp(key, (*curr)->key);

		if (cmp_res > 0) {
			curr = &(*curr)->right;
		} else if (cmp_res < 0) {
			curr = &(*curr)->left;
		} else { /* Update the value. */
			if (self->V->size) {
				if (self->V->dtor)
					self->V->dtor(value_of(*curr));
				memcpy(value_of(*curr), value, self->V->size);
			}
			return OK;
		}
	}

	*curr = rb_node_new(&args);
	if (!*curr)
		return CANNOT_ALLOCATE_MEMORY;

	(*curr)->parent = parent;
	self->len += 1;
	rb_fix_insert(&self->root, *curr);
	return OK;
}

void mcc_rb_map_remove(struct mcc_rb_map *self, const void *key)
{
	struct mcc_rb_node **curr, *tmp;
	mcc_i32 cmp_res;

	if (!self || !key)
		return;

	curr = &self->root;
	while (*curr) {
		cmp_res = self->K->cmp(key, (*curr)->key);
		if (cmp_res > 0)
			curr = &(*curr)->right;
		else if (cmp_res < 0)
			curr = &(*curr)->left;
		else
			break;
	}
	if (!*curr) /* No such key-value pair. */
		return;

	/*
	 * If the deleting node has two children, find its successor, swap their
	 * key-value pairs, and make the successor the deleting node.
	 */
	if ((*curr)->left && (*curr)->right) {
		tmp = *curr;
		curr = &(*curr)->right;
		while ((*curr)->left)
			curr = &(*curr)->left;
		mcc_memswap((*curr)->key, tmp->key, self->K->size);
		mcc_memswap(value_of(*curr), value_of(tmp), self->V->size);
	}

	tmp = *curr;

	/*
	 * If the node has only one child, the color of the child node must be
	 * red, and the color of the node must be black.
	 */
	if ((*curr)->left || (*curr)->right) {
		/*
		 * Directly replace with the child node, and then change the
		 * color of the child node to black.
		 */
		*curr = (*curr)->left ? (*curr)->left : (*curr)->right;
		(*curr)->color = MCC_RB_BLACK;
		(*curr)->parent = tmp->parent;
	} else { /* The node does not have children. */
		*curr = NULL;
		if (tmp->color == MCC_RB_BLACK)
			/* The deleted node becomes a double-black node.*/
			rb_fix_remove(&self->root, tmp->parent);
	}

	rb_node_delete(tmp, self->K, self->V, false);
	self->len -= 1;
}

mcc_err mcc_rb_map_get(struct mcc_rb_map *self, const void *key, void *value)
{
	void *ptr;

	ptr = mcc_rb_map_get_ptr(self, key);
	if (!ptr)
		return NONE;

	memcpy(value, ptr, self->V->size);
	return OK;
}

mcc_err mcc_rb_map_get_key_value(struct mcc_rb_map *self, const void *key,
				 void *k, void *v)
{
	struct mcc_rb_node *curr;
	mcc_i32 cmp_res;

	if (!self || !key || !k || !v)
		return INVALID_ARGUMENTS;

	curr = self->root;
	while (curr) {
		cmp_res = self->K->cmp(key, curr->key);
		if (cmp_res > 0) {
			curr = curr->right;
		} else if (cmp_res < 0) {
			curr = curr->left;
		} else {
			memcpy(k, curr->key, self->K->size);
			memcpy(v, value_of(curr), self->V->size);
			return OK;
		}
	}
	return NONE;
}

void *mcc_rb_map_get_ptr(struct mcc_rb_map *self, const void *key)
{
	struct mcc_rb_node *curr;
	mcc_i32 cmp_res;

	if (!self || !key)
		return NULL;

	curr = self->root;
	while (curr) {
		cmp_res = self->K->cmp(key, curr->key);
		if (cmp_res > 0) {
			curr = curr->right;
		} else if (cmp_res < 0) {
			curr = curr->left;
		} else {
			return value_of(curr);
		}
	}
	return NULL;
}

mcc_usize mcc_rb_map_len(struct mcc_rb_map *self)
{
	return !self ? 0 : self->len;
}

mcc_bool mcc_rb_map_is_empty(struct mcc_rb_map *self)
{
	return !self ? true : self->len == 0;
}

mcc_err mcc_rb_map_iter_init(struct mcc_rb_map *self,
			     struct mcc_rb_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iterator_next_fn)&mcc_rb_map_iter_next;
	iter->current = self->root;
	while (iter->current && iter->current->left)
		iter->current = iter->current->left;
	iter->container = self;
	return OK;
}

mcc_bool mcc_rb_map_iter_next(struct mcc_rb_map_iter *iter,
			      struct mcc_kv_pair *result)
{
	struct mcc_rb_node *curr, *tmp;
	mcc_compare_fn cmp;

	if (!iter || !iter->current)
		return false;

	curr = iter->current;

	if (result) {
		result->key = iter->current->key;
		result->value = value_of(iter->current);
	}

	if (curr->right) {
		/* Find the successor. */
		tmp = curr->right;
		while (tmp->left)
			tmp = tmp->left;
		iter->current = tmp;
	} else {
		tmp = curr->parent;
		cmp = iter->container->K->cmp;

		while (tmp && cmp(tmp->key, curr->key) <= 0)
			tmp = tmp->parent;
		iter->current = tmp;
	}
	return true;
}
