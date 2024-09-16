#include "mcc_map.h"
#include "mcc_err.h"
#include "mcc_utils.h"

struct mcc_rb_node {
	int color;
	struct mcc_rb_node *parent;
	struct mcc_rb_node *left;
	struct mcc_rb_node *right;
	struct mcc_kv_pair pair;
};

struct mcc_map {
	struct mcc_rb_node *root;
	size_t len;
	struct mcc_object_interface k;
	struct mcc_object_interface v;

	bool is_str_key;
	bool is_str_val;
};

static void mcc_map_dtor(void *self)
{
	mcc_map_delete(*(struct mcc_map **)self);
}

static int mcc_map_cmp(const void *self, const void *other)
{
	struct mcc_map *const *p1 = self;
	struct mcc_map *const *p2 = other;

	return mcc_size_t_i.cmp(&(**p1).len, &(**p2).len);
}

static size_t mcc_map_hash(const void *self)
{
	struct mcc_map *const *p = self;

	return mcc_str_i.hash((**p).k.name) * mcc_str_i.hash((**p).v.name) *
	       mcc_size_t_i.hash(&(**p).len);
}

const struct mcc_object_interface mcc_map_i = {
	.name = "struct mcc_map *",
	.size = sizeof(struct mcc_map *),
	.dtor = &mcc_map_dtor,
	.cmp = &mcc_map_cmp,
	.hash = &mcc_map_hash,
};

static inline bool rb_is_red(struct mcc_rb_node *node)
{
	return !node ? false : node->color == MCC_RB_RED;
}

static inline bool rb_is_black(struct mcc_rb_node *node)
{
	return !node ? true : node->color == MCC_RB_BLACK;
}

static inline bool rb_is_root(struct mcc_rb_node *node)
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

static struct mcc_rb_node *rb_node_new(const void *k, const void *v,
				       const struct mcc_map *map)
{
	struct mcc_rb_node *self;
	const struct mcc_object_interface *ki = &map->k;
	const struct mcc_object_interface *vi = &map->v;

	self = calloc(1, sizeof(struct mcc_rb_node));
	if (!self)
		return NULL;

	if (map->is_str_key) {
		self->pair.key = strdup(k);
		if (!self->pair.key)
			goto handle_key_allocate_err;
	} else {
		self->pair.key = malloc(ki->size);
		if (!self->pair.key)
			goto handle_key_allocate_err;
		memcpy(self->pair.key, k, ki->size);
	}

	if (map->is_str_val) {
		self->pair.value = strdup(v);
		if (!self->pair.value)
			goto handle_value_allocate_err;
	} else {
		self->pair.value = malloc(vi->size);
		if (!self->pair.value)
			goto handle_value_allocate_err;
		memcpy(self->pair.value, v, vi->size);
	}

	return self;

handle_value_allocate_err:
	if (ki->dtor)
		ki->dtor(self->pair.key);
	free(self->pair.key);
handle_key_allocate_err:
	free(self);
	return NULL;
}

static int rb_node_delete(struct mcc_rb_node *self,
			  const struct mcc_object_interface *ki,
			  const struct mcc_object_interface *vi,
			  const bool is_recursive)
{
	if (!self)
		return OK;

	if (is_recursive) {
		rb_node_delete(self->left, ki, vi, is_recursive);
		rb_node_delete(self->right, ki, vi, is_recursive);
	}

	if (ki->dtor)
		ki->dtor(self->pair.key);
	free(self->pair.key);

	if (vi->dtor)
		vi->dtor(self->pair.value);
	free(self->pair.value);

	free(self);
	return OK;
}

struct mcc_map *mcc_map_new(const struct mcc_object_interface *key,
			    const struct mcc_object_interface *value)
{
	struct mcc_map *self;

	if (!key || !value)
		return NULL;

	self = calloc(1, sizeof(struct mcc_map));
	if (!self)
		return NULL;

	self->k = *key;
	self->v = *value;
	self->is_str_key = !strcmp(key->name, "char *");
	self->is_str_val = !strcmp(value->name, "char *");
	return self;
}

