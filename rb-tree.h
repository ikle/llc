/*
 * Red-Black Tree
 *
 * Copyright (c) 2010,2015,2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RB_TREE_H
#define RB_TREE_H  1

typedef int  rb_compare_fn (const void *a, const void *b);
typedef void rb_free_fn (void *data);

struct rb_tree *rb_alloc (rb_compare_fn compare);
void rb_free (struct rb_tree *t, rb_free_fn free_data);

struct rb_node *rb_lookup (struct rb_tree *t, const void *data);
void *rb_data (struct rb_node *n);

/*
 * Returns NULL on error. If a node matching "data" already exists,
 * the existant node is returned. Otherwise, new node is returned.
 */
struct rb_node *rb_insert (struct rb_tree *t, void *data);
void *rb_delete (struct rb_tree *t, struct rb_node *n);

#endif  /* RB_TREE_H */
