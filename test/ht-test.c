#include <err.h>
#include <stdio.h>
#include <string.h>

#include <data/ht.h>

static size_t str_hash (const void *o)
{
#if 1
	return ht_hash (0, o, strlen (o));
#else
	return *(const char *) o;  /* bad hash to test collisions */
#endif
}

static int str_eq (const void *a, const void *b)
{
	return strcmp (a, b) == 0;
}

static void str_free (void *o)
{
	/* do nothing with static strings */
}

static const struct ht_type str_type = {
	.hash	= str_hash,
	.eq	= str_eq,
	.free	= str_free,
};

int main (int argc, char *argv[])
{
	struct ht ht;
	size_t i;

	if (!ht_init (&ht, &str_type))
		err (1, "cannot initialize hash table");

	if (!ht_insert (&ht, "test string #1") ||
	    !ht_insert (&ht, "Lorem ipsum dolor sit amet") ||
	    !ht_insert (&ht, "consectetur adipiscing elit") ||
	    !ht_insert (&ht, "sed do eiusmod tempor incididunt") ||
	    !ht_insert (&ht, "ut labore et dolore magna aliqua") ||
	    !ht_insert (&ht, "test string #2"))
		err (1, "cannot insert string");

	if (ht_insert (&ht, "test string #1"))
		errx (1, "string exists, but not found");

	printf ("load factor = %zu%%\n\n", ht.count * 100 / ht.size);

	for (i = 0; i < ht.size; ++i)
		if (ht.table[i] != NULL)
			printf ("%2zu at %2zu: %s\n",
				str_hash (ht.table[i]) & (ht.size - 1), i,
				(const char *) ht.table[i]);

	ht_fini (&ht);
	return 0;
}
