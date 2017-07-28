/*
 * Red-Black Tree
 *
 * Copyright (c) 2010,2015,2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>

#include "rb-tree.h"

struct rb_node {
	struct rb_node *left, *right, *parent;
	enum rb_color { red, black } color;
	void *data;
};

void *rb_data (struct rb_node *n)
{
	return n->data;
}

struct rb_tree {
	int (*compare) (const void *a, const void *b);
	struct rb_node nil;  /* self-referencing sentinel nil node */
	struct rb_node hat;  /* fake root's parent node            */
};

static struct rb_node *rb_nil (struct rb_tree *t)
{
	return &t->nil;
}

static struct rb_node *rb_hat (struct rb_tree *t)
{
	return &t->hat;
}

#define rb_root(t)  ((t)->hat.left)

struct rb_tree *rb_alloc (rb_compare_fn compare)
{
	struct rb_tree *t;

	if ((t = malloc (sizeof (*t))) == NULL)
		return NULL;

	t->compare = compare;

	t->nil.left  = t->nil.right = t->nil.parent = &t->nil;
	t->nil.color = black;
	t->nil.data  = NULL;

	t->hat = t->nil;

	return t;
}

static void rb_free_node (struct rb_tree *t, struct rb_node *n,
			  rb_free_fn free_data)
{
	if (n == rb_nil (t))
		return;

	rb_free_node (t, n->left,  free_data);

	if (free_data != NULL)
		free_data (n->data);

	rb_free_node (t, n->right, free_data);

	free (n);
}

void rb_free (struct rb_tree *t, void (*free_data) (void *data))
{
	rb_free_node (t, rb_root (t), free_data);
	free (t);
}

struct rb_node *rb_lookup (struct rb_tree *t, const void *data)
{
	struct rb_node *n;
	int result;

	for (n = rb_root (t);
	     n != rb_nil (t);
	     n = result < 0 ? n->left : n->right)
		if ((result = t->compare (data, n->data)) == 0)
			return n;

	return NULL;
}

#define declare_helpers(left, right)					\
static void rotate_##left (struct rb_tree *t, struct rb_node *n)	\
{									\
	struct rb_node *child;						\
									\
	child = n->right;						\
	n->right = child->left;						\
									\
	if (child->left != rb_nil (t))					\
		child->left->parent = n;				\
									\
	child->parent = n->parent;					\
									\
	if (n == n->parent->left)					\
		n->parent->left  = child;				\
	else								\
		n->parent->right = child;				\
									\
	child->left = n;						\
	n->parent = child;						\
}									\
									\
static int rb_node_is_##left (struct rb_node *n)			\
{									\
	return n == n->parent->left;					\
}

declare_helpers (left, right)
declare_helpers (right, left)

static struct rb_node *rb_alloc_node (struct rb_tree *t,
				      struct rb_node *parent, void *data)
{
	struct rb_node *n;

	if ((n = malloc (sizeof (*n))) == NULL)
		return NULL;

	n->left = n->right = rb_nil (t);
	n->parent = parent;
	n->color  = red;
	n->data   = data;

	if (parent == rb_hat (t) || t->compare (data, parent->data) < 0)
		parent->left = n;
	else
		parent->right = n;

	return n;
}

#define declare_recolor(left, right)					\
static struct rb_node *							\
rb_recolor_on_insert_##left (struct rb_tree *t, struct rb_node *n)	\
{									\
	struct rb_node *uncle = n->parent->parent->right;		\
									\
	/* assert (n->parent->color == red); */				\
	/* assert (rb_node_is_##left (n->parent)); */			\
									\
	if (uncle->color == red) {					\
		/* case 3 */						\
		n->parent->color = black;				\
		uncle->color = black;					\
		n->parent->parent->color = red;				\
									\
		return n->parent->parent;				\
	}								\
									\
	if (rb_node_is_##right (n)) {					\
		/* case 4 */						\
		n = n->parent;						\
		rotate_##left (t, n);					\
	}								\
									\
	/* case 5 */							\
	n->parent->color = black;					\
	n->parent->parent->color = red;					\
									\
	rotate_##right (t, n->parent->parent);				\
	return n;							\
}

declare_recolor (left, right)
declare_recolor (right, left)

/*
 * Returns NULL on error. If a node matching "data" already exists,
 * the existant node is returned. Otherwise, new node is returned.
 */
struct rb_node *rb_insert (struct rb_tree *t, void *data)
{
	struct rb_node *n, *parent, *ret;
	int result;

	/* find insertion point */
	for (parent = rb_hat (t), n = rb_root (t);
	     n != rb_nil (t);
	     parent = n, n = result < 0 ? n->left : n->right)
		if ((result = t->compare (data, n->data)) == 0)
			return n;

	if ((n = rb_alloc_node (t, parent, data)) == NULL)
		return NULL;

	ret = n;

	while (n->parent->color == red)
		if (rb_node_is_left (n->parent))
			n = rb_recolor_on_insert_left  (t, n);
		else
			n = rb_recolor_on_insert_right (t, n);

	rb_root (t)->color = black;  /* root node is black always */
	return ret;
}

#define rb_recolor_on_delete_branch(t, n, left, right)  {	\
	struct rb_node *sibling = n->parent->right;		\
								\
	/* case 2 */						\
	if (sibling->color == red) {				\
		sibling->color = black;				\
		n->parent->color = red;				\
								\
		rotate_##left (t, n->parent);			\
		sibling = n->parent->right;			\
	}							\
								\
	/* case 3 & 4 */					\
	if (sibling->right->color == black &&			\
	    sibling->left->color == black) {			\
		sibling->color = red;				\
		n = n->parent;					\
		continue;  					\
	}							\
								\
	/* case 5 */						\
	if (sibling->right->color == black) {			\
		sibling->left->color = black;			\
		sibling->color = red;				\
								\
		rotate_##right (t, sibling);			\
		sibling = n->parent->right;			\
	}							\
								\
	/* case 6 */						\
	sibling->color = n->parent->color;			\
	n->parent->color = black;				\
	sibling->right->color = black;				\
								\
	rotate_##left (t, n->parent);				\
	break;							\
}

static void rb_recolor_on_delete (struct rb_tree *t, struct rb_node *n)
{
	while (n->color == black && n != rb_root (t))
		if (rb_node_is_left (n))
			rb_recolor_on_delete_branch (t, n, left, right)
		else
			rb_recolor_on_delete_branch (t, n, right, left)

	n->color = black;
}

static struct rb_node *rb_get_leftmost (struct rb_tree *t, struct rb_node *n)
{
	for (; n->left != rb_nil (t); n = n->left) {}

	return n;
}

void *rb_delete (struct rb_tree* t, struct rb_node* n)
{
	struct rb_node *x;
	void *data;

	if (n == NULL)
		return NULL;

	data = n->data;

	if (n->left != rb_nil (t) && n->right != rb_nil (t)) {
		x = rb_get_leftmost (t, n->right);
		n->data = x->data;
		n = x;
	}

	/* assert (n->left == rb_nil (t) || n->right == rb_nil (t)); */

	x = (n->left != rb_nil (t)) ? n->left : n->right;

	/* yes, x CAN be nil, store parent of deleted node in it */
	x->parent = n->parent;

	if (n == n->parent->left)
		n->parent->left  = x;
	else
		n->parent->right = x;

	if (n->color == black)
		rb_recolor_on_delete (t, x);

	free (n); 
	return data;
}
