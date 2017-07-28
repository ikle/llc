/*
 * Sequence: List or Stack
 *
 * Copyright (c) 2014 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef SEQ_H
#define SEQ_H  1

#include <assert.h>
#include <stddef.h>

#define SEQ_DECLARE(type)						\
struct type##_seq {							\
	struct type *head, **tail;					\
};									\
									\
static void type##_seq_init (struct type##_seq *s)			\
{									\
	assert (s != NULL);						\
									\
	s->head = NULL;							\
	s->tail = &s->head;						\
}									\
									\
void type##_seq_fini (struct type##_seq *s, void (*f) (struct type *))	\
{									\
	struct type *i, *next;						\
									\
	assert (s != NULL);						\
	assert (f != NULL);						\
									\
	for (i = s->head; i != NULL; i = next) {			\
		next = i->next;						\
		f (i);							\
	}								\
}									\
									\
static void type##_seq_push (struct type##_seq *s, struct type *i)	\
{									\
	assert (s != NULL);						\
	assert (i != NULL);						\
	assert (i->next == NULL);					\
									\
	i->next = s->head;						\
	if (s->head == NULL)						\
		s->tail = &i->next;					\
	s->head = i;							\
}									\
									\
static struct type *type##_seq_pop (struct type##_seq *s)		\
{									\
	struct type *i = s->head;					\
									\
	assert (s != NULL);						\
									\
	if (i == NULL)							\
		return NULL;						\
									\
	s->head = i->next;						\
	if (s->head == NULL)						\
		s->tail = &s->head;					\
									\
	return i;							\
}									\
									\
static void type##_seq_enqueue (struct type##_seq *s, struct type *i)	\
{									\
	assert (s != NULL);						\
	assert (i != NULL);						\
									\
	*s->tail = i;							\
	s->tail = &i->next;						\
}									\
									\
static struct type *type##_seq_dequeue (struct type##_seq *s)		\
{									\
	assert (s != NULL);						\
									\
	return type##_seq_pop (s);					\
}									\
									\
static int type##_seq_is_empty (struct type##_seq *s)			\
{									\
	assert (s != NULL);						\
									\
	return s->head == NULL;						\
}

#endif  /* SEQ_H */
