/*
 * String Type
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_STRING_H
#define DATA_STRING_H  1

#include <data/type.h>

int string_eq (const void *a, const void *b);
size_t string_hash (const void *o);

#endif  /* DATA_STRING_H */