void mcc_map_delete(struct mcc_map *self)
{
	if (!self)
		return;

	mcc_map_clear(self);
	free(self);
}

void mcc_map_clear(struct mcc_map *self)
{
	if (!self)
		return;

	rb_node_delete(self->root, &self->k, &self->v, true);
	self->root = NULL;
	self->len = 0;
}

int mcc_map_insert(struct mcc_map *self, const void *key, const void *value)
{
	struct mcc_rb_node **curr, *parent;
	int cmp_res;

	if (!self || !value)
		return INVALID_ARGUMENTS;

	curr = &self->root;
	parent = NULL;
	while (*curr) {
		parent = *curr;
		cmp_res = self->k.cmp(key, (*curr)->pair.key);
		if (cmp_res > 0)
			curr = &(*curr)->right;
		else if (cmp_res < 0)
			curr = &(*curr)->left;
		else { /* Update the value. */
			if (self->v.dtor)
				self->v.dtor((*curr)->pair.value);
			memcpy((*curr)->pair.value, value, self->v.size);
			return OK;
		}
	}

	*curr = rb_node_new(key, value, self);
	if (!*curr)
		return CANNOT_ALLOCATE_MEMORY;

	(*curr)->parent = parent;
	self->len += 1;
	rb_fix_insert(&self->root, *curr);
	return OK;
}

static inline void swap_key_value(struct mcc_rb_node *a, struct mcc_rb_node *b)
{
	struct mcc_kv_pair tmp = a->pair;
	a->pair = b->pair;
	b->pair = tmp;
}

void mcc_map_remove(struct mcc_map *self, const void *key)
{
	struct mcc_rb_node **curr, *tmp;
	int cmp_res;

	if (!self || !key)
		return;

	curr = &self->root;
	while (*curr) {
		cmp_res = self->k.cmp(key, (*curr)->pair.key);
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
		swap_key_value(*curr, tmp);
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

	rb_node_delete(tmp, &self->k, &self->v, false);
	self->len -= 1;
}

int mcc_map_get(struct mcc_map *self, const void *key, void *value)
{
	void *ptr;

	ptr = mcc_map_get_ptr(self, key);
	if (!ptr)
		return NONE;

	memcpy(value, ptr, self->v.size);
	return OK;
}

void *mcc_map_get_ptr(struct mcc_map *self, const void *key)
{
	struct mcc_rb_node *curr;
	int cmp_res;

	if (!self || !key)
		return NULL;

	curr = self->root;
	while (curr) {
		cmp_res = self->k.cmp(key, curr->pair.key);
		if (cmp_res > 0) {
			curr = curr->right;
		} else if (cmp_res < 0) {
			curr = curr->left;
		} else {
			return curr->pair.value;
		}
	}
	return NULL;
}

size_t mcc_map_len(struct mcc_map *self)
{
	return !self ? 0 : self->len;
}

bool mcc_map_is_empty(struct mcc_map *self)
{
	return !self ? true : self->len == 0;
}

int mcc_map_iter_init(struct mcc_map *self, struct mcc_map_iter *iter)
{
	if (!self || !iter)
		return INVALID_ARGUMENTS;

	iter->interface.next = (mcc_iter_next_f)&mcc_map_iter_next;
	iter->curr = self->root;
	while (iter->curr && iter->curr->left)
		iter->curr = iter->curr->left;
	iter->container = self;
	return OK;
}

bool mcc_map_iter_next(struct mcc_map_iter *iter, void *result)
{
	struct mcc_rb_node *tmp;
	struct mcc_rb_node *curr;
	mcc_compare_f cmp;

	if (!iter || !iter->curr)
		return false;

	curr = iter->curr;

	if (result)
		*(struct mcc_kv_pair *)result = curr->pair;

	if (curr->right) {
		/* Find the successor. */
		tmp = curr->right;
		while (tmp->left)
			tmp = tmp->left;
		iter->curr = tmp;
	} else {
		tmp = curr->parent;
		cmp = iter->container->k.cmp;

		while (tmp && cmp(tmp->pair.key, curr->pair.key) <= 0)
			tmp = tmp->parent;
		iter->curr = tmp;
	}
	return true;
}
